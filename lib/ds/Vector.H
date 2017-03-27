#ifndef OPLIB_DS_VECTOR_H_
#define OPLIB_DS_VECTOR_H_

#include <algorithm>
#include <iterator>
#include <utility>
#include <boost/compressed_pair.hpp>
#include <iostream>
#include <memory>
#include <type_traits>
#include <stdexcept>
#include <limits>

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
    typedef std::reverse_iterator<iterator> reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
    typedef Alloc allocator_type;

   protected:
    //Alloc allocator;
    iterator start { nullptr };
    iterator finish { nullptr };
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
      cp_.first() = finish;
    }

    template<typename... Args>
    void emplaceAux(iterator position_, Args&&... args);
   
    void insertAux(iterator position_, const value_type& x_)
    { emplaceAux(position_, x_); }

    // These two constructIterDispatch methods
    // needs to guarantee exception safety: commit-or-rollback
    template <typename Iter>
    void
    constructIterDispatch(Iter first, Iter last, std::input_iterator_tag)
    {
      size_t sz = std::distance(first, last);
      auto ps = getAllocator().allocate(sz);
      auto pf = ps;
      try {
        // If exception thrown, we need to do cleanup
        pf = std::uninitialized_copy_n(first, sz, ps);
      }
      catch (...)
      {
        destroy(ps, pf);
        getAllocator().deallocate(pf, sz);
        throw;
      }

      destroy(begin(), end());
      deallocate();

      start = ps;
      finish = pf;
      cp_.first() = finish;
    }

    template <typename Iter>
    void
    constructIterDispatch(Iter first, Iter last, std::random_access_iterator_tag)
    {
      Vector vtmp;
      for (; first != last; ++first) vtmp.push_back(*first); 

      // The swap idiom: for exception safety
      vtmp.swap(*this);
    }

    void rangeCheck(size_type n)
    { 
      if (n >= size())
      {
        throw std::range_error("Invalid index");
      }
    }

   public:

    allocator_type get_allocator() const
    { return cp_.second(); }

    iterator begin() const { return start;  }
    iterator end() const { return finish; }
    const_iterator cbegin() const { return start; }
    const_iterator cend() const { return finish; }

    // Reverse iterator will call -- on the passed iterator
    reverse_iterator rbegin() const 
    { return reverse_iterator(end()); }
    reverse_iterator rend() const 
    { return reverse_iterator(begin()); }
    const_reverse_iterator crbegin() const
    { return const_reverse_iterator(cend()); }
    const_reverse_iterator crend() const
    { return const_reverse_iterator(cbegin()); }

    reference front() { return *begin(); }
    reference back() { return *(end() - 1); }

    size_type size() const { return size_type(cend() - cbegin()); }
    size_type max_size() const { return std::numeric_limits<size_type>::max(); }
    size_type capacity() const { return size_type(cp_.first() - begin()); }
    bool empty() const { return start == finish; }
    value_type* data() noexcept { return start; }
    const value_type* data() const noexcept { return start; }

    reference operator[] (size_type n) { return *(begin() + n); }
    const_reference operator[] (size_type n) const { return *(begin() + n); }

    reference at (size_type n) { rangeCheck(n); return *(begin() + n); }
    const_reference at (size_type n) const { rangeCheck(n); return *(begin() + n); }

    // Constructors
    Vector(const Alloc& alloc = Alloc()) : cp_(nullptr, alloc) { }
    Vector(size_type n, const T& value, const Alloc& alloc = Alloc())
    : cp_(nullptr, alloc) { fillInitialize(n, value); }
    explicit Vector(size_type n, const Alloc& alloc = Alloc())
    : cp_(nullptr, alloc) { fillInitialize(n, T()); }
    Vector(std::initializer_list<value_type> il, const Alloc& alloc = Alloc())
    : Vector(il.begin(), il.end()) {}

    // use SFINAE to de-initialize iterator-based constructors
    // when parameter passed in are integers template <typename InputIter, std::enable_if_t<
    template <
              typename InputIter, 
              std::enable_if_t<
               std::is_base_of<
                 std::input_iterator_tag, 
                 typename std::iterator_traits<InputIter>::iterator_category
                              >::value, 
              int> = 0
             >
    Vector(InputIter first, InputIter last, const Alloc& alloc = Alloc())
    : cp_(nullptr, alloc)
    {
      using IterTag = typename std::iterator_traits<InputIter>::iterator_category;
      std::pair<iterator, iterator> p { nullptr, nullptr };

      // constructIterDispatch() is exception-safe:
      // conforms to the commit-or-rollback semantic
      constructIterDispatch(first, last, IterTag());
    }

    // Assign: similar to ctors
    template <
              typename InputIter, 
              std::enable_if_t<
               std::is_base_of<
                 std::input_iterator_tag, 
                 typename std::iterator_traits<InputIter>::iterator_category
                              >::value, 
              int> = 0
             >
    void assign(InputIter first, InputIter last)
    {
      // For exception safety:
      // if exception happend, the vector is kept unchanged
      using IterTag = typename std::iterator_traits<InputIter>::iterator_category;
      constructIterDispatch(first, last, IterTag());
    }

    void assign(size_type n, const T& value)
    {
      Vector vtmp(n, value);
      vtmp.swap(*this);
    }

    void assign(std::initializer_list<value_type> il)
    { assign(il.begin(), il.end()); }
  
    // Copy/dtor/assign/move rule of five
    ~Vector()
    {
      destroy(begin(), end());
      deallocate();
    }

    Vector(const Vector<T, Alloc>& v);
    Vector<T, Alloc>& operator = (const Vector<T, Alloc>& rhs);
    Vector(Vector<T, Alloc>&& v);
    Vector<T, Alloc>& operator = (Vector<T, Alloc>&& rhs);

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

    void push_back(T&& x)
    { emplace_back(std::move(x)); }

    template <typename... Args>
    void emplace_back(Args&&... args)
    {
      if (finish != cp_.first())
      {
        getAllocator().construct(finish, std::forward<Args>(args)...);
        ++finish;
      }
      else
      {
        // Remember to call std::forward<Args>(args)...!  
        emplaceAux(end(), std::forward<Args>(args)...); 
      }
    }

    template <typename... Args>
    void emplace(iterator position_, Args&&... args)
    { emplaceAux(position_, std::forward<Args>(args)...); }

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
