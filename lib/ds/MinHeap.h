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
    using key_type = T;

    MinHeap() {}
    ~MinHeap() {}
    
    MinHeap(std::initializer_list<value_type> il_);

    size_t size() const { return _elements.size(); }
    bool empty() const { return _elements.empty(); }

    // Throw out_of_range if no element is in the heap
    const T& top() const { return _elements.at(0); }
    void pop();

   private:

    void minHeapify(size_t index_);
    void buildHeap();

    value_type& elemAt(size_t index_)
    { return _elements.at(index_); }

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
  void MinHeap<T, Equal, Comp>::minHeapify(size_t index_)
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
      std::swap(_elements[index_], _elements[smallest]);
      minHeapify(smallest);
    }
  }

  // Run in O(n) time
  template <typename T, typename Equal, typename Comp> 
  void MinHeap<T, Equal, Comp>::buildHeap()
  {
    for (int index = _elements.size() / 2 - 1; index >= 0; --index)
    {
      minHeapify(index);
    }
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
    std::swap(elemAt(0), elemAt(size() - 1));
    _elements.pop_back();
    minHeapify(0);
  }
}

#endif
