#pragma once
#ifndef OPLIB_DS_HASHTABLE_H
#define OPLIB_DS_HASHTABLE_H
#include <memory>
#include <vector>
#include <stdexcept>
#include <algorithm>
#include <utility>

// TODO: add c++11 new features: emplace move copy/assign
namespace oplib 
{
namespace ds
{
  // We predefined 28 prime numbers to be used on our
  // Hasht_able, and we provide a function to
  // find the smallest prime which is larger than given number
  static constexpr const int primeCount { 28 };
  static const unsigned long primesList[primeCount] = 
  {
    53, 97, 193, 389, 769,
    1543, 3079, 6151, 12289, 24593,
    49157, 98317, 196613, 393241, 786433,
    1572869, 3145739, 6291469, 12582917, 25165843,
    50331653, 100663319, 201326611, 402653189, 805306457,
    1610612741, 3221225473ul, 4294967291ul
  };

  inline unsigned long nextPrime(unsigned long n_)
  {
    auto beg = primesList;
    auto end = primesList + primeCount;
    auto pos = std::lower_bound(beg, end, n_);
    return (pos == end) ? *(end - 1) : *pos;
  }

  // We use linked hash node to resolve conflict
  // HashNode is the smallest element that stores the payload and link
  template <typename T>
  struct HashNode
  {
    HashNode(const T& v_) : _val(v_), _next(nullptr) { }
    T _val;
    HashNode* _next;
  };

  template <typename Value, typename Key, typename HashFunc,
            typename ExtractKey, typename EqualKey, 
            class Alloc>
  class Hashtable;

  template <typename Value, typename Key, typename HashFunc,
            typename ExtractKey, typename EqualKey, 
            class Alloc = std::allocator<Value>>
  struct HashIterator
  {
    using hashtable = Hashtable<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>;
    using self = HashIterator<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>;
    using reference = Value&;
    using difference_type = ptrdiff_t;
    using iterator_category = std::forward_iterator_tag;
    using NodePtr = HashNode<Value>*;
    using pointer = NodePtr;

    NodePtr _pnode { nullptr };
    hashtable* _ht { nullptr };

    HashIterator() {}
    HashIterator(NodePtr ptr_, const hashtable* ht_):
    _pnode(ptr_), _ht(const_cast<hashtable*>(ht_)) {}

    reference operator * () const
    { return _pnode->_val; }

    NodePtr operator -> () const
    { return _pnode; }

    void increment();

    self& operator ++ () 
    { increment(); return *this; }

    self operator ++ (int) 
    {
      self tmp = *this;
      increment();
      return tmp;
    }

    bool operator == (const HashIterator& rhs_) const
    { return _pnode == rhs_._pnode; }
   
    bool operator != (const HashIterator& rhs_) const
    { return !operator ==(rhs_); }

  };

  template <typename Value, typename Key, typename HashFunc,
            typename ExtractKey, typename EqualKey, 
            class Alloc = std::allocator<Value>>
  class Hashtable
  {
    friend class HashIterator<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>;

   public:
    using hasher = HashFunc;
    using key_equal = EqualKey;
    using value_type = Value;
    using allocator_type = Alloc;
    using size_type = size_t;

    using reference = value_type;
    using const_reference = const value_type;
    using pointer = value_type*;
    using const_pointer = const value_type*;

    using iterator = HashIterator<Value, Key, hasher, ExtractKey, EqualKey, Alloc>;
    using const_iterator = const HashIterator<Value, Key, hasher, ExtractKey, EqualKey, Alloc>;

   private:
    hasher _hash;

    // Test if two keys are equal
    key_equal _equals;

    // The method to extract key(hash key) from value
    ExtractKey _keyExtractor;

    // Allocator for the node
    using NodeType = HashNode<value_type>;
    using NodePtr = NodeType*;
    using NodeAlloc = typename Alloc::template rebind<NodeType>::other;
    NodeAlloc _alloc;

    // Num of elements already inserted to our hasht_able
    size_type _numElements;

    // _buckets storing lists of values with the same hashed position
    using BucketList = std::vector<NodePtr, NodeAlloc>;
    BucketList _buckets;


   private:
    // Allocate a new node
    // If allocation failed, return a nullptr
    NodePtr createNode(const value_type& val)
    {
      try {
        NodePtr p = _alloc.allocate(1);
        _alloc.construct(p, val);
        return p;
      }
      catch (std::bad_alloc& ex_)
      {
        ex_.what();
        return nullptr;
      }
    }

    // Delete the node allocated by alloc
    void putNode(NodePtr p_)
    {
      _alloc.destroy(p_);
      _alloc.deallocate(p_, 1);
    }

    unsigned long nextSize(unsigned long n_)
    { return nextPrime(n_); }

    void init(size_type n_)
    {
      const size_type nbucks = nextSize(n_);
      _buckets.reserve(nbucks);
      _buckets.insert(_buckets.end(), nbucks, nullptr);
      _numElements = 0;
    }

    // Decide the position a value is hashed to, just
    // call _hash(_keyExtractor(val)) % n
    size_type bucketNum(const value_type& val_, size_type n_) const
    { return _hash(_keyExtractor(val_)) % n_; }

    // Test if given potential_ elements, should we expand the
    void resize(size_type potential_)
    {
      const size_type oldnum = _buckets.size();
      if (potential_ > oldnum)
      {
        const size_type n = nextSize(potential_);
        BucketList tmp(n, nullptr);
        for (size_type bucket = 0; bucket < oldnum; bucket++)
        {
          NodePtr first = _buckets[bucket];
          while (first != nullptr)
          {
            // Note: we can't assign the whole list
            // because two node hashed in the same bucket previously
            // may be hashed to different bucket when resized!
            // Find the position this value should be 
            // in the new _buckets(with new M value)
            size_type newBucket = bucketNum(first->_val, n);

            // Remove the element from original table
            // And insert to the new table
            _buckets[bucket] = first->_next;
            first->_next = tmp[newBucket];
            tmp[newBucket] = first;
            first = _buckets[bucket];
          }
        }
        _buckets.swap(tmp);
      }
    }

    std::pair<iterator, bool> insertUniqueNoresize(const value_type& val_)
    {
      const size_type n = bucketNum(val_, _buckets.size());
      NodePtr first = _buckets[n];
      for (auto cur = first; cur != nullptr; cur = cur->_next)
      {
        // The key already exists
        if (_equals(_keyExtractor(cur->_val), _keyExtractor(val_)))
          return std::make_pair(iterator(cur, this), false);
      }

      NodePtr tmp = createNode(val_);
      tmp->_next = _buckets[n];
      _buckets[n] = tmp;
      ++_numElements;
      return std::make_pair(iterator(tmp, this), true);
    }

    iterator insertHintUniqueNoresize(const_iterator hint_, const value_type& val_)
    {
      if (*hint_ == val_) return iterator(hint_._pnode, this);
      return insertUniqueNoresize(val_).first;
    }

    iterator getFirstElem() const
    {
      if (!empty())
      {
        for (size_type i = 0; i < _buckets.size(); ++i)
        {
          if (_buckets[i] != nullptr)
          {
            return iterator(_buckets[i], this);
          }
        }
      }
      return iterator(nullptr, this);
    }

   public:

    // Iterators
    iterator begin()
    { return getFirstElem(); }

    const_iterator cbegin() const
    { return getFirstElem(); }

    iterator end()
    { return iterator(nullptr, this); }

    const_iterator cend() const
    { return iterator(nullptr, this); }

    bool empty() const
    { return _numElements == 0; }

    size_type bucket_count() const
    { return _buckets.size(); }

    size_type size() const
    { return _numElements; }

    size_type max_size() const { return size_type(-1); }

    // Maximum number of _buckets available in this system
    size_type max_bucket_count()
    { return primesList[primeCount - 1]; }

    Hashtable(size_type n,
              const HashFunc& hf_ = HashFunc(),
              const EqualKey& eq_ = EqualKey(),
              const Alloc& alloc_ = Alloc())
    : _hash(hf_), _equals(eq_), _keyExtractor(ExtractKey()), _alloc(alloc_), _numElements(0)
    { init(n); }

    ~Hashtable()
    { clear(); }

    std::pair<iterator, bool> insertUnique(const value_type& val_)
    {
      // test if we need to expand the table
      resize(_numElements + 1);
      return insertUniqueNoresize(val_);
    }

    iterator insertHintUnique(const_iterator hint_, const value_type& val_)
    {
      resize(_numElements + 1);
      return insertHintUniqueNoresize(hint_, val_);
    }

    size_type elemsInbucket(size_type index_) const
    {
      if (index_ < 0 || index_ >= _buckets.size())
        throw std::out_of_range("index invalid");
      size_type cnt = 0;
      for (auto first = _buckets[index_]; first != nullptr; first = first->_next)
        cnt += 1;
      return cnt;
    }

    iterator find(const value_type& val_)
    {
      return static_cast<iterator>(static_cast<const Hashtable&>(*this).find(val_));
    }

    const_iterator find(const value_type& val_) const
    {
      const size_type n = bucketNum(val_, _buckets.size());

      NodePtr first = _buckets[n];
      for (auto cur = first; cur != nullptr; cur = cur->_next)
      {
        // The key already exists
        if (_equals(_keyExtractor(cur->_val), _keyExtractor(val_)))
          return iterator(cur, this);
      }

      return cend();
    }

    iterator erase(const_iterator first_, const_iterator last_)
    {
      iterator cur = first_;
      for (; cur != last_; )
      {
        cur = erase(cur).first;
      }
      return cur;
    }

    std::pair<iterator, bool> erase(const_iterator iter_)
    {
      if (iter_ == end()) return std::make_pair(iterator(nullptr, this), false);

      const size_type n = bucketNum(iter_->_val, _buckets.size());
      NodePtr first = _buckets[n];

      auto prev = _buckets[n];
      for (auto cur = first; cur != nullptr; cur = cur->_next)
      {
        // The key already exists
        if (cur == iter_._pnode)
        {
          iterator next(cur, this);
          ++next;
          if (prev == _buckets[n])
            _buckets[n] = cur->_next;
          else
          {
            prev->_next = cur->_next;
          }
          putNode(cur);
          --_numElements;
          return std::make_pair(next, true);
        }
        prev = cur;
      }
      return std::make_pair(iterator(nullptr, this), false);
    }

    bool erase(const value_type& val_)
    {
      return erase(find(val_)).second;
    }

    void clear()
    {
      for (size_type i = 0; i != _buckets.size(); ++i)
      {
        auto first = _buckets[i];
        while (first != nullptr)
        {
          _buckets[i] = first->_next;
          putNode(first);
          first = _buckets[i];
        }
        _buckets[i] = nullptr;
      }
      _numElements = 0;
    }

    // Use copy-and-swap idiom to guarantee exception safety
    void copy_from(const Hashtable& ht_)
    {
      BucketList tmp;
      tmp.reserve(ht_._buckets.size());
      tmp.insert(tmp.end(), ht_.bucket.size(), nullptr);
      try {
        for (size_type i = 0; i != ht_.bucket.size(); ++i)
        {
          NodePtr first = ht_.bucket[i];
          if (first != nullptr)
          {
            NodePtr copy = createNode(first->_val);
            _buckets[i] = copy;
            for (NodePtr cur = first->_next; cur; cur = cur->_next)
            {
              copy->_next = createNode(cur->val);
              copy = copy->_next;
            }
          }
        }
        tmp.swap(_buckets);

        // Clear tmp for memory cleaness
        //tmp.clear();
        _numElements = ht_._numElements;
      }
      catch (const std::bad_alloc& e)
      {

      }
    }

    void swap(Hashtable& table_)
    {
      using std::swap;
      swap(_hash, table_._hash);
      swap(_equals, table_._equals);
      swap(_keyExtractor, table_._keyExtractor);
      swap(_alloc, table_._alloc);
      swap(_numElements, table_._numElements);
      swap(_buckets, table_._buckets);
    }

  };

  template <typename Value, typename Key, typename HashFunc,
            typename ExtractKey, typename EqualKey, 
            class Alloc>
  void HashIterator<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>::increment()
  {
    if (_pnode == nullptr) return;
    if (_pnode->_next == nullptr)
    {
      auto n = _ht->bucketNum(_pnode->_val, _ht->_buckets.size());
      _pnode = _pnode->_next;
      while (_pnode == nullptr && ++n < _ht->_buckets.size())
        _pnode = _ht->_buckets[n];
    }
  }
}
}

#endif
