#ifndef OPLIB_DS_MAP_H
#define OPLIB_DS_MAP_H

#include <functional>
#include <utility>
#include <stdexcept>

#include "RBTree.H"

namespace oplib
{
namespace ds
{

  template <
            typename Key,
            typename T,
            typename Comp = std::less<Key>,
            typename Alloc = std::allocator<T>
           >
  class Map
  {
   public:
    using key_type = Key;
    using mapped_type = T;
    using value_type = std::pair<const key_type, mapped_type>;
    using key_compare = Comp;
    using allocator_type = Alloc;

    struct KeyExtractor
    {
      key_type operator() (const value_type& val_)
      {
        return val_.first;
      }
    };

    class value_compare
    {
      key_compare _comp;
     public:
      value_compare(key_compare comp_) : _comp(comp_) {}
      bool operator() (const value_type& v1, const value_type& v2) const
      {
        return _comp(v1.first, v2.first);
      }
    };

   private:
    using imp_type = oplib::ds::RBTree<key_type, value_type, KeyExtractor, Comp, allocator_type>;

    imp_type _impl;

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

    // Constructor/Destructors
    explicit Map(const key_compare& comp_ = key_compare(),
                 const allocator_type& alloc_ = allocator_type())
    : _impl(comp_, alloc_)
    {}

    template <typename InputIterator>
    Map(InputIterator first_, InputIterator last_,
        const key_compare& comp_ = key_compare(),
        const allocator_type& alloc_ = allocator_type())
    : _impl(comp_, alloc_)
    {
      std::for_each(first_, last_, [&] (const auto& val_) {
                                     _impl.insertUnique(val_);
                                   });
    }

    Map(std::initializer_list<value_type> il,
        const key_compare& comp_ = key_compare(),
        const allocator_type& alloc_ = allocator_type())
    : _impl(il, comp_, alloc_)
    {}

    // Rule of 5
    Map(const Map& rhs_) : _impl(rhs_._impl) {}
    Map& operator = (const Map& rhs_)
    {
      _impl.operator = (rhs_._impl);
      return *this;
    }
    Map(Map&& rhs_) : _impl(std::move(rhs_._impl)) {}
    Map& operator = (Map&& rhs_)
    {
      _impl.operator = (std::move(rhs_._impl));
      return *this;
    }
    ~Map() {}

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

    key_compare key_comp() const { return _impl.key_comp(); }
    value_compare value_comp() const { return value_compare(_impl.key_comp()); }

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

    template <typename... Args>
    iterator emplace_hint(const_iterator pos_, Args&&... args_)
    { return _impl.emplaceUnique(pos_, std::forward<Args>(args_)...); }

    mapped_type& operator[] (const key_type& key_)
    { return (_impl.insertUnique(std::make_pair(key_, mapped_type())).first)->second; }

    mapped_type& at(const key_type& key_)
    {
      auto iter = _impl.find(key_);
      if (iter == _impl.end())
        throw std::out_of_range("key not present");
      else
      {
        return iter->second; 
      }
    }

    // TODO: duplicate code, need refator, refer to effective c++
    const mapped_type& at(const key_type& key_) const
    {
      auto iter = _impl.find(key_);
      if (iter == _impl.end())
        throw std::out_of_range();
      else
      {
        return iter->second; 
      }
    }

    size_type erase(const key_type& key_)
    { return _impl.erase(key_).second ? 1 : 0; }

    // Erase element at pos and return the next position
    iterator erase(const_iterator pos)
    { return _impl.erase(pos).first; }

    iterator erase(iterator first_, iterator last_)
    { return _impl.erase(first_, last_); }

    void clear()
    { _impl.clear(); }

    void swap(Map& rhs_)
    { _impl.swap(rhs_._impl); }

    const_iterator find(const key_type& val_) const
    { return _impl.find(val_); }

    iterator find(const key_type& val_)
    { return _impl.find(val_); }

    size_type count(const key_type& val_) const
    { return (_impl.find(val_) == _impl.cend()) ? 0 : 1; }

    const_iterator lower_bound(const key_type& key_) const
    { return _impl.lower_bound(key_); }

    iterator lower_bound(const key_type& key_)
    { return _impl.lower_bound(key_); }

    const_iterator upper_bound(const key_type& key_) const
    { return _impl.upper_bound(key_); }

    iterator upper_bound(const key_type& key_)
    { return _impl.upper_bound(key_); }

    std::pair<iterator, iterator> equal_range(const key_type& key_) 
    { return _impl.equal_range(key_); }

    std::pair<const_iterator, const_iterator> equal_range(const key_type& key_) const
    { return _impl.equal_range(key_); }
  };


}
}

#endif
