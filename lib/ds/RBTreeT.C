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
    // _pnode is at Header guard: header's parent is root, root's parent is header.
    // and root must be black, header is red
    _pnode = _pnode->_right;
  }
  if (_pnode->_left != nullptr)
  {
    return RBNodeBase::maxnode(_pnode->_left);
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
void RBTree<Key, Value, KeyOfValue, Comp, Alloc>::clear()
{
  //TODO
}

template <typename Key, typename Value, class KeyOfValue,
          typename Comp, template <typename> class Alloc>
RBTree<Key, Value, KeyOfValue, Comp, Alloc>::RBTree(const Comp& comp_)
: _allocator(NodeAlloc()),
  _nodeCount(0),
  _comparator(comp_)
{
  init();
}
