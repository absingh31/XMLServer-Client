// vectors are lifeline of C++ programming, 
// If we can't use the STL version, let's make our own version of it
/*
@Functionality: Basic vector class
@Author: Abhishek Singh
*/

#include <new>
#include <cassert>

#pragma once
/* 
@ Public APIs implemented with this vector class
1). push_back
2). size
3). [] ==> for indexing
4). pop_back
5). clear
*/


template<class T>
class abvector
{
public:
    abvector() 
    {
        // STL vector takes the default preallocation size as 2 but 
        // I'd rather allocate enough memory rather than reallocating again and again
        abAlloc(15);
    }

    ~abvector() 
    {
        clear();
        ::operator delete(data_, capacity_ * sizeof(T));
    }

    void abAlloc(size_t newCapacity) 
    {
        T* newDataBlock = (T*)::operator new(newCapacity * sizeof(T));

        for (size_t idx = 0; idx < size_; idx++) 
        {
            newDataBlock[idx] = std::move(data_[idx]);
        }

        for (size_t idx = 0; idx < size_; idx++) 
        {
            data_[idx].~T();
        }

        ::operator delete(data_, capacity_ * sizeof(T));
        data_ = newDataBlock;
        capacity_ = newCapacity;
    }

    void push_back(const T& value) 
    {
        if (size_ >= capacity_) abAlloc(2*capacity_);
        data_[size_++] = value;
    }

    void push_back(T&& value) 
    {
        if (size_ >= capacity_) abAlloc(2*capacity_);

        data_[size_++] = std::move(value);
    }

    size_t size() const { return size_; }

    const T& operator[](const size_t index) const 
    {
        assert(index < size_);

        return data_[index];
    }

    T& operator[](const size_t index) 
    {
        assert(index < size_);

        return data_[index];
    }

    void pop_back() 
    {
        if (size_ > 0) 
        {
            size_--;
            data_[size_].~T();
        }
    }

    void clear() 
    {
        for (size_t idx = 0; idx < size_; idx++) 
        {
            data_[idx].~T();
        }

        size_ = 0;
    }

private:
    T* data_ = nullptr; // pointer to array
    size_t size_ = 0;
    size_t capacity_ = 0;
};



