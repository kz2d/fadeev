#pragma once

template<class T>
class Allocator;

class Block {
public:
    using size_type = size_t;
    using difference_type = int64_t;
    using pointer = char*;

    Block(size_type count, size_type size);
    ~Block();
    bool is_full() const noexcept;
    size_type get_size() const noexcept;
    size_type get_count() const noexcept;
    pointer get_start() const noexcept;
    void set_start(pointer p) noexcept;
    pointer insert() noexcept;
    void erase(pointer p) noexcept;

    bool operator< (const Block& other) const noexcept;
private:
    pointer start;
    size_type size;
    size_type chunk_count;
    bool* chunk_state;
    size_type free_chunks;
    size_type predict;
};

Block::Block(size_type count, size_type size) : size(size), chunk_count(count), free_chunks(count), predict(0){
    chunk_state = new bool[chunk_count];
    for (int i = 0; i < chunk_count; ++i) {
        chunk_state[i] = true;
    }
}

Block::~Block(){
    delete[] chunk_state;
}

bool Block::is_full() const noexcept{
    return free_chunks == 0;
}

typename Block::size_type Block::get_size() const noexcept{
    return size;
}

typename Block::size_type Block::get_count() const noexcept{
    return chunk_count;
}

typename Block::pointer Block::get_start() const noexcept{
    return start;
}

void Block::set_start(Block::pointer p) noexcept{
    start = p;
}

typename Block::pointer Block::insert() noexcept{
    pointer p = nullptr;
    for (int i = predict; i < chunk_count; ++i) {
        if (chunk_state[i]) {
            p = start + size * i;
            chunk_state[i] = false;
            predict = i + 1;
            --free_chunks;
            break;
        }
    }
    
    if (p == nullptr) {
        for (int i = 0; i < predict; ++i) {
            if (chunk_state[i]) {
                p = start + size * i;
                chunk_state[i] = false;
                predict = i + 1;
                --free_chunks;
                break;
            }
        }
    }
    return p;
}

void Block::erase(Block::pointer p) noexcept{
    difference_type diff = (p - start) / size;
    if (diff >= 0 && diff < chunk_count && !chunk_state[diff]) {
        chunk_state[diff] = true;
        predict = diff;
        ++free_chunks;
    }
}

bool Block::operator< (const Block& other) const noexcept {
    return size < other.size;
}

