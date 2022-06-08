#pragma once

#include <vector>
#include <numeric>
#include <algorithm>
#include "block.h"
#include "shared_ptr.h"

// #define DEBUG 

const size_t DEFAULT_CHUNK_COUNT = 10;
const size_t DEFAULT_BLOCK_COUNT = 10;

template <class T>
class Allocator {
    template<class U, class V>
    friend bool operator==(const Allocator<U>& lhs, const Allocator<V>& rhs);

    template<class U, class V>
    friend bool operator!=(const Allocator<U>& lhs, const Allocator<V>& rhs);
public:
    using value_type = T;
    using pointer = T*;
    using const_poiter = const T*;
    using size_type = size_t;

    template <class U>
    struct rebind {
        using other = Allocator<U>;
    };

    template <class U>
    friend class Allocator;

    template <class U>
    Allocator(const Allocator<U>& other) {
        block = other.block;
        start = other.start;
        block_count = other.block_count;
    }

    Allocator();
    Allocator(const size_type* chunk_size, const size_type* chunk_count, size_type block_count);
    ~Allocator();
    
    pointer allocate(size_type n);
    void deallocate(pointer p, size_type n) noexcept;

private:
    void construct_block_structure(size_t total_size);

    shared_ptr<Block> block;
    shared_ptr<char> start;
    size_type block_count;
};

template<class T>
Allocator<T>::Allocator() {
    size_type total = 0;
    size_type chunk_size = 8;
    block_count = DEFAULT_BLOCK_COUNT;
    block.init(static_cast<Block*> (operator new[](block_count * sizeof(Block))));
    for (int i = 0; i < block_count; ++i) {
        new(&block[i]) Block(DEFAULT_CHUNK_COUNT, chunk_size);
        total += DEFAULT_CHUNK_COUNT * chunk_size;
        chunk_size *= 2;
    }
    construct_block_structure(total);
}

template<class T>
Allocator<T>::Allocator(const size_type* chunk_size, const size_type* chunk_count, size_type block_count) : block_count(block_count) {
    size_type total = 0;
    block.init(static_cast<Block*> (operator new[](block_count * sizeof(Block))));
    for (int i = 0; i < block_count; ++i) {
        new(&block[i]) Block(chunk_count[i], chunk_size[i]);
        total += chunk_count[i] * chunk_size[i];
    }
    std::sort((Block*)block, (Block*)block + block_count);
    construct_block_structure(total);
}

template<class T>
void Allocator<T>::construct_block_structure(size_t total_size) {
    start.init(static_cast<char*> (operator new[](total_size)));
    char* local_start = start;

    for (int i = 0; i < block_count; ++i) {
        block[i].set_start(local_start);
        local_start += block[i].get_size() * block[i].get_count();
    }
}

template<class T>
Allocator<T>::~Allocator() {
    #ifdef DEBUG
        std::cout << "Destructor\n";
    #endif

    block.clean();
    start.clean();
}

template<class T>
typename Allocator<T>::pointer Allocator<T>::allocate(size_type n) {
    #ifdef DEBUG
        std::cout << "Try to allocate " << n << " elements of size " << sizeof(T) << " byte(s) (";
    #endif
    size_type total = n * sizeof(T);
    for (int i = 0; i < block_count; ++i) {
        if (block[i].get_size() >= total && !block[i].is_full()) {
            #ifdef DEBUG
                std::cout << "size of block is " << block[i].get_size() << ")\n";
            #endif
            return (pointer) (block[i].insert());
        }
    }
    throw std::runtime_error("Not enough space in allocator");
}

template<class T>
void Allocator<T>::deallocate(pointer p, size_type n) noexcept {
    #ifdef DEBUG
        std::cout << "Try to deallocate " << n << " elements ";
    #endif

    Block::pointer b_start, bp = (Block::pointer)p;
    size_type count, size;

    for (int i = 0; i < block_count; ++i) {
        count = block[i].get_count();
        size = block[i].get_size();
        b_start = block[i].get_start();
        if (b_start <= bp && bp <= b_start + count * size && ((bp - b_start) % size) == 0  && n * sizeof(T) <= size) {
            block[i].erase(bp);
            #ifdef DEBUG
                std::cout << "(Success)\n";
            #endif
            return;
        }
    }

    #ifdef DEBUG
        std::cout << "(Error)\n";
    #endif
    return;
}

template<class T, class U>
bool operator==(const Allocator<T>& lhs, const Allocator<U>& rhs) {
    return rhs.block == lhs.block && rhs.blocks_count == lhs.blocks_count;
}

template<class T, class U>
bool operator!=(const Allocator<T>& lhs, const Allocator<U>& rhs) {
    return !(rhs == lhs);
}


