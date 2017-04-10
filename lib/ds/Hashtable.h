#pragma once
#ifndef OPLIB_DS_HASHTABLE_H
#define OPLIB_DS_HASHTABLE_H
#include <memory>
#include <vector>
#include <stdexcept>
#include <algorithm>
#include <utility>

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
            class Alloc = std::allocator<Value>>
  class Hashtable
  {
   public:
    using hasher = HashFunc;
    using key_equal = EqualKey;
    using size_type = size_t;
    using value_type = Value;
    using allocator_type = Alloc;

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

    bool insertUniqueNoresize(const value_type& val_)
    {
      const size_type n = bucketNum(val_, _buckets.size());
      NodePtr first = _buckets[n];
      for (auto cur = first; cur != nullptr; cur = cur->_next)
      {
        // The key already exists
        if (_equals(_keyExtractor(cur->_val), _keyExtractor(val_)))
          return false;
      }

      NodePtr tmp = createNode(val_);
      tmp->_next = _buckets[n];
      _buckets[n] = tmp;
      ++_numElements;
      return true;
    }

   public:

    size_type bucket_count() const
    { return _buckets.size(); }

    size_type size() const
    { return _numElements; }

    // Maximum number of _buckets available in this system
    size_type max_bucket_count()
    { return primesList[primeCount - 1]; }

    Hashtable(size_type n,
              const HashFunc& hf = HashFunc(),
              const EqualKey& eq = EqualKey())
    : _hash(hf), _equals(eq), _keyExtractor(ExtractKey()), _numElements(0)
    { init(n); }

    ~Hashtable()
    { clear(); }

    bool insertUnique(const value_type& val_)
    {
      // test if we need to expand the table
      resize(_numElements + 1);
      return insertUniqueNoresize(val_);
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

  };
}
}

#endif
