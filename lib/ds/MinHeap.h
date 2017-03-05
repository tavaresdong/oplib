#ifndef OPLIB_MIN_HEAP_H
#define OPLIB_MIN_HEAP_H

#include <vector>
#include <functional>
#include <unordered_map>
#include <utility>

#include <assert.h>

namespace oplib
{
  template <typename T, typename Comp = std::less<T>>
  class MinHeap
  {
   public:
    using value_type = T;

    MinHeap() : _comp(Comp()) {}
    MinHeap(std::initializer_list<value_type> il_);

    size_t size() const { return _elements.size(); }
    bool empty() const { return _elements.empty(); }

    // Throw out_of_range if no element is in the heap
    const T& top() const { return _elements.at(0); }
    void pop();

    bool insert(const value_type& val_);
    bool erase(const value_type& val_);

    bool changeKey(const value_type& oldKey_, const value_type& newKey_);

    // For debugging purpose: Test whether the heap property is kept
    bool heapified() const
    { return minHeapPropertyKept(0); }

   private:

    bool minHeapPropertyKept(size_t ind_) const;

    void siftDown(size_t index_);
    void siftUp(size_t index_);

    void buildHeap();
    void buildIndex();

    value_type& elemAt(size_t index_) const
    { return _elements.at(index_); }

    void swapKey(size_t i, size_t j)
    {
      assert(i < _elements.size());
      assert(j < _elements.size());
      assert(_keyIndex[_elements[j]] == j);
      assert(_keyIndex[_elements[i]] == i);

      std::swap(_elements[i], _elements[j]);
      _keyIndex[_elements[i]] = i;
      _keyIndex[_elements[j]] = j;

      assert(_keyIndex[_elements[j]] == j);
      assert(_keyIndex[_elements[i]] == i);
    }

    void removeLast()
    {
      _keyIndex.erase(_elements.back());
      _elements.pop_back();
    }

    static size_t parent(size_t i)
    { return (i - 1) / 2; }

    static size_t left(size_t i)
    { return i * 2 + 1; }

    static size_t right(size_t i)
    { return i * 2 + 2; }

    std::vector<T> _elements;
    std::unordered_map<T, size_t> _keyIndex;
    const Comp _comp;
  };

  template <typename T, typename Comp>
  void MinHeap<T, Comp>::siftDown(size_t index_)
  {
    if (index_ >= size()) return;
    size_t smallest = index_;

    size_t left = MinHeap::left(index_);
    if (left < size() && _comp(_elements[left], _elements[smallest]))
      smallest = left;

    size_t right = MinHeap::right(index_);
    if (right < size() && _comp(_elements[right], _elements[smallest]))
      smallest = right;

    if (smallest != index_)
    {
      swapKey(index_, smallest);
      siftDown(smallest);
    }
  }

  template <typename T, typename Comp>
  void MinHeap<T, Comp>::siftUp(size_t index_)
  {
    if (index_ == 0) return;
    size_t parent = MinHeap::parent(index_);
    if (!_comp(_elements[parent], _elements[index_]))
    {
      swapKey(index_, parent);
      siftUp(parent);
    }
  }

  // Run in O(nlogn) time
  template <typename T, typename Comp> 
  void MinHeap<T, Comp>::buildIndex()
  {
    for (size_t ind = 0; ind < _elements.size(); ++ind)
    {
      _keyIndex[_elements[ind]] = ind;
    }
  }

  // Run in O(nlogn) time
  template <typename T, typename Comp> 
  void MinHeap<T, Comp>::buildHeap()
  {
    // for (int index = _elements.size() / 2 - 1; index >= 0; --index)
    // {
    //   siftDown(index);
    // }
    // Here we need to remove duplicates, so instead of 
    // the traditional O(n) algorithm, we will simply sort the array,
    // and remove duplicates
    std::sort(_elements.begin(), _elements.end(), _comp);

    // Two elements are considered equal if neither of them is
    // smaller than the other
    _elements.erase(std::unique(_elements.begin(), _elements.end(),
                    [&](const value_type& v1, const value_type& v2)
                    {
                      return !_comp(v1, v2) && !_comp(v2, v1);
                    }),
                   _elements.end());
    buildIndex(); 
  }

  template <typename T, typename Comp> 
  MinHeap<T, Comp>::MinHeap(std::initializer_list<value_type> il_)
  : _elements(il_), _comp(Comp())
  {
     buildHeap();
  }

  template <typename T, typename Comp> 
  void MinHeap<T, Comp>::pop()
  {
    if (empty()) return;
    swapKey(0, _elements.size() - 1);
    removeLast();
    siftDown(0);
  }

  template <typename T, typename Comp> 
  bool MinHeap<T, Comp>::insert(const value_type& val_)
  {
    if (_keyIndex.count(val_))
    {
      // This value already exist in the heap,
      // you can only increase/decrease it
      // or delete it
      return false;
    }

    _elements.push_back(val_);
    _keyIndex[val_] = _elements.size() - 1;

    siftUp(_elements.size() - 1);
    return true;
  }

  template <typename T, typename Comp> 
  bool MinHeap<T, Comp>::erase(const value_type& val_)
  {
    if (!_keyIndex.count(val_))
    {
      // The key does not exist
      return false;
    }

    size_t index = _keyIndex[val_];
    swapKey(index, size() - 1);
    removeLast();

    // The swapped value could be either smaller than its parent
    // or bigger than its child.
    siftUp(index);
    siftDown(index);
    return true;
  }

  template <typename T, typename Comp> 
  bool MinHeap<T, Comp>::minHeapPropertyKept(size_t ind_) const
  {
    assert(ind_ < _elements.size());
    auto left = MinHeap::left(ind_);
    auto right = MinHeap::right(ind_);

    if (left >= size() && right >= size()) return true;

    bool kept = true;
    if (!_comp(_elements[ind_], _elements[left]))
      kept = false;

    if (right < size())
    {
      if (!_comp(_elements[ind_], _elements[right]))
        kept = false;
    }

    if (!kept) return false;
    else
    {
      if (right < size())
        return minHeapPropertyKept(left) && 
               minHeapPropertyKept(right);
      else
        return minHeapPropertyKept(left);
    }
  }

  template <typename T, typename Comp> 
  bool MinHeap<T, Comp>::changeKey(const value_type& oldKey_, const value_type& newKey_)
  {
    if (_keyIndex.count(newKey_)) return false;
    auto iter = _keyIndex.find(oldKey_);
    if (iter == _keyIndex.end())
    {
      return false;
    }
    else
    {
      int ind = iter->second;
      _keyIndex.erase(iter);
      auto ret = _keyIndex.insert({newKey_, ind});
      assert(ret.second);
      assert(_elements[ind] == oldKey_);
      _elements[ind] = newKey_;
      siftUp(ind);
      siftDown(ind);
      return true;
    }
  }
}

#endif
