#ifndef VECTOR_H_
#define VECTOR_H_

#include "alloc.H"
#include "memutil.H"

// Will be replaced later
#include <algorithm>
#include <utility>
#include <boost/compressed_pair.hpp>

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
        typedef const value_type& const_reference;
        typedef const value_type* const_iterator;

    protected:
        //Alloc allocator;
        iterator start = nullptr;
        iterator finish = nullptr;
        //iterator eos; // current tail of usable storage
        boost::compressed_pair<iterator, Alloc> cp;

        void deallocate()
        {
            if (start)
                cp.second().deallocate(start, cp.first() - start);
        }

        iterator allocate_and_fill(size_type n, const T& x)
        {
            iterator result = cp.second().allocate(n);
            oplib::uninitialized_fill_n(result, n, x);
            return result;
        }

        void fill_initialize(size_type n, const T& value)
        {
            start = allocate_and_fill(n, value);
            finish = start + n;
            cp.first() = finish;
        }

        void insert_aux(iterator position, const value_type& x);

    public:
        iterator begin() const { return start;  }
        iterator end() const { return finish; }
        const_iterator cbegin() const { return start; }
        const_iterator cend() const { return finish; }
        size_type size() const { return size_type(cend() - cbegin()); }
        size_type capacity() { return size_type(cp.first() - begin()); }
        bool empty() const { return start == finish; }
        reference operator[] (size_type n) { return *(begin() + n); }

        // ctor and dtors
        Vector(const Alloc& alloc = Alloc()) : cp(nullptr, alloc) { }
        Vector(size_type n, const T& value, const Alloc& alloc = Alloc())
          : cp(nullptr, alloc) { fill_initialize(n, value); }
        explicit Vector(size_type n, const Alloc& alloc = Alloc())
          : cp(nullptr, alloc) { fill_initialize(n, T()); }
    
        // Copy and assign
        Vector(const Vector<T, Alloc>& v);
        Vector<T, Alloc>& operator = (const Vector<T, Alloc>& rhs);

        ~Vector()
        {
            cp.second().destroy(begin(), end());
            deallocate();
        }

        reference front() { return *begin(); }
        reference back() { return *(end() - 1); }

        void pop_back()
        {
            --finish;
            cp.second().destroy(finish);
        }

        void push_back(const T& x)
        {
            if (finish != cp.first())
            {
                cp.second().construct(finish, x);
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
            cp.second().destroy(i, finish);
            finish = finish - (last - first);
            return first;
        }
 
        iterator erase(iterator position)
        {
            if (position + 1 != finish)
                std::copy(position + 1, finish, position);
            finish -= 1;
            cp.second().destroy(finish);
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
