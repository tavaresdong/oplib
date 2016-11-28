#ifndef VECTOR_H_
#define VECTOR_H_

#include "alloc.H"
#include "memutil.H"

// Will be replaced later
#include <algorithm>
#include <utility>

namespace oplib
{
    template <typename T, typename Alloc = opallocator<T> >
    class Vector
    {
    public:
        typedef T  value_type;
        typedef value_type* pointer;
        typedef value_type* iterator;
        typedef value_type& reference;
        typedef size_t size_type;
        typedef ptrdiff_t difference_type;

    protected:
        Alloc allocator;
        iterator start; // current head
        iterator finish; // current tail
        iterator eos; // current tail of usable storage

        void deallocate()
        {
            if (start)
                allocator.deallocate(start, eos - start);
        }

        iterator allocate_and_fill(size_type n, const T& x)
        {
            iterator result = allocator.allocate(n);
            uninitialized_fill_n(result, n, x);
            return result;
        }

        void fill_initialize(size_type n, const T& value)
        {
            start = allocate_and_fill(n, value);
            finish = start + n;
            eos = finish;
        }

        void insert_aux(iterator position, const value_type& x);

    public:
        iterator begin() { return start;  }
        iterator end() { return finish; }
        size_type size() { return size_type(end() - begin()); }
        size_type capacity() { return size_type(eos - begin()); }
        bool empty() const { return begin() == end(); }
        reference operator[] (size_type n) { return *(begin() + n); }

        // ctor and dtors
        Vector() : start(nullptr), finish(nullptr), eos(nullptr) { }
        Vector(size_type n, const T& value) { fill_initialize(n, value); }
        explicit Vector(size_type n) { fill_initialize(n, T()); }
    
        // Copy and assign
        Vector(const Vector<T, Alloc>& v);
        Vector<T, Alloc>& operator = (const Vector<T, Alloc>& rhs);

        ~Vector()
        {
            allocator.destroy(begin(), end());
            deallocate();
        }

        reference front() { return *begin(); }
        reference back() { return *(end() - 1); }

        void pop_back()
        {
            --finish;
            allocator.destroy(finish);
        }

        void push_back(const T& x)
        {
            if (finish != eos)
            {
                allocator.construct(finish, x);
                ++finish;
            }
            else
            {
                insert_aux(end(), x);
            }
        }

        iterator erase(iterator first, iterator last)
        {
            iterator i = std::copy(last, finish, first);
            allocator.destroy(i, finish);
            finish = finish - (last - first);
            return first;
        }
 
        iterator erase(iterator position)
        {
            if (position + 1 != finish)
                std::copy(position + 1, finish, position);
            finish -= 1;
            allocator.destroy(finish);
            return position;
        }

        void resize(size_type new_size, const T& x);

        void clear()
        {
             erase(begin(), end());
        }

        void insert(iterator position, size_type n, const T& x);
    };

    #include "vectorT.C"
}


#endif
