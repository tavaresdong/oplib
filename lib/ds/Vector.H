#ifndef VECTOR_H_
#define VECTOR_H_

#include <algorithm>
#include <algorithm>
#include <utility>
#include <boost/compressed_pair.hpp>
#include <iostream>

namespace oplib
{
namespace ds
{
  template <typename T, typename Alloc = std::allocator<T> >
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
    boost::compressed_pair<iterator, Alloc> cp_;

    decltype(auto) getAllocator() { return cp_.second(); }

    void deallocate()
    { if (start) getAllocator().deallocate(start, cp_.first() - start); }

    void destroy(iterator beg_, iterator end_)
    { for (; beg_ < end_; ) getAllocator().destroy(beg_++); }

    void fillInitialize(size_type n_, const T& value_)
    {
      start = getAllocator().allocate(n_);
      std::uninitialized_fill_n(start, n_, value_);
      finish = start + n_;
      //cp_.first() = finish;
      cp_.first() = finish;
    }

    void insertAux(iterator position_, const value_type& x_);

   public:
    iterator begin() const { return start;  }
    iterator end() const { return finish; }
    const_iterator cbegin() const { return start; }
    const_iterator cend() const { return finish; }
    size_type size() const { return size_type(cend() - cbegin()); }
    size_type capacity() const { return size_type(cp_.first() - begin()); }
    bool empty() const { return start == finish; }
    reference operator[] (size_type n) { return *(begin() + n); }

    // ctor and dtors
    Vector(const Alloc& alloc = Alloc()) : cp_(nullptr, alloc) { }
    Vector(size_type n, const T& value, const Alloc& alloc = Alloc())
    : cp_(nullptr, alloc) { fillInitialize(n, value); }
    explicit Vector(size_type n, const Alloc& alloc = Alloc())
    : cp_(nullptr, alloc) { fillInitialize(n, T()); }
  
    // Copy and assign
    Vector(const Vector<T, Alloc>& v);
    Vector<T, Alloc>& operator = (const Vector<T, Alloc>& rhs);

    ~Vector()
    {
      destroy(begin(), end());
      deallocate();
    }

    reference front() { return *begin(); }
    reference back() { return *(end() - 1); }

    void pop_back()
    {
      --finish;
      getAllocator().destroy(finish);
    }

    void push_back(const T& x)
    {
      if (finish != cp_.first())
      {
        getAllocator().construct(finish, x);
        ++finish;
      }
      else
      {
        insertAux(end(), x);
      }
    }

    iterator erase(iterator first, iterator last)
    {
      // TODO: move if nothrow movable
      iterator i = std::copy(last, finish, first);
      destroy(i, finish);
      finish = finish - (last - first);
      return first;
    }

    iterator erase(iterator position)
    {
      if (position + 1 != finish)
        std::copy(position + 1, finish, position);
      finish -= 1;
      getAllocator().destroy(finish);
      return position;
    }

    void resize(size_type new_size, const T& x);

    // TODO investigate the property of reserve()
    void reserve(size_type sz);

    void swap(Vector& v);

    // TODO investigate the property of shrink_to_fit()
    void shrink_to_fit()
    {
      if (size() < capacity())
      {
        Vector<T, Alloc>(*this).swap(*this);
      }
    }

    void clear()
    { erase(begin(), end()); }

    void insert(iterator position, size_type n, const T& x);
  };

  // Oveload swap() for Vector
  template <typename T, typename Alloc>
  void swap(Vector<T, Alloc>& v1, Vector<T, Alloc>& v2)
  { v1.swap(v2); }

  #include "VectorT.C"
}
}

#endif
