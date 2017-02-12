template <typename T, template <typename> class Alloc>
Linkedlist<T, Alloc>::~Linkedlist()
{
  clear();
  destroy_node(sentinel);
  put_node(sentinel);
}

template <typename T, template <typename> class Alloc>
typename Linkedlist<T, Alloc>::iterator
Linkedlist<T, Alloc>::insert(iterator position, const value_type& x)
{
  node_ptr node = create_node(x);
  node->next = position.node;
  node->prev = position.node->prev;
  ((node_ptr) node->prev)->next = node;
  position.node->prev = node;
  return node;
}

template <typename T, template <typename> class Alloc>
typename Linkedlist<T, Alloc>::iterator
Linkedlist<T, Alloc>::erase(iterator position)
{
  iterator prev = (node_ptr) position.node->next;
  ((node_ptr) position.node->prev)->next = position.node->next;
  ((node_ptr) position.node->next)->prev = position.node->prev;
  destroy_node(position.node);
  put_node(position.node);
  return prev;
}

template <typename T, template <typename> class Alloc>
void
Linkedlist<T, Alloc>::clear()
{
  for (auto iter = begin(); iter.node != sentinel;)
  {
      iter = erase(iter);
  }
}

/**
 * Remove all elements with value x
 */
template <typename T, template <typename> class Alloc>
void
Linkedlist<T, Alloc>::remove(const value_type& x)
{
  for (auto iter = begin(); iter.node != sentinel;)
  {
    if (*iter == x)
    {
      iter = erase(iter);
    }
    else
      ++iter;
  }
}

template <typename T, template <typename> class Alloc>
template <typename UnaryPredicate>
void
Linkedlist<T, Alloc>::remove_if(UnaryPredicate up)
{
  for (auto iter = begin(); iter.node != sentinel;)
  {
    if (up(*iter))
    {
      iter = erase(iter);
    }
    else
      ++iter;
 } 
}

/**
 * Remove duplicated adjacent elements
 */
template <typename T, template <typename> class Alloc>
void
Linkedlist<T, Alloc>::unique()
{
  auto iter = begin();
  auto prev_value = *iter;
  for (++iter; iter.node != sentinel; )
  {
    if (*iter == prev_value)
    {
      iter = erase(iter);
    }
    else
    {
      prev_value = *iter;
      ++iter;
    }
  }
}

template <typename T, template <typename> class Alloc>
void
Linkedlist<T, Alloc>::transfer(iterator position, iterator first, iterator last)
{
  // If last == pos, no need to transfer
  // If first == last, empty, don't transfer, or leads to error
  if (position != last && first != last)
  {
    ((node_ptr) last.node->prev)->next = position.node;
    ((node_ptr) first.node->prev)->next = last.node;
    ((node_ptr) position.node->prev)->next = first.node;
    node_ptr prev_pos = (node_ptr) position.node->prev;
    position.node->prev = last.node->prev;
    last.node->prev = first.node->prev;
    first.node->prev = prev_pos;
  }
}

template <typename T, template <typename> class Alloc>
void
Linkedlist<T, Alloc>::splice(iterator position, Linkedlist<T, Alloc>& x)
{
  transfer(position, x.begin(), x.end());
} 

template <typename T, template <typename> class Alloc>
void
Linkedlist<T, Alloc>::splice(iterator position, Linkedlist<T, Alloc>& nomatter, iterator i)
{
  auto next = i;
  ++next;
  if (position == next || position == i)
  {
    return;
  }
  transfer(position, i, next);
} 

template <typename T, template <typename> class Alloc>
void
Linkedlist<T, Alloc>::splice(iterator position, iterator first, iterator last)
{
  if (first != last)
      transfer(position, first, last);
} 

template <typename T, template <typename> class Alloc>
void
Linkedlist<T, Alloc>::reverse()
{
  if (sentinel->next == sentinel || ((node_ptr) sentinel->next)->next == sentinel)
  {
    return;
  }
  else
  {
    iterator first = begin();
    while (first.node->next != sentinel)
    {
      transfer(first, --end(), end());
    }
  }
}

template<typename T, template <typename> class Alloc>
void
Linkedlist<T, Alloc>::reverse(iterator first, iterator last)
{
  while (first.node->next != last.node)
  {
    auto toRev = last;
    --toRev;
    transfer(first, toRev, last);
  }
}

template <typename T, template <typename> class Alloc>
void
Linkedlist<T, Alloc>::merge(Linkedlist<T, Alloc>& list)
{
  auto first1 = begin();
  auto last1 = end();
  auto first2 = list.begin();
  auto last2 = list.end();

  while (first1 != last1 && first2 != last2)
  {
    // Use <= to keep order: list1 then list2(for equal elements)
    if (*first1 <= *first2)
    {
      ++first1;
    }
    else
    {
      auto prev = first2;
      transfer(first1, prev, ++first2); 
    }
  }
  
  if (first2 != last2)
    transfer(last1, first2, last2);
}

template <typename T, template <typename> class Alloc>
void
Linkedlist<T, Alloc>::sort()
{
  // Quicksort, TODO: make the quicksort better
  if (sentinel->next == sentinel || ((node_ptr) sentinel->next)->next == sentinel)
  {
    return;
  }

  Linkedlist<T, Alloc> leftlist;
  Linkedlist<T, Alloc> rightlist;
  value_type pivot = front();
  
  // split this list to two smaller lists, and sort them recursively
  for (auto iter = ++begin(); iter != end();)
  {
    auto prev = iter;
    ++iter;
    if (*prev < pivot)
    {
      splice(leftlist.end(), prev, iter);
    }
    else
    {
      splice(rightlist.end(), prev, iter);
    }
  }

  leftlist.sort();
  rightlist.sort();
  merge(leftlist);
  splice(end(), rightlist);
}

// A function to output a list
template <typename T, template <typename> class Alloc>
std::ostream&
operator << (std::ostream& os, const Linkedlist<T, Alloc>& l)
{
  for (auto val : l)
  {
    os << val << " ";
  }
  return os;
}

