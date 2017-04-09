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
    return;
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
          typename Comp, class Alloc>
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
          typename Comp, class Alloc>
RBTree<Key, Value, KeyOfValue, Comp, Alloc>::RBTree(const Comp& comp_, const allocator_type&)
: _allocator(typename allocator_type::template rebind<NodeAlloc>::other()),
  _nodeCount(0),
  _comparator(comp_),
  _keyExtractor(KeyOfValue())
{
  init();
}

template <typename Key, typename Value, class KeyOfValue,
          typename Comp, class Alloc>
RBTree<Key, Value, KeyOfValue, Comp, Alloc>::RBTree(std::initializer_list<value_type> il,
                                                    const Comp& comp_, 
                                                    const allocator_type&)
: _allocator(typename allocator_type::template rebind<NodeAlloc>::other()),
  _nodeCount(0),
  _comparator(comp_),
  _keyExtractor(KeyOfValue())
{
  init();
  std::for_each(il.begin(), il.end(), [&] (const auto& val_) {
                                        this->insertUnique(val_);
                                      });
}

template <typename Key, typename Value, class KeyOfValue,
          typename Comp, class Alloc>
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
          typename Comp, class Alloc>
typename RBTree<Key, Value, KeyOfValue, Comp, Alloc>::iterator
RBTree<Key, Value, KeyOfValue, Comp, Alloc>::insert(NodePtr /* child */, NodePtr parent_, const value_type& val_)
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
          typename Comp, class Alloc>
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
          typename Comp, class Alloc>
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
          typename Comp, class Alloc>
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
  inline bool isLeaf(RBNodeBase* node_)
  { return node_->_left == nullptr && node_->_right == nullptr; }

  inline bool hasLeft(RBNodeBase* node_)
  { return node_->_left != nullptr; }

  inline bool hasRight(RBNodeBase* node_)
  { return node_->_right != nullptr; }

  inline bool hasBothChildren(RBNodeBase* node_)
  { return hasLeft(node_) && hasRight(node_); }

  inline RBNodeBase* sibling(RBNodeBase* node_, RBNodeBase* parent_)
  {
    return (node_ == parent_->_left) ? parent_->_right : parent_->_left;
  }

  inline void calculateBlackDepth(RBNodeBase* node_, std::vector<int>& depths_, int curDepth)
  {
    if (node_ == nullptr) return;
    if (node_->_color == RBTreeNodeColor::Black) ++curDepth;
    if (isLeaf(node_)) depths_.push_back(curDepth);
    if (hasLeft(node_)) calculateBlackDepth(node_->_left, depths_, curDepth);
    if (hasRight(node_)) calculateBlackDepth(node_->_right, depths_, curDepth);
  }

  inline bool noConsecutiveReds(RBNodeBase* node_)
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
          typename Comp, class Alloc>
bool RBTree<Key, Value, KeyOfValue, Comp, Alloc>::rbPropertyKept() const
{
  if (_header == nullptr)
  {
    if (_nodeCount != 0) return false;
    return true;
  }

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
          typename Comp, class Alloc>
typename RBTree<Key, Value, KeyOfValue, Comp, Alloc>::const_iterator
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

// TODO: not good, duplicate code, need refactor
template <typename Key, typename Value, class KeyOfValue,
          typename Comp, class Alloc>
typename RBTree<Key, Value, KeyOfValue, Comp, Alloc>::iterator
RBTree<Key, Value, KeyOfValue, Comp, Alloc>::find(const key_type& key_)
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
          typename Comp, class Alloc>
typename RBTree<Key, Value, KeyOfValue, Comp, Alloc>::iterator
RBTree<Key, Value, KeyOfValue, Comp, Alloc>::lower_bound(const key_type& key_)
{
  auto par = _header;
  auto cur = root();
  while (cur != nullptr && key(cur) != key_)
  {
    par = cur;
    if (_comparator(key_, key(cur)))
      cur = static_cast<NodePtr>(cur->_left);
    else
      cur = static_cast<NodePtr>(cur->_right);
  }
  if (cur != nullptr) return cur;
  else if (par == _header) return par;
  else
  {
    iterator iter { par };
    if (_comparator(key(par), key_)) ++iter;
    return iter;
  }
}

template <typename Key, typename Value, class KeyOfValue,
          typename Comp, class Alloc>
typename RBTree<Key, Value, KeyOfValue, Comp, Alloc>::const_iterator
RBTree<Key, Value, KeyOfValue, Comp, Alloc>::lower_bound(const key_type& key_) const
{
  auto par = _header;
  auto cur = root();
  while (cur != nullptr && key(cur) != key_)
  {
    par = cur;
    if (_comparator(key_, key(cur)))
      cur = static_cast<NodePtr>(cur->_left);
    else
      cur = static_cast<NodePtr>(cur->_right);
  }
  if (cur != nullptr) return cur;
  else if (par == _header) return par;
  else
  {
    const_iterator iter { par };
    if (_comparator(key(par), key_)) ++iter;
    return iter;
  }
}

template <typename Key, typename Value, class KeyOfValue,
          typename Comp, class Alloc>
typename RBTree<Key, Value, KeyOfValue, Comp, Alloc>::iterator
RBTree<Key, Value, KeyOfValue, Comp, Alloc>::upper_bound(const key_type& key_)
{
  auto par = _header;
  auto cur = root();
  while (cur != nullptr && key(cur) != key_)
  {
    par = cur;
    if (_comparator(key_, key(cur)))
      cur = static_cast<NodePtr>(cur->_left);
    else
      cur = static_cast<NodePtr>(cur->_right);
  }
  if (cur != nullptr)
  {
    iterator iter { cur };
    while (!_comparator(key_, key(static_cast<NodePtr>(iter._pnode)))) ++iter;
    return iter;
  }
  else if (par == _header) return par;
  else
  {
    iterator iter { par };
    while (!_comparator(key_, key(static_cast<NodePtr>(iter._pnode)))) ++iter;
    return iter;
  }
}

template <typename Key, typename Value, class KeyOfValue,
          typename Comp, class Alloc>
typename RBTree<Key, Value, KeyOfValue, Comp, Alloc>::const_iterator
RBTree<Key, Value, KeyOfValue, Comp, Alloc>::upper_bound(const key_type& key_) const
{
  auto par = _header;
  auto cur = root();
  while (cur != nullptr && key(cur) != key_)
  {
    par = cur;
    if (_comparator(key_, key(cur)))
      cur = static_cast<NodePtr>(cur->_left);
    else
      cur = static_cast<NodePtr>(cur->_right);
  }
  if (cur != nullptr)
  {
    const_iterator iter { cur };
    while (!_comparator(key_, key(static_cast<NodePtr>(iter._pnode)))) ++iter;
    return iter;
  }
  else if (par == _header) return par;
  else
  {
    const_iterator iter { par };
    while (!_comparator(key_, key(static_cast<NodePtr>(iter._pnode)))) ++iter;
    return iter;
  }
}

template <typename Key, typename Value, class KeyOfValue,
          typename Comp, class Alloc>
std::pair<typename RBTree<Key, Value, KeyOfValue, Comp, Alloc>::iterator,
          typename RBTree<Key, Value, KeyOfValue, Comp, Alloc>::iterator>
RBTree<Key, Value, KeyOfValue, Comp, Alloc>::equal_range(const key_type& key_)
{
  auto par = _header;
  auto cur = root();
  while (cur != nullptr && key(cur) != key_)
  {
    par = cur;
    if (_comparator(key_, key(cur)))
      cur = static_cast<NodePtr>(cur->_left);
    else
      cur = static_cast<NodePtr>(cur->_right);
  }
  if (cur != nullptr)
  {
    iterator iter { cur }, next { cur };
    while (!_comparator(key_, key(static_cast<NodePtr>(next._pnode)))) ++next;
    return std::make_pair(iter, next);
  }
  else if (par == _header) 
    return std::make_pair(iterator(par), iterator(par));
  else
  {
    iterator iter { par };
    while (!_comparator(key_, key(static_cast<NodePtr>(iter._pnode)))) ++iter;
    return std::make_pair(iter, iter);
  }
}

template <typename Key, typename Value, class KeyOfValue,
          typename Comp, class Alloc>
std::pair<typename RBTree<Key, Value, KeyOfValue, Comp, Alloc>::const_iterator,
          typename RBTree<Key, Value, KeyOfValue, Comp, Alloc>::const_iterator>
RBTree<Key, Value, KeyOfValue, Comp, Alloc>::equal_range(const key_type& key_) const
{
  auto par = _header;
  auto cur = root();
  while (cur != nullptr && key(cur) != key_)
  {
    par = cur;
    if (_comparator(key_, key(cur)))
      cur = static_cast<NodePtr>(cur->_left);
    else
      cur = static_cast<NodePtr>(cur->_right);
  }
  if (cur != nullptr)
  {
    const_iterator iter { cur }, next { cur };
    while (!_comparator(key_, key(static_cast<NodePtr>(next._pnode)))) ++next;
    return std::make_pair(iter, next);
  }
  else if (par == _header) 
    return std::make_pair(iterator(par), iterator(par));
  else
  {
    const_iterator iter { par };
    while (!_comparator(key_, key(static_cast<NodePtr>(iter._pnode)))) ++iter;
    return std::make_pair(iter, iter);
  }
}

template <typename Key, typename Value, class KeyOfValue,
          typename Comp, class Alloc>
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

   // node->_value = nnode->_value;
    swapNode(node, nnode);
    
    eraseOneChild(iter_._pnode);
    return std::make_pair(iterator(next), true);
  }
  else
  {
    eraseOneChild(iter_._pnode);
    return std::make_pair(next, true);
  }
}

template <typename Key, typename Value, class KeyOfValue,
          typename Comp, class Alloc>
void RBTree<Key, Value, KeyOfValue, Comp, Alloc>::swapNode(RBNodeBase* n1, RBNodeBase* n2)
{
  assert(n1 != _header && n2 != _header);
  bool n1Root = (n1->_parent->_parent == n1) ? true : false;
  bool n2Root = (n2->_parent->_parent == n2) ? true : false;

  bool n1Leftmost = (_header->_left == n1) ? true : false;
  bool n1Rightmost = (_header->_right == n1) ? true : false;

  bool n2Leftmost = (_header->_left == n2) ? true : false;
  bool n2Rightmost = (_header->_right == n2) ? true : false;

  auto left1 = n1->_left;
  auto right1 = n1->_right;
  auto parent1 = n1->_parent;
  auto left2 = n2->_left;
  auto right2 = n2->_right;
  auto parent2 = n2->_parent;

  n1->_left = (left2 == n1) ? n2 : left2;
  n1->_right = (right2 == n1) ? n2 : right2;
  n1->_parent = (parent2 == n1) ? n2 : parent2;

  n2->_left = (left1 == n2) ? n1 : left1;
  n2->_right = (right1 == n2) ? n1 : right1;
  n2->_parent = (parent1 == n2) ? n1 : parent1;

  if (n1Root) _header->_parent = n2;
  if (n2Root) _header->_parent = n1;

  if (n1Leftmost) _header->_left = n2;
  if (n1Rightmost) _header->_right = n2;

  if (n2Leftmost) _header->_left = n1;
  if (n2Leftmost) _header->_right = n2;

  std::swap(n1->_color, n2->_color);
}

template <typename Key, typename Value, class KeyOfValue,
          typename Comp, class Alloc>
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
          typename Comp, class Alloc>
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
          typename Comp, class Alloc>
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
          typename Comp, class Alloc>
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
          typename Comp, class Alloc>
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
          typename Comp, class Alloc>
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
          typename Comp, class Alloc>
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
          typename Comp, class Alloc>
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

template <typename Key, typename Value, class KeyOfValue,
          typename Comp, class Alloc>
RBTree<Key, Value, KeyOfValue, Comp, Alloc>::RBTree(const RBTree& rhs_)
: _allocator(rhs_._allocator),
  _nodeCount(rhs_._nodeCount),
  _comparator(rhs_._comparator),
  _keyExtractor(rhs_._keyExtractor)
{
  init();
  if (rhs_.empty())
    return;
  else
  {
    _header->_parent = copy(rhs_.root(), _header);
    _header->_left = RBNodeBase::minnode(_header->_parent);
    _header->_right = RBNodeBase::maxnode(_header->_parent);
  }
}

template <typename Key, typename Value, class KeyOfValue,
          typename Comp, class Alloc>
typename RBTree<Key, Value, KeyOfValue, Comp, Alloc>::NodePtr
RBTree<Key, Value, KeyOfValue, Comp, Alloc>::copy(NodePtr cur_, NodePtr parent_)
{
  if (cur_ == nullptr) return nullptr;
  NodePtr nd = createNode(cur_->_value);
  nd->_parent = parent_;
  nd->_color = cur_->_color;
  nd->_left = copy(static_cast<NodePtr>(cur_->_left), nd);
  nd->_right = copy(static_cast<NodePtr>(cur_->_right), nd);
  return nd;
}

template <typename Key, typename Value, class KeyOfValue,
          typename Comp, class Alloc>
RBTree<Key, Value, KeyOfValue, Comp, Alloc>&
RBTree<Key, Value, KeyOfValue, Comp, Alloc>::operator = (const RBTree<Key, Value, KeyOfValue, Comp, Alloc>& rhs_)
{
  RBTree<Key, Value, KeyOfValue, Comp, Alloc> tmp(rhs_);
  swap(tmp);
  return *this;
}

template <typename Key, typename Value, class KeyOfValue,
          typename Comp, class Alloc>
RBTree<Key, Value, KeyOfValue, Comp, Alloc>::RBTree(RBTree&& rhs_)
: _allocator(std::move(rhs_._allocator)),
  _nodeCount(rhs_._nodeCount),
  _comparator(std::move(rhs_._comparator)),
  _keyExtractor(std::move(rhs_._keyExtractor))
{
  _header = rhs_._header;
  rhs_._nodeCount = 0;
  rhs_._header = nullptr;
}

template <typename Key, typename Value, class KeyOfValue,
          typename Comp, class Alloc>
RBTree<Key, Value, KeyOfValue, Comp, Alloc>&
RBTree<Key, Value, KeyOfValue, Comp, Alloc>::operator = (RBTree<Key, Value, KeyOfValue, Comp, Alloc>&& rhs_) // Yes, by value
{
  RBTree<Key, Value, KeyOfValue, Comp, Alloc> tmp(std::move(rhs_));
  swap(tmp);
  return *this;
}

template <typename Key, typename Value, class KeyOfValue,
          typename Comp, class Alloc>
template <typename... Args>
std::pair<typename RBTree<Key, Value, KeyOfValue, Comp, Alloc>::iterator, bool>
RBTree<Key, Value, KeyOfValue, Comp, Alloc>::emplaceUnique(Args&&... args_)
{
  auto newNode = createNode(std::forward<Args>(args_)...);

  NodePtr par = _header;
  NodePtr cur = root();
  bool comp = true;
  while (cur != nullptr)
  {
    par = cur;
    comp = _comparator(_keyExtractor(newNode->_value), key(cur));
    cur = comp ? left(cur) : right(cur);
  }

  iterator it { par };
  if (comp)
  {
    if (it == begin())
      return std::make_pair(insertNode(static_cast<NodePtr>(cur), 
                                       static_cast<NodePtr>(par),
                                       newNode), true);
    else
      --it; // The last element along the path that has value <= val_
  }

  if (_comparator(key(static_cast<NodePtr>(it._pnode)),
                  _keyExtractor(newNode->_value)))
    return std::make_pair(insertNode(static_cast<NodePtr>(cur),
                                     static_cast<NodePtr>(par),
                                     newNode), true);

  return std::make_pair(it, false);
}

template <typename Key, typename Value, class KeyOfValue,
          typename Comp, class Alloc>
typename RBTree<Key, Value, KeyOfValue, Comp, Alloc>::iterator
RBTree<Key, Value, KeyOfValue, Comp, Alloc>::insertNode(NodePtr /* child */, NodePtr parent_, NodePtr newNode_)
{

  if (parent_ == _header)
  {
    // Case1: empty tree
    parent_->_left = newNode_;
    parent_->_right = newNode_;
    parent_->_parent = newNode_;
  }
  else if (_comparator(_keyExtractor(newNode_->_value), key(parent_)))
  {
    parent_->_left = newNode_;
    if (parent_ == leftmost())
    {
      // Assign leftmost to the new node
      _header->_left = newNode_;
    }
  }
  else
  {
    parent_->_right = newNode_;
    if (parent_ == rightmost())
    {
      // Assign rightmost to the new node
      _header->_right = newNode_;
    }
  }

  // Initialize the newly created node:
  newNode_->_left = newNode_->_right = nullptr;
  newNode_->_parent = parent_;

  rebalance(newNode_, _header->_parent);
  ++_nodeCount;
  return iterator(newNode_);
}

template <typename Key, typename Value, class KeyOfValue,
          typename Comp, class Alloc>
template <typename... Args>
typename RBTree<Key, Value, KeyOfValue, Comp, Alloc>::iterator
RBTree<Key, Value, KeyOfValue, Comp, Alloc>::emplaceUnique(const_iterator iter_, Args&&... args_)
{
  auto newNode = createNode(std::forward<Args>(args_)...);

  // Insert with a hint, if hint is valid, constant time insertion is guaranteed
  if (iter_._pnode == leftmost())
  {
    // (1) iter points to begin()
    if (size() > 0 && _comparator(_keyExtractor(newNode->_value), key(static_cast<NodePtr>(iter_._pnode))))
    {
      return insertNode(static_cast<NodePtr>(iter_._pnode),
                        static_cast<NodePtr>(iter_._pnode),
                        newNode);
    }
  }
  else if (iter_._pnode == _header)
  {
    // (2) iter points to end() and val_ is larger than the biggest
    if (size() > 0 && _comparator(key(static_cast<NodePtr>(rightmost())), _keyExtractor(newNode->_value)))
    {
      return insertNode(nullptr,
                        static_cast<NodePtr>(rightmost()),
                        newNode);
    }
  }
  else
  {
    auto before = iter_;
    --before;

    // If before < val_ < iter, then hint is the right place to insert
    // (1) if iter->left != nullptr then before is the rightmost node of iter->left
    //     so before's right must be nullptr, we can insert here
    // (2) otherwise, iter->left is nullptr and before is iter_'s parent
    //     and we can insert the new node into iter_'s left
    if (_comparator(key(static_cast<NodePtr>(before._pnode)), _keyExtractor(newNode->_value)) &&
        _comparator(_keyExtractor(newNode->_value), key(static_cast<NodePtr>(iter_._pnode))))
    {
      if (right(before._pnode) == nullptr)
      {
        return insertNode(nullptr, static_cast<NodePtr>(before._pnode), newNode);
      }
      else
      {
        return insertNode(nullptr, static_cast<NodePtr>(iter_._pnode), newNode);
      }
    }
  }

  // Fall back to general insert method: O(logn)
  return emplaceUnique(std::forward<Args>(args_)...).first;
}

