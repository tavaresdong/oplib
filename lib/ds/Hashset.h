#pragma once
#ifndef OPLIB_DS_HASHSET_H
#define OPLIB_DS_HASHSET_H

#include "Hashtable.h"

#include <functional>

namespace oplib
{
namespace ds
{
  template <
            typename Key,
            typename Hash = std::hash<Key>,
            typename Pred = std::equal_to<Key>,
            typename Alloc = std::allocator<Key>
           >
  class Hashset
  {
   public:
    using key_type = Key;
    using value_type = Key;
    using hasher = Hash;
    using key_equal = Pred;
    using allocator_type = Alloc;

   private:

    struct Identity
    {
      key_type operator() (const value_type& val_) const
      { return val_; } 
    };
    using imp_type = oplib::ds::Hashtable<value_type, key_type, hasher, Identity, key_equal, allocator_type>;

    imp_type _impl;

   public:
    using reference = typename imp_type::reference;
    using const_reference = typename imp_type::const_reference;
    using pointer = typename imp_type::pointer;
    using const_pointer = typename imp_type::const_pointer;
    using size_type = typename imp_type::size_type;
    
    using iterator = typename imp_type::iterator;
    using const_iterator = typename imp_type::const_iterator;

    // capacity
    bool empty() const
    { return _impl.empty(); }

    size_type size() const
    { return _impl.size(); }

    size_type max_size() const
    { return _impl.max_size(); }

    // Iterators
    iterator begin()
    { return _impl.begin(); }

    iterator end()
    { return _impl.end(); }

    // Constructor/Destructor
    // TODO copy/assign
    explicit Hashset(size_type n_,
                     const hasher& hf_ = hasher(),
                     const key_equal& eq_ = key_equal(),
                     const allocator_type& alloc_ = allocator_type())
    : _impl(n_, hf_, eq_, alloc_)
    {}

    explicit Hashset(const allocator_type& alloc_)
    : _impl(0, hasher(), key_equal(), alloc_)
    {}

    ~Hashset() {}

    // Accessors
    iterator find(const key_type& key_)
    { return _impl.find(key_); }

    const_iterator find(const key_type& key_) const
    { return _impl.find(key_); }

    size_type count(const key_type& key_)
    { return _impl.find(key_) == _impl.end() ? 0 : 1; }

    // Modifiers
    std::pair<iterator, bool> insert(const value_type& val_)
    { return _impl.insertUnique(val_); }

    iterator insert(const_iterator hint_, const value_type& val_)
    { return _impl.insertHintUnique(hint_, val_); }

    iterator erase(const_iterator pos_)
    { return _impl.erase(pos_).first; }

    size_type erase(const key_type& key_)
    { return _impl.erase(key_) ? 1 : 0; }

    iterator erase(const_iterator first_, const_iterator last_)
    { return _impl.erase(first_, last_); }

    void clear()
    { _impl.clear(); }

    void swap(Hashset& set_)
    { _impl.swap(set_._impl); }

  };

  template <
            typename Key,
            typename Hash,
            typename Pred,
            typename Alloc
           >
  void swap(Hashset<Key, Hash, Pred, Alloc>& set1_,
            Hashset<Key, Hash, Pred, Alloc>& set2_)
  { set1_.swap(set2_); }
}
}

#endif
