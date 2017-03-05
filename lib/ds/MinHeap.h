#ifndef OPLIB_MIN_HEAP_H
#define OPLIB_MIN_HEAP_H

#include <vector>
#include <functional>
#include <unordered_map>
#include <utility>

#include <assert.h>

namespace oplib
{
  // TODO: question: do we only need one of Equal and Comp?
  template <typename T, typename Equal = std::equal_to<T>, typename Comp = std::less<T>>
  class MinHeap
  {
   public:
    using value_type = T;

    MinHeap() {}
    MinHeap(std::initializer_list<value_type> il_);

    size_t size() const { return _elements.size(); }
    bool empty() const { return _elements.empty(); }

    // Throw out_of_range if no element is in the heap
    const T& top() const { return _elements.at(0); }
    void pop();

    bool insert(const value_type& val_);
    bool erase(const value_type& val_);

    // For debug use: the heap property is kept
    bool heapified() const
    { return minHeapPropertyKept(0); }

   private:

    bool minHeapPropertyKept(size_t ind_) const;

    void siftDown(size_t index_);
    void siftUp(size_t index_);

    void buildHeap();
    void buildIndex();

    value_type& elemAt(size_t index_)
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
  };

  template <typename T, typename Equal, typename Comp>
  void MinHeap<T, Equal, Comp>::siftDown(size_t index_)
  {
    if (index_ >= size()) return;
    size_t smallest = index_;

    size_t left = MinHeap::left(index_);
    if (left < size() && Comp()(_elements[left], _elements[smallest]))
      smallest = left;

    size_t right = MinHeap::right(index_);
    if (right < size() && Comp()(_elements[right], _elements[smallest]))
      smallest = right;

    if (smallest != index_)
    {
      swapKey(index_, smallest);
      siftDown(smallest);
    }
  }

  template <typename T, typename Equal, typename Comp>
  void MinHeap<T, Equal, Comp>::siftUp(size_t index_)
  {
    if (index_ == 0) return;
    size_t parent = MinHeap::parent(index_);
    if (!Comp()(_elements[parent], _elements[index_]))
    {
      swapKey(index_, parent);
      siftUp(parent);
    }
  }

  // Run in O(nlogn) time
  template <typename T, typename Equal, typename Comp> 
  void MinHeap<T, Equal, Comp>::buildIndex()
  {
    for (size_t ind = 0; ind < _elements.size(); ++ind)
    {
      _keyIndex[_elements[ind]] = ind;
    }
  }

  // Run in O(nlogn) time
  template <typename T, typename Equal, typename Comp> 
  void MinHeap<T, Equal, Comp>::buildHeap()
  {
    // for (int index = _elements.size() / 2 - 1; index >= 0; --index)
    // {
    //   siftDown(index);
    // }
    // Here we need to remove duplicates, so instead of 
    // the traditional O(n) algorithm, we will simply sort the array,
    // and remove duplicates
    std::sort(_elements.begin(), _elements.end(), Comp());
    _elements.erase(std::unique(_elements.begin(), _elements.end(), Equal()),
                   _elements.end());
    buildIndex(); 
  }

  template <typename T, typename Equal, typename Comp> 
  MinHeap<T, Equal, Comp>::MinHeap(std::initializer_list<value_type> il_)
  : _elements(il_)
  {
     buildHeap();
  }

  template <typename T, typename Equal, typename Comp> 
  void MinHeap<T, Equal, Comp>::pop()
  {
    if (empty()) return;
    swapKey(0, _elements.size() - 1);
    removeLast();
    siftDown(0);
  }

  template <typename T, typename Equal, typename Comp> 
  bool MinHeap<T, Equal, Comp>::insert(const value_type& val_)
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

  template <typename T, typename Equal, typename Comp> 
  bool MinHeap<T, Equal, Comp>::erase(const value_type& val_)
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

  template <typename T, typename Equal, typename Comp> 
  bool MinHeap<T, Equal, Comp>::minHeapPropertyKept(size_t ind_) const
  {
    assert(ind_ < _elements.size());
    auto left = MinHeap::left(ind_);
    auto right = MinHeap::right(ind_);

    if (left >= size() && right >= size()) return true;

    bool kept = true;
    if (!Comp()(_elements[ind_], _elements[left]))
      kept = false;

    if (right < size())
    {
      if (!Comp()(_elements[ind_], _elements[right]))
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
}

#endif
