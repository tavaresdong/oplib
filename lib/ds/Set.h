#ifndef OPLIB_DS_SET_H
#define OPLIB_DS_SET_H

#include <functional>
#include <algorithm>

#include "RBTree.H"

namespace oplib
{
namespace ds
{

namespace detail
{
  template <typename T>
  struct Identity 
  {
    const T& operator () (const T& val_)
    { return val_; }
  };
}

  template <
            typename T, 
            typename Comp = std::less<T>,
            class Alloc = std::allocator<T>
           >
  class Set
  {
   public:
    using key_type = T;
    using value_type = T;
    using key_compare = Comp;
    using value_compare = Comp;
    using allocator_type = Alloc;
   private:
    using imp_type = oplib::ds::RBTree<T, T, detail::Identity<T>, Comp, Alloc>;
   public:
    using reference = typename imp_type::reference;
    using const_reference = typename imp_type::const_reference;
    using pointer = typename imp_type::pointer;
    using const_pointer = typename imp_type::const_pointer;
    using iterator = typename imp_type::iterator;
    using const_iterator = typename imp_type::const_iterator;
    using size_type = typename imp_type::size_type;


   public:
    // Constructor/Destructors
    // TODO copy construction move constructor
    explicit Set(const key_compare& comp_ = key_compare(),
                 const allocator_type& alloc_ = allocator_type())
    : _impl(comp_, alloc_)
    {}

    template <typename InputIterator>
    Set(InputIterator first_, InputIterator last_,
        const key_compare& comp_ = key_compare(),
        const allocator_type& alloc_ = allocator_type())
    : _impl(comp_, alloc_)
    {
      std::for_each(first_, last_, [&] (const auto& val_) {
                                     _impl.insertUnique(val_);
                                   });
    }

    ~Set() {}

   public:

    // Capacity 
    size_type size() const { return _impl.size(); }
    bool empty() const { return _impl.empty(); }
    size_type max_size() const { return _impl.max_size(); }

    // Iterators
    // TODO rbegin rend crbegin crend
    iterator begin() { return _impl.begin(); }
    iterator end() { return _impl.end(); }
    const_iterator cbegin() const { return _impl.cbegin(); }
    const_iterator cend() const { return _impl.cend(); }

    // Note: When accessing a member from a const member function,
    // the member automatically became const, so _impl inside
    // the method is regarded as const
    key_compare key_comp() const { return _impl.key_comp(); }
    value_compare value_comp() const { return _impl.key_comp(); }

    // Modifiers
    // TODO insert an rvalue insert a init-list 
    std::pair<iterator, bool> insert(const value_type& val_)
    { return _impl.insertUnique(val_); }

    iterator insert(const_iterator position, const value_type& val_)
    { return _impl.insertUnique(position, val_); }

    template <typename InputIterator>
    void insert(InputIterator first_, InputIterator last_)
    {
      std::for_each(first_, last_, [&] (const auto& val_) {
                                     _impl.insertUnique(val_);
                                   });
    }

    void clear()
    { _impl.clear(); }

    void swap(Set& rhs_)
    { _impl.swap(rhs_._impl); }

   private:
    imp_type _impl;
  };

  // Oveload swap() for Set 
  template <typename T, typename Comp, typename Alloc>
  void swap(Set<T, Comp, Alloc>& v1, Set<T, Comp, Alloc>& v2)
  { v1.swap(v2); }
}
}
#endif
