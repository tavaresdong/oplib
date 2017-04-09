#ifndef OPLIB_DS_RBTREE_H
#define OPLIB_DS_RBTREE_H

#include <iterator>
#include <memory>
#include <utility>
#include <initializer_list>

namespace oplib
{
namespace ds
{
  enum class RBTreeNodeColor
  {
    Red,
    Black
  };

  struct RBNodeBase
  {
    using Ptr = RBNodeBase*;
    RBTreeNodeColor _color;
    Ptr _left;
    Ptr _right;
    Ptr _parent;

    static Ptr minnode(Ptr ptr_)
    {
      while (ptr_->_left != nullptr)
        ptr_ = ptr_->_left;

      return ptr_;
    }

    static Ptr maxnode(Ptr ptr_)
    {
      while (ptr_->_right != nullptr)
        ptr_ = ptr_->_right;

      return ptr_;
    }
  };

  template <typename T>
  struct RBNode : public RBNodeBase
  {
    using Ptr = RBNode<T>*;
    T _value;
    RBNode(T val_) : _value(val_) {}
  };

  struct RBIteratorBase
  {
    using RBNodeBasePtr = RBNodeBase::Ptr;
    using iterator_tag = std::bidirectional_iterator_tag;

    RBNodeBasePtr _pnode;
  };

  template <typename Value, typename Ref, typename Ptr>
  struct RBIterator : public RBIteratorBase
  {
    using value_type = Value;
    using reference = Ref;
    using pointer = Ptr;
    using self = RBIterator<Value, Ref, Ptr>;
    using difference_type = ptrdiff_t;
    using iterator_category = std::bidirectional_iterator_tag;
    using RBNodePtr = typename RBNode<Value>::Ptr;

    RBIterator() {}
    RBIterator(RBNodePtr ptr_) { _pnode = ptr_; }
    RBIterator(const RBIterator& it_) { _pnode = it_._pnode; }

    reference operator * () 
    { return (static_cast<RBNodePtr>(_pnode))->_value; }

    pointer   operator -> () 
    { return &(operator *()); }

    void increment();
    void decrement();

    self& operator ++ () 
    { increment(); return *this; }

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

    bool operator == (const RBIterator& rhs_) const
    { return _pnode == rhs_._pnode; }
   
    bool operator != (const RBIterator& rhs_) const
    { return !operator ==(rhs_); }

  };


  template <typename Key, typename Value, class KeyOfValue,
            typename Comp, class Alloc = std::allocator<Key> >
  class RBTree
  {
   public:
    using key_type = Key;
    using value_type = Value;
    using size_type = size_t;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const value_type*;

    using iterator = RBIterator<value_type, reference, pointer>;
    using const_iterator = const RBIterator<value_type, reference, pointer>; 
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    using allocator_type = Alloc;
    using key_compare = Comp;

   protected:
    using NodeType = RBNode<Value>;
    using NodePtr = NodeType*;

    // Using rebind to get member template type, the real allocator type
    using NodeAlloc = typename Alloc::template rebind<NodeType>::other;

    // The implementation of the RBTree
    NodeAlloc _allocator;
    size_type _nodeCount;
    NodePtr _header;
    Comp _comparator;
    KeyOfValue _keyExtractor;

    // Allocate/Deallocate nodes
    // create/destroy nodes
    NodePtr getNode()
    { return _allocator.allocate(1); }

    void putNode(NodePtr ptr_)
    { if (ptr_ != nullptr) _allocator.deallocate(ptr_, 1); }

   // NodePtr createNode(const value_type& val_)
   // {
   //   auto nd = getNode();
   //   try
   //   {
   //     _allocator.construct(nd, val_);
   //   }
   //   catch (...)
   //   {
   //     putNode(nd);
   //   }
   //   return nd;
   // }
   //

    template <typename... Args>
    NodePtr createNode(Args&&... args_)
    {
      auto nd = getNode();
      try
      {
        _allocator.construct(nd, std::forward<Args>(args_)...);
      }
      catch (...)
      {
        putNode(nd);
      }
      return nd;
    }

    void destroyNode(NodePtr ptr_)
    {
      _allocator.destroy(ptr_);
      putNode(ptr_);
    }

    // Convenient functions
    NodePtr root() const
    { return static_cast<NodePtr>(_header->_parent); }

    NodePtr leftmost() const
    { return static_cast<NodePtr>(_header->_left); }

    NodePtr rightmost() const
    { return static_cast<NodePtr>(_header->_right); }

    // Convenient functions to retrive members of a node
    static NodePtr left(RBNodeBase* node_)
    { return static_cast<NodePtr>(node_->_left); }

    static NodePtr right(RBNodeBase* node_)
    { return static_cast<NodePtr>(node_->_right); }

    static NodePtr parent(RBNodeBase* node_)
    { return static_cast<NodePtr>(node_->_parent); }

    static reference value(NodePtr node_)
    { return node_->_value; }

    static Key key(NodePtr node_)
    { return KeyOfValue()(value(node_)); }

    static RBTreeNodeColor color(RBNodeBase* node_)
    { return node_->_color; }

    static NodePtr min(NodePtr node_)
    { return static_cast<NodePtr>(minnode(node_)); }

    static NodePtr max(NodePtr node_)
    { return static_cast<NodePtr>(maxnode(node_)); }

   private:

    void init()
    {
      // The guard header, storing no data
      _header = getNode();
      _header->_color = RBTreeNodeColor::Red;

      // guard's parent points to the actual root of the tree
      _header->_parent = nullptr;
      
      _header->_left = _header;
      _header->_right = _header;
    }

    NodePtr copy(NodePtr cur_, NodePtr parent_);

    void clear(NodePtr ptr_);

    iterator insert(NodePtr child_, NodePtr parent_, const value_type& val_);

    iterator insertNode(NodePtr child_, NodePtr parent_, NodePtr newNode_);
    void eraseOneChild(RBNodeBase* node_);
    void rebalance(RBNodeBase* toInsert_, RBNodeBase*& root_);
    void leftRotate(RBNodeBase* node_, RBNodeBase*& root_);
    void rightRotate(RBNodeBase* node_, RBNodeBase*& root_);
    void switchNode(RBNodeBase* parent_, RBNodeBase* child_);
    void swapNode(RBNodeBase* n1_, RBNodeBase* n2_);
    void eraseCase1(RBNodeBase* node_, RBNodeBase* parent_);
    void eraseCase2(RBNodeBase* node_, RBNodeBase* parent_);
    void eraseCase3(RBNodeBase* node_, RBNodeBase* parent_);
    void eraseCase4(RBNodeBase* node_, RBNodeBase* parent_);
    void eraseCase5(RBNodeBase* node_, RBNodeBase* parent_);
    void eraseCase6(RBNodeBase* node_, RBNodeBase* parent_);

   public:

    explicit RBTree(const Comp& comp_ = Comp(),
                    const allocator_type& alloc_ = allocator_type());
    RBTree(std::initializer_list<value_type> il,
           const Comp& comp_ = Comp(),
           const allocator_type& alloc_ = allocator_type());
    // Rule of 5
    RBTree(const RBTree& rhs_);
    RBTree& operator = (const RBTree& rhs_);
    RBTree(RBTree&& rhs_);
    RBTree& operator = (RBTree&& rhs_);
    ~RBTree()
    {
      clear();
      putNode(_header);
    }

    iterator begin() { return leftmost(); }
    iterator end() { return _header; }
    const_iterator cbegin() const { return leftmost(); }
    const_iterator cend() const { return _header; }
    reverse_iterator rbegin()
    { return reverse_iterator(end()); }
    reverse_iterator rend()
    { return reverse_iterator(begin()); }
    const_reverse_iterator crbegin() const 
    { return const_reverse_iterator(cend()); }
    const_reverse_iterator crend() const 
    { return const_reverse_iterator(cbegin()); }

    bool empty() const { return _nodeCount == 0; }
    size_type size() const { return _nodeCount; }
    size_type max_size() const { return size_type(-1); }
    key_compare key_comp() const { return _comparator; }

    const_iterator find(const key_type& key_) const;
    iterator find(const key_type& key_);

    const_iterator lower_bound(const key_type& key_) const;
    iterator lower_bound(const key_type& key_);

    const_iterator upper_bound(const key_type& key_) const;
    iterator upper_bound(const key_type& key_);

    std::pair<const_iterator, const_iterator> equal_range(const key_type& key_) const;
    std::pair<iterator, iterator> equal_range(const key_type& key_);

    // Used for debugging: whether the Red-Black tree
    // Property is kept: header is (red/no two consecutive red nodes in a path/
    // all paths have same black height)
    bool rbPropertyKept() const;

    // Modify methods
    void clear()
    {
      if (_header != nullptr)
      {
        clear(root()); 
        _header->_parent = nullptr;
        _header->_left = _header;
        _header->_right = _header;
        _nodeCount = 0;
      }
    }

    template <typename... Args>
    std::pair<iterator, bool> emplaceUnique(Args&&... args_);

    iterator insertEqual(const value_type& val_);

    std::pair<iterator, bool> insertUnique(const value_type& val_)
    { return emplaceUnique(val_); }
    iterator insertUnique(const_iterator iter_, const value_type& val_);

    // Erase an element by position
    std::pair<iterator, bool> erase(iterator iter);

    // Erase an element by value, if there are multiple values, erase the first one
    // encountered
    std::pair<iterator, bool> erase(const key_type& key_)
    {
      auto pos = find(key_);
      if (pos != end()) return erase(pos);
      return std::make_pair(end(), false);
    }

    iterator erase(iterator first_, iterator last_)
    {
      if (first_ == begin() && last_ == end())
      {
        clear();
        return begin();
      }
      else
      {
        while (first_ != last_)
        {
          first_ = erase(first_).first;
        }
      }
      return last_;
    }

    void swap(RBTree& rhs_)
    {
      std::swap(_allocator, rhs_._allocator);
      std::swap(_nodeCount, rhs_._nodeCount);
      std::swap(_header, rhs_._header);
      std::swap(_comparator, rhs_._comparator);
      std::swap(_keyExtractor, rhs_._keyExtractor);
    }
  };

  #include "RBTreeT.C"
}
}
#endif
