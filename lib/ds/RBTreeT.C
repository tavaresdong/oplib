#include <iostream>
#include <cassert>

// There is a header guard: red, root's parent is header 
// and header's parent is root. header's left is the leftmost node,
// header's right is the rightmost node
// Note it is undefined behaviour if your --begin() or ++end()
template <typename Value, typename Ref, typename Ptr>
void RBIterator<Value, Ref, Ptr>::increment()
{
  if (_pnode->_right != nullptr)
  {
    _pnode = RBNodeBase::minnode(_pnode->_right);
  } 
  else
  {
    auto p = _pnode->_parent;
    while (_pnode == p->_right)
    {
      _pnode = p;
      p = p->_parent;
    }

    // We have a header guard, its parent points to the root element
    if (_pnode->_right != p)
      _pnode = p; 
  }
}

template <typename Value, typename Ref, typename Ptr>
void RBIterator<Value, Ref, Ptr>::decrement()
{
  if (_pnode->_color == RBTreeNodeColor::Red &&
      _pnode->_parent->_parent == _pnode)
  {
    // _pnode is header: header's parent is root, root's parent is header.
    // and root must be black, header is red, so _pnode is end()
    // --end() == the last element
    _pnode = _pnode->_right;
  }
  if (_pnode->_left != nullptr)
  {
    _pnode = RBNodeBase::maxnode(_pnode->_left);
  }
  else
  {
    auto p = _pnode->_parent;
    while (p->_left == _pnode)
    {
      _pnode = p;
      p = _pnode->_parent;
    }

    _pnode = p;
  }
}

template <typename Key, typename Value, class KeyOfValue,
          typename Comp, template <typename> class Alloc>
void RBTree<Key, Value, KeyOfValue, Comp, Alloc>::clear(NodePtr ptr_)
{
  if (ptr_ != nullptr)
  {
    clear(static_cast<NodePtr>(ptr_->_left));
    clear(static_cast<NodePtr>(ptr_->_right));
    destroyNode(ptr_);
  }
}

template <typename Key, typename Value, class KeyOfValue,
          typename Comp, template <typename> class Alloc>
RBTree<Key, Value, KeyOfValue, Comp, Alloc>::RBTree(const Comp& comp_)
: _allocator(NodeAlloc()),
  _nodeCount(0),
  _comparator(comp_),
  _keyExtractor(KeyOfValue())
{
  init();
}

template <typename Key, typename Value, class KeyOfValue,
          typename Comp, template <typename> class Alloc>
typename RBTree<Key, Value, KeyOfValue, Comp, Alloc>::iterator
RBTree<Key, Value, KeyOfValue, Comp, Alloc>::insertEqual(const value_type& val_)
{
  NodePtr par = _header;
  NodePtr cur = root();

  while (cur != nullptr)
  {
    par = cur;
    if (_comparator(_keyExtractor(val_), key(cur)))
    {
      cur = cur->_left;
    }
    else
    {
      cur = cur ->_right;
    }
  }

  return insert(static_cast<NodePtr>(cur),
                static_cast<NodePtr>(par),
                val_);
}

template <typename Key, typename Value, class KeyOfValue,
          typename Comp, template <typename> class Alloc>
typename RBTree<Key, Value, KeyOfValue, Comp, Alloc>::iterator
RBTree<Key, Value, KeyOfValue, Comp, Alloc>::insert(NodePtr child_, NodePtr parent_, const value_type& val_)
{
  auto newNode = createNode(val_);

  if (parent_ == _header)
  {
    // Case1: empty tree
    parent_->_left = newNode;
    parent_->_right = newNode;
    parent_->_parent = newNode;
  }
  else if (_comparator(_keyExtractor(val_), key(parent_)))
  {
    parent_->_left = newNode;
    if (parent_ == leftmost())
    {
      // Assign leftmost to the new node
      _header->_left = newNode;
    }
  }
  else
  {
    parent_->_right = newNode;
    if (parent_ == rightmost())
    {
      // Assign rightmost to the new node
      _header->_right = newNode;
    }
  }

  // Initialize the newly created node:
  newNode->_left = newNode->_right = nullptr;
  newNode->_parent = parent_;

  rebalance(newNode, _header->_parent);
  ++_nodeCount;
  return iterator(newNode);
}

template <typename Key, typename Value, class KeyOfValue,
          typename Comp, template <typename> class Alloc>
std::pair<typename RBTree<Key, Value, KeyOfValue, Comp, Alloc>::iterator, bool>
RBTree<Key, Value, KeyOfValue, Comp, Alloc>::insertUnique(const value_type& val_)
{
 // This implementation is OK, but costs two invocations of _comparator, 
 // which is not efficient!
 //
 // NodePtr _par = _header;
 // NodePtr _cur = root();

 // while (_cur != nullptr)
 // {
 //   _par = _cur;
 //   if (_comparator(_keyExtractor(val_), key(_cur)))
 //   {
 //     _cur = _cur->_left;
 //   }
 //   else if (_comparator(key(_cur), _keyExtractor(val_)))
 //   {
 //     _cur = _cur ->_right;
 //   }
 //   else
 //   {
 //     // Value of current node equals the val_ to insert
 //     return std::make_pair(iterator(_cur), false);
 //   }
 // }

 // return std::make_pair(insert(static_cast<NodePtr>(_cur),
 //                       static_cast<NodePtr>(_par),
 //                       val_), true);
  NodePtr par = _header;
  NodePtr cur = root();
  bool comp = true;
  while (cur != nullptr)
  {
    par = cur;
    comp = _comparator(_keyExtractor(val_), key(cur));
    cur = comp ? left(cur) : right(cur);
  }

  iterator it { par };
  if (comp)
  {
    if (it == begin())
      return std::make_pair(insert(static_cast<NodePtr>(cur), 
                                   static_cast<NodePtr>(par),
                                   val_), true);
    else
      --it; // The last element along the path that has value <= val_
  }

  if (_comparator(key(static_cast<NodePtr>(it._pnode)),
                  _keyExtractor(val_)))
    return std::make_pair(insert(static_cast<NodePtr>(cur),
                                 static_cast<NodePtr>(par),
                                 val_), true);

  return std::make_pair(it, false);
}

template <typename Key, typename Value, class KeyOfValue,
          typename Comp, template <typename> class Alloc>
void RBTree<Key, Value, KeyOfValue, Comp, Alloc>::rebalance(RBNodeBase* toInsert_, RBNodeBase*& root_)
{
  // RBTree: the count of black nodes along all paths should be the same,
  // so the inserted node must be red.
  // If its parent is not black, then we need to do rotation
  toInsert_->_color = RBTreeNodeColor::Red;
  while (toInsert_ != root_ && toInsert_->_parent->_color == RBTreeNodeColor::Red)
  {
    auto parent = toInsert_->_parent;
    auto grandpa = parent->_parent;

    if (parent == grandpa->_left)
    {
      // Sibling of father node
      auto uncle = grandpa->_right;
      if (uncle == nullptr || color(uncle) == RBTreeNodeColor::Black)
      {
        if (toInsert_ == parent->_right)
        {
          // Left rotate first
          toInsert_ = parent;
          leftRotate(toInsert_, root_);
          parent = toInsert_->_parent;
        }
        parent->_color = RBTreeNodeColor::Black;
        grandpa->_color = RBTreeNodeColor::Red;
        rightRotate(grandpa, root_);
      }
      else
      {
        parent->_color = RBTreeNodeColor::Black;
        uncle->_color = RBTreeNodeColor::Black;
        grandpa->_color = RBTreeNodeColor::Red;
        toInsert_ = grandpa;  // Needs to check upwards
      }
    }
    else
    {
      auto uncle = toInsert_->_parent->_parent->_left;
      if (uncle == nullptr || color(uncle) == RBTreeNodeColor::Black)
      {
        if (toInsert_ == grandpa->_left)
        {
          toInsert_ = parent;
          rightRotate(toInsert_, root_);
          parent = toInsert_->_parent;
        }
        parent->_color = RBTreeNodeColor::Black;
        grandpa->_color = RBTreeNodeColor::Red;
        leftRotate(grandpa, root_);
      }
      else
      {
        parent->_color = RBTreeNodeColor::Black;
        uncle->_color = RBTreeNodeColor::Black;
        grandpa->_color = RBTreeNodeColor::Red;
        toInsert_ = grandpa;
      }
    }
  }

  // The root node is always black
  root_->_color = RBTreeNodeColor::Black;
}

template <typename Key, typename Value, class KeyOfValue,
          typename Comp, template <typename> class Alloc>
void RBTree<Key, Value, KeyOfValue, Comp, Alloc>::leftRotate(RBNodeBase* node_, RBNodeBase*& root_)
{
  bool isRoot = (node_ == root_);
  auto parent = node_->_parent;

  // Could be neither(consider the root case, guarded by isRoot
  bool isLeft = (node_ == parent->_left) ? true : false;

  auto newRoot = node_->_right;
  node_->_right = newRoot->_left;

  if (node_->_right != nullptr)
    node_->_right->_parent = node_;

  node_->_parent = newRoot;
  newRoot->_left = node_;
  newRoot->_parent = parent;

  if (isRoot) root_ = newRoot;
  else if (isLeft) parent->_left = newRoot;
  else parent->_right = newRoot;
}

template <typename Key, typename Value, class KeyOfValue,
          typename Comp, template <typename> class Alloc>
void RBTree<Key, Value, KeyOfValue, Comp, Alloc>::rightRotate(RBNodeBase* node_, RBNodeBase*& root_)
{
  bool isRoot = (node_ == root_);
  auto parent = node_->_parent;

  bool isLeft = (node_ == parent->_left) ? true : false;

  auto newRoot = node_->_left;
  node_->_left = newRoot->_right;

  if (node_->_left != nullptr)
    node_->_left->_parent = node_;

  node_->_parent = newRoot;
  newRoot->_right = node_;
  newRoot->_parent = parent;

  if (isRoot) root_ = newRoot;
  else if (isLeft) parent->_left = newRoot;
  else parent->_right = newRoot;
}

namespace detail
{
  bool isLeaf(RBNodeBase* node_)
  { return node_->_left == nullptr && node_->_right == nullptr; }

  bool hasLeft(RBNodeBase* node_)
  { return node_->_left != nullptr; }

  bool hasRight(RBNodeBase* node_)
  { return node_->_right != nullptr; }

  bool hasBothChildren(RBNodeBase* node_)
  { return hasLeft(node_) && hasRight(node_); }

  RBNodeBase* sibling(RBNodeBase* node_, RBNodeBase* parent_)
  {
    return (node_ == parent_->_left) ? parent_->_right : parent_->_left;
  }

  void calculateBlackDepth(RBNodeBase* node_, std::vector<int>& depths_, int curDepth)
  {
    if (node_ == nullptr) return;
    if (node_->_color == RBTreeNodeColor::Black) ++curDepth;
    if (isLeaf(node_)) depths_.push_back(curDepth);
    if (hasLeft(node_)) calculateBlackDepth(node_->_left, depths_, curDepth);
    if (hasRight(node_)) calculateBlackDepth(node_->_right, depths_, curDepth);
  }

  bool noConsecutiveReds(RBNodeBase* node_)
  {
    if (node_ == nullptr) return true;
    if (isLeaf(node_)) return true;
    if (node_->_color == RBTreeNodeColor::Red)
    {
      if (hasLeft(node_) &&
          node_->_left->_color == RBTreeNodeColor::Red)
      {
        return false;
      }
      if (hasRight(node_) &&
          node_->_right->_color == RBTreeNodeColor::Red)
      {
        return false;
      }
    }

    if (!noConsecutiveReds(node_->_left)) return false;
    return noConsecutiveReds(node_->_right);
  }
}

template <typename Key, typename Value, class KeyOfValue,
          typename Comp, template <typename> class Alloc>
bool RBTree<Key, Value, KeyOfValue, Comp, Alloc>::rbPropertyKept() const
{
  // (0.0) When tree is empty, _header' _left and _right points
  // to _header itself
  if (_header->_parent == nullptr)
  {
    if (_header->_left != _header ||
        _header->_right != _header)
      return false;
  }

  // (0.1) _header->_left points to the smallest node of the tree
  // _header->_right points to the largest node of the tree
  if (_header->_parent != nullptr)
  {
    std::cout << "0.1" << std::endl;
    if (_header->_left != RBNodeBase::minnode(_header->_parent) ||
        _header->_right != RBNodeBase::maxnode(_header->_parent))
      return false;
  }

  // (1) Header color: red
  if (_header->_color != RBTreeNodeColor::Red)
  {
    return false;
  }

  // (2) root node(if exist) must be Black
  if (_header->_parent != nullptr &&
      _header->_parent->_color != RBTreeNodeColor::Black)
  {
    return false;
  }

  // (3) All path have the same black count
  std::vector<int> blackDepths;
  detail::calculateBlackDepth(_header->_parent, blackDepths, 0);
  if (!blackDepths.empty())
  {
    auto depth = blackDepths.front();
    for (const auto& d : blackDepths)
    {
      if (d != depth)
      {
        return false;
      }
    }
  }

  // (4) No two consecutive (parent/child) node are all red
  return detail::noConsecutiveReds(_header->_parent);
}

template <typename Key, typename Value, class KeyOfValue,
          typename Comp, template <typename> class Alloc>
typename RBTree<Key, Value, KeyOfValue, Comp, Alloc>::iterator
RBTree<Key, Value, KeyOfValue, Comp, Alloc>::find(const key_type& key_) const
{
  auto cur = root();
  while (cur != nullptr && key(cur) != key_)
  {
    if (_comparator(key_, key(cur)))
      cur = static_cast<NodePtr>(cur->_left);
    else
      cur = static_cast<NodePtr>(cur->_right);
  }
  if (cur != nullptr) return cur;
  return _header;
}

template <typename Key, typename Value, class KeyOfValue,
          typename Comp, template <typename> class Alloc>
std::pair<typename RBTree<Key, Value, KeyOfValue, Comp, Alloc>::iterator, bool>
RBTree<Key, Value, KeyOfValue, Comp, Alloc>::erase(iterator iter_)
{
  // If this node has two children, then switch position with 
  // the next smallest one and erase it there
  assert(iter_ != end());

  iterator next = iter_;
  ++next;
  if (detail::hasBothChildren(iter_._pnode))
  {
    // Switch position of iter_ and next_
    NodePtr node = static_cast<NodePtr>(iter_._pnode);
    NodePtr nnode = static_cast<NodePtr>(next._pnode);

    // TODO move if possible
    node->_value = nnode->_value;
    
    eraseOneChild(next._pnode);
    return std::make_pair(iterator(node), true);
  }
  else
  {
    eraseOneChild(iter_._pnode);
    return std::make_pair(next, true);
  }
}

template <typename Key, typename Value, class KeyOfValue,
          typename Comp, template <typename> class Alloc>
void RBTree<Key, Value, KeyOfValue, Comp, Alloc>::switchNode(RBNodeBase* parent_, RBNodeBase* child_)
{
  auto grandpa = parent_->_parent;

  if (parent_ == grandpa->_left)
    grandpa->_left = child_;
  else if (parent_ == grandpa->_right) // This check is intended for root case
    grandpa->_right = child_;

  if (parent_ == grandpa->_parent) // parent_(node to be erased) is root node
    grandpa->_parent = child_;
  else
  {
    // Non-root case, leftmost and rightmost may need to be changed
    if (parent_ == leftmost())
    {
      _header->_left = (child_ == nullptr) ? grandpa : RBNodeBase::minnode(child_);
    }

    if (parent_ == rightmost())
    {
      _header->_right = (child_ == nullptr) ? grandpa : RBNodeBase::maxnode(child_);
    }
  }

  if (child_ != nullptr) child_->_parent = grandpa;
}

template <typename Key, typename Value, class KeyOfValue,
          typename Comp, template <typename> class Alloc>
void RBTree<Key, Value, KeyOfValue, Comp, Alloc>::eraseCase1(RBNodeBase* node_, RBNodeBase* parent_)
{
  // Case1: child is now root and is black
  // Nothing needs to be changed
  if (parent_ == _header)
  {
    assert(node_ == nullptr || node_->_color == RBTreeNodeColor::Black);
  }
  else
  {
    eraseCase2(node_, parent_);
  }
}

template <typename Key, typename Value, class KeyOfValue,
          typename Comp, template <typename> class Alloc>
void RBTree<Key, Value, KeyOfValue, Comp, Alloc>::eraseCase2(RBNodeBase* node_, RBNodeBase* parent_)
{
  assert(parent_ != _header);
  RBNodeBase* sibling = detail::sibling(node_, parent_);
  assert(sibling != nullptr);
  if (sibling != nullptr && sibling->_color == RBTreeNodeColor::Red)
  {
    assert(parent_->_color == RBTreeNodeColor::Black);
    sibling->_color = RBTreeNodeColor::Black;
    parent_->_color = RBTreeNodeColor::Red;
    leftRotate(parent_, _header->_parent);
  }
  eraseCase3(node_, parent_);
}

template <typename Key, typename Value, class KeyOfValue,
          typename Comp, template <typename> class Alloc>
void RBTree<Key, Value, KeyOfValue, Comp, Alloc>::eraseCase3(RBNodeBase* node_, RBNodeBase* parent_)
{
  RBNodeBase* sibling = detail::sibling(node_, parent_);
  assert(sibling != nullptr);
  if (parent_->_color == RBTreeNodeColor::Black &&
      sibling->_color == RBTreeNodeColor::Black &&
      (sibling->_left == nullptr || sibling->_left->_color == RBTreeNodeColor::Black) &&
      (sibling->_right == nullptr || sibling->_right->_color == RBTreeNodeColor::Black))
  {
    sibling->_color = RBTreeNodeColor::Red;
    eraseCase1(parent_, parent_->_parent);
  }
  else
    eraseCase4(node_, parent_);
}

template <typename Key, typename Value, class KeyOfValue,
          typename Comp, template <typename> class Alloc>
void RBTree<Key, Value, KeyOfValue, Comp, Alloc>::eraseCase4(RBNodeBase* node_, RBNodeBase* parent_)
{
  RBNodeBase* sibling = detail::sibling(node_, parent_);
  assert(sibling != nullptr);
  if (parent_->_color == RBTreeNodeColor::Red &&
      sibling->_color == RBTreeNodeColor::Black &&
      (sibling->_left == nullptr || sibling->_left->_color == RBTreeNodeColor::Black) &&
      (sibling->_right == nullptr || sibling->_right->_color == RBTreeNodeColor::Black))
  {
    parent_->_color = RBTreeNodeColor::Black;
    sibling->_color = RBTreeNodeColor::Red;
  }
  else
    eraseCase5(node_, parent_);
}

template <typename Key, typename Value, class KeyOfValue,
          typename Comp, template <typename> class Alloc>
void RBTree<Key, Value, KeyOfValue, Comp, Alloc>::eraseCase5(RBNodeBase* node_, RBNodeBase* parent_)
{
  RBNodeBase* sibling = detail::sibling(node_, parent_);
  assert(sibling != nullptr);
  if (node_ == parent_->_left &&
      sibling->_color == RBTreeNodeColor::Black &&
      (sibling->_left != nullptr && sibling->_left->_color == RBTreeNodeColor::Red) &&
      (sibling->_right == nullptr || sibling->_right->_color == RBTreeNodeColor::Black))
  {
    auto newSibling = sibling->_left;
    rightRotate(sibling, _header->_parent);
    sibling->_color = RBTreeNodeColor::Red;
    newSibling->_color = RBTreeNodeColor::Black;
  }
  else if (node_ == parent_->_right &&
           sibling->_color == RBTreeNodeColor::Black &&
           (sibling->_left == nullptr || sibling->_left->_color == RBTreeNodeColor::Black) &&
           (sibling->_right != nullptr && sibling->_right->_color == RBTreeNodeColor::Red))
  {
    auto newSibling = sibling->_right;
    leftRotate(sibling, _header->_parent);
    sibling->_color = RBTreeNodeColor::Red;
    newSibling->_color = RBTreeNodeColor::Black;
  }
  eraseCase6(node_, parent_);
}

template <typename Key, typename Value, class KeyOfValue,
          typename Comp, template <typename> class Alloc>
void RBTree<Key, Value, KeyOfValue, Comp, Alloc>::eraseCase6(RBNodeBase* node_, RBNodeBase* parent_)
{
  RBNodeBase* sibling = detail::sibling(node_, parent_);
  assert(sibling != nullptr);
  assert(sibling->_color == RBTreeNodeColor::Black);
  sibling->_color = parent_->_color;
  parent_->_color = RBTreeNodeColor::Black;

  if (node_ == parent_->_left)
  {
    if (sibling->_right != nullptr)
    {
      assert(sibling->_right->_color == RBTreeNodeColor::Red);
      sibling->_right->_color = RBTreeNodeColor::Black;
    }
    leftRotate(parent_, _header->_parent);
  }
  else
  {
    if (sibling->_left != nullptr)
    {
      assert(sibling->_right->_color == RBTreeNodeColor::Red);
      sibling->_left->_color = RBTreeNodeColor::Black;
    }
    rightRotate(parent_, _header->_parent);
  }
}

template <typename Key, typename Value, class KeyOfValue,
          typename Comp, template <typename> class Alloc>
void RBTree<Key, Value, KeyOfValue, Comp, Alloc>::eraseOneChild(RBNodeBase* node_)
{
  assert(!detail::hasBothChildren(node_));
  
  auto child = (detail::hasLeft(node_)) ? node_->_left : node_->_right; 
  auto parent = node_->_parent;

  switchNode(node_, child);
  if (node_->_color == RBTreeNodeColor::Black)
  {
    // the node to be deleted is Black
    if (child != nullptr && child->_color == RBTreeNodeColor::Red)
    {
      child->_color = RBTreeNodeColor::Black;
    }
    else
    {
      eraseCase1(child, parent);
    }
  }

  destroyNode(static_cast<NodePtr>(node_));
  --_nodeCount;
}
