
template <class T>
class shared_ptr {
public:
    using pointer = T*;
    
    shared_ptr() {
        count = new int(0);
    }

    shared_ptr& operator=(const shared_ptr& other) {
        if (this == &other) {return *this;}
        ptr = other.ptr; 
        count = other.count;
        ++(*count);
        return *this;
    }

    void init (pointer p) {
        ptr = p;
        ++(*count);
    }

    void clean() {
        if (*count == 1) {
            operator delete[](static_cast<void*>(ptr));
            delete count;
        } else {
            --(*count);
        }
    }

    operator pointer() const {
        return ptr;
    }

private:
    pointer ptr = nullptr;
    int* count;
};