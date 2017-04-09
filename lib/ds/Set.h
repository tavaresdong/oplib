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
    using reverse_iterator = typename imp_type::reverse_iterator;
    using const_reverse_iterator = typename imp_type::const_reverse_iterator;
    using size_type = typename imp_type::size_type;


   public:
    // Constructor/Destructors
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

    Set(std::initializer_list<value_type> il,
        const key_compare& comp_ = key_compare(),
        const allocator_type& alloc_ = allocator_type())
    : _impl(il, comp_, alloc_)
    {}

    // Rule of 5
    Set(const Set& rhs_) : _impl(rhs_._impl) {}
    Set& operator = (const Set& rhs_)
    {
      _impl.operator = (rhs_._impl);
      return *this;
    }
    Set(Set&& rhs_) : _impl(std::move(rhs_._impl)) {}
    Set& operator = (Set&& rhs_)
    {
      _impl.operator = (std::move(rhs_._impl));
      return *this;
    }
    ~Set() {}


   public:

    // Capacity 
    size_type size() const { return _impl.size(); }
    bool empty() const { return _impl.empty(); }
    size_type max_size() const { return _impl.max_size(); }

    // Iterators
    iterator begin() { return _impl.begin(); }
    iterator end() { return _impl.end(); }
    const_iterator cbegin() const { return _impl.cbegin(); }
    const_iterator cend() const { return _impl.cend(); }
    reverse_iterator rbegin() { return _impl.rbegin(); }
    reverse_iterator rend() { return _impl.rend(); }
    const_reverse_iterator crbegin() const { return _impl.crbegin(); }
    const_reverse_iterator crend() const { return _impl.crend(); }

    // Note: When accessing a member from a const member function,
    // the member automatically became const, so _impl inside
    // the method is regarded as const
    key_compare key_comp() const { return _impl.key_comp(); }
    value_compare value_comp() const { return _impl.key_comp(); }

    // Modifiers
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

    template <typename... Args>
    std::pair<iterator, bool> emplace(Args&&... args_)
    { return _impl.emplaceUnique(std::forward<Args>(args_)...); }


    size_type erase(const value_type& val_)
    { return _impl.erase(val_).second ? 1 : 0; }

    // Erase element at pos and return the next position
    iterator erase(const_iterator pos)
    { return _impl.erase(pos).first; }

    iterator erase(iterator first_, iterator last_)
    { return _impl.erase(first_, last_); }

    const_iterator find(const value_type& val_) const
    { return _impl.find(val_); }

    iterator find(const value_type& val_)
    { return _impl.find(val_); }

    size_type count(const value_type& val_) const
    { return (_impl.find(val_) == _impl.cend()) ? 0 : 1; }

    const_iterator lower_bound(const value_type& val_) const
    { return _impl.lower_bound(val_); }

    iterator lower_bound(const value_type& val_)
    { return _impl.lower_bound(val_); }

    const_iterator upper_bound(const value_type& val_) const
    { return _impl.upper_bound(val_); }

    iterator upper_bound(const value_type& val_)
    { return _impl.upper_bound(val_); }

    std::pair<iterator, iterator> equal_range(const value_type& val_) 
    { return _impl.equal_range(val_); }

    std::pair<const_iterator, const_iterator> equal_range(const value_type& val_) const

    { return _impl.equal_range(val_); }

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
