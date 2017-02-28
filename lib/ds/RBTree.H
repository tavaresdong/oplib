#ifndef OPLIB_RBTREE_H
#define OPLIB_RBTREE_H

#include <iterator>

namespace oplib
{
  enum class RBTreeNodeColor
  {
    Red,
    Black
  };

  struct RBNodeBase
  {
    using Ptr = RBNodeBase*;
    RBNodeColor _color;
    Ptr _left;
    Ptr _right;
    Ptr _parent;

    static Ptr minnode(Ptr ptr_)
    {
      while (ptr_ != nullptr)
        ptr_ = ptr_->_left;

      return ptr_;
    }

    static Ptr maxnode(Ptr ptr_)
    {
      while (ptr_ != nullptr)
        ptr_ = ptr_->right;

      return ptr_;
    }
  };

  template <typename T>
  stuct RBNode : public RBNodeBase
  {
    using Ptr = RBNode<T>*;
    T _value;
  }

  struct RBIteratorBase
  {
    using RBNodeBasePtr = RBNodeBase::Ptr;
    using iterator_tag = std::bidirectional_iterator_tag;
    using difference_type = ptrdiff_t;

    RBNodeBasePtr _pnode;
  };

  template <typename Value, typename Ref, typename Ptr>
  struct RBIterator : public RBIteratorBase
  {
    using value_type = Value;
    using reference = Ref;
    using pointer = Ptr;
    using iterator = RBIterator<Value, Value&, Value*>;
    using const_iterator = RBIterator<Value, const Value&, const Value*>;
    using self = RBIterator<Value, Ref, Ptr>;
    using RBNodePtr = RBNode<Value>::Ptr;

    RBIterator() {}
    RBIterator(RBNodePtr ptr_) { _pnode = ptr_; }
    RBIterator(const iterator& it_) { _pnode = it_._pnode; }

    reference operator * () { return (static_cast<RBNodePtr>(_pnode))->_value; }
    pointer   operator -> () { return &(operator *()); }

    void increment();
    void decrement();

    self& operator ++ () { increment(); return *this }
    self  operator ++ (int) 
    {
      self tmp = *this;
      increment();
      return tmp;
    }

    self& operator -- () { decrement(); return *this; }
    self  operator -- (int)
    {
      self tmp = *this;
      decrement();
      return tmp;
    }
  };

  template <typename Key, typename Value, typename Comp, typename Alloc = opallocator>
  class RBTree
  {
   protected:
    using NodeType = RBNode<Value>
    using NodeAlloc = opallocator<Value>;

  };

  #include "RBTreeT.C"
}

#endif
