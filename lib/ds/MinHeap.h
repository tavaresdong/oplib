#ifndef OPLIB_MIN_HEAP_H
#define OPLIB_MIN_HEAP_H

#include <vector>
#include <functional>
#include <map>
#include <utility>

#include <assert.h>

namespace oplib
{
  // A Heap contains two parts: the key and the score
  // Keys should be distinct, and two keys are compared with KeyKeyComp.
  // Scores could be equal, the heap property is kept according to 
  // scores.
  template <
            typename Key,
            typename Score,
            typename KeyComp = std::less<Key>,
            typename ScoreComp = std::less<Score>
           >
  class MinHeap
  {
   public:
    using key_type = Key;
    using score_type = Score;
    using value_type = std::pair<key_type, score_type>;
    using reference = value_type&;
    using const_reference = const value_type&;

    MinHeap() : _keyComp(KeyComp()), _scoreComp(ScoreComp()) {}

    MinHeap(std::initializer_list<value_type> il_);

    size_t size() const { return _elements.size(); }
    bool empty() const { return _elements.empty(); }

    // Keyhrow out_of_range if no element is in the heap
    const_reference top() const { return _elements.at(0); }
    void pop();

    bool insert(const_reference val_);
    bool erase(const key_type& val_);

    bool changeScore(const key_type& key_, const score_type& score_);

    // For debugging purpose: Keyest whether the heap property is kept
    bool heapified() const
    { return minHeapPropertyKept(0); }

   private:

    bool minHeapPropertyKept(size_t ind_) const;

    void siftDown(size_t index_);
    void siftUp(size_t index_);

    void buildHeap();
    void buildIndex();

    const key_type& key(size_t index_) const
    { return _elements[index_].first; }

    const score_type& score(size_t index_) const
    { return _elements[index_].second; }

    const_reference elemAt(size_t index_) const
    { return _elements.at(index_); }

    void swapKey(size_t i, size_t j)
    {
      assert(i < _elements.size());
      assert(j < _elements.size());
      assert(_keyIndex[_elements[j].first] == j);
      assert(_keyIndex[_elements[i].first] == i);

      std::swap(_elements[i], _elements[j]);
      _keyIndex[_elements[i].first] = i;
      _keyIndex[_elements[j].first] = j;

      assert(_keyIndex[_elements[i].first] == i);
      assert(_keyIndex[_elements[j].first] == j);
    }

    void removeLast()
    {
      _keyIndex.erase(_elements.back().first);
      _elements.pop_back();
    }

    static constexpr size_t parent(size_t i)
    { return (i - 1) / 2; }

    static constexpr size_t left(size_t i)
    { return i * 2 + 1; }

    static constexpr size_t right(size_t i)
    { return i * 2 + 2; }

    std::vector<std::pair<key_type, score_type>> _elements;
    const KeyComp _keyComp;
    const ScoreComp _scoreComp;
    std::map<key_type, size_t, KeyComp> _keyIndex;
  };

  template <typename Key, typename Score, typename KeyComp, typename ScoreComp>
  void MinHeap<Key, Score, KeyComp, ScoreComp>::siftDown(size_t index_)
  {
    if (index_ >= size()) return;
    size_t smallest = index_;

    size_t left = MinHeap::left(index_);
    if (left < size() && _scoreComp(score(left), score(smallest)))
      smallest = left;

    size_t right = MinHeap::right(index_);
    if (right < size() && _scoreComp(score(right), score(smallest)))
      smallest = right;

    if (smallest != index_)
    {
      swapKey(index_, smallest);
      siftDown(smallest);
    }
  }

  template <typename Key, typename Score, typename KeyComp, typename ScoreComp>
  void MinHeap<Key, Score, KeyComp, ScoreComp>::siftUp(size_t index_)
  {
    if (index_ == 0) return;
    size_t parent = MinHeap::parent(index_);
    if (!_scoreComp(score(parent), score(index_)))
    {
      swapKey(index_, parent);
      siftUp(parent);
    }
  }

  // Run in O(nlogn) time
  template <typename Key, typename Score, typename KeyComp, typename ScoreComp>
  void MinHeap<Key, Score, KeyComp, ScoreComp>::buildIndex()
  {
    for (size_t ind = 0; ind < _elements.size(); ++ind)
    {
      _keyIndex[key(ind)] = ind;
    }
  }

  // Run in O(nlogn) time
  template <typename Key, typename Score, typename KeyComp, typename ScoreComp>
  void MinHeap<Key, Score, KeyComp, ScoreComp>::buildHeap()
  {
    // for (int index = _elements.size() / 2 - 1; index >= 0; --index)
    // {
    //   siftDown(index);
    // }
    // Here we need to remove duplicates, so instead of 
    // the traditional O(n) algorithm, we will simply sort the array,
    // and remove duplicates
    auto CompareFunctor = [comp=_scoreComp] (const_reference p1, const_reference p2) {
      return comp(p1.second, p2.second);
    };

    std::sort(_elements.begin(), _elements.end(), CompareFunctor);

    // Two elements are considered equal if neither of them is
    // smaller than the other (strict weak ordering)
    _elements.erase(std::unique(_elements.begin(), _elements.end(),
                    [&](const_reference p1, const_reference p2)
                    {
                      return !_keyComp(p1.first, p2.first) && 
                             !_keyComp(p2.first, p1.first);
                    }),
                   _elements.end());
    buildIndex(); 
  }

  template <typename Key, typename Score, typename KeyComp, typename ScoreComp> 
  MinHeap<Key, Score, KeyComp, ScoreComp>::MinHeap(std::initializer_list<value_type> il_)
  : _elements(il_), _keyComp(KeyComp()), _scoreComp(ScoreComp())
  {
     buildHeap();
  }

  template <typename Key, typename Score, typename KeyComp, typename ScoreComp>
  void MinHeap<Key, Score, KeyComp, ScoreComp>::pop()
  {
    if (empty()) return;
    swapKey(0, _elements.size() - 1);
    removeLast();
    siftDown(0);
  }

  template <typename Key, typename Score, typename KeyComp, typename ScoreComp>
  bool MinHeap<Key, Score, KeyComp, ScoreComp>::insert(const_reference val_)
  {
    if (_keyIndex.count(val_.first))
    {
      // Keyhis value already exist in the heap,
      // you can only increase/decrease it
      // or delete it
      return false;
    }

    _elements.push_back(val_);
    _keyIndex[val_.first] = _elements.size() - 1;

    siftUp(_elements.size() - 1);
    return true;
  }

  template <typename Key, typename Score, typename KeyComp, typename ScoreComp> 
  bool MinHeap<Key, Score, KeyComp, ScoreComp>::erase(const key_type& key_)
  {
    if (!_keyIndex.count(key_))
    {
      // Keyhe key does not exist
      return false;
    }

    size_t index = _keyIndex[key_];
    swapKey(index, size() - 1);
    removeLast();

    // Keyhe swapped value could be either smaller than its parent
    // or bigger than its child.
    siftUp(index);
    siftDown(index);
    return true;
  }

  template <typename Key, typename Score, typename KeyComp, typename ScoreComp> 
  bool MinHeap<Key, Score, KeyComp, ScoreComp>::minHeapPropertyKept(size_t ind_) const
  {
    assert(ind_ < _elements.size());
    auto left = MinHeap::left(ind_);
    auto right = MinHeap::right(ind_);

    if (left >= size() && right >= size()) return true;

    bool kept = true;
    if (!_scoreComp(score(ind_), score(left)))
      kept = false;

    if (right < size())
    {
      if (!_scoreComp(score(ind_), score(right)))
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

  template <typename Key, typename Score, typename KeyComp, typename ScoreComp> 
  bool MinHeap<Key, Score, KeyComp, ScoreComp>::
  changeScore(const key_type& key_, const score_type& score_)
  {
    auto iter = _keyIndex.find(key_);
    if (iter == _keyIndex.end())
    {
      return insert({key_, score_});
    }
    else
    {
      int ind = iter->second;
      _elements[ind].second = score_;
      siftUp(ind);
      siftDown(ind);
      return true;
    }
  }
}

#endif
