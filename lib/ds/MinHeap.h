#ifndef OPLIB_MIN_HEAP_H
#define OPLIB_MIN_HEAP_H

#include <vector>
#include <functional>
#include <unordered_map>
#include <utility>

#include <assert.h>

namespace oplib
{
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

   private:

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
    if (left < size() && _elements[left] < _elements[smallest])
      smallest = left;

    size_t right = MinHeap::right(index_);
    if (right < size() && _elements[right] < _elements[smallest])
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
    if (_elements[parent] > _elements[index_])
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
    _keyIndex.erase(_elements.back());
    _elements.pop_back();
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
}

#endif
