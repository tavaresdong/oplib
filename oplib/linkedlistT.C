template <typename T, typename Alloc>
Linkedlist<T, Alloc>::~Linkedlist()
{
    clear();
    destroy_node(sentinel);
    put_node(sentinel);
}

template <typename T, typename Alloc>
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

template <typename T, typename Alloc>
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

template <typename T, typename Alloc>
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
template <typename T, typename Alloc>
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

template <typename T, typename Alloc>
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
template <typename T, typename Alloc>
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

template <typename T, typename Alloc>
void
Linkedlist<T, Alloc>::transfer(iterator position, iterator first, iterator last)
{
    ((node_ptr) last.node->prev)->next = position.node;
    ((node_ptr) first.node->prev)->next = last.node;
    ((node_ptr) position.node->prev)->next = first.node;
    node_ptr prev_pos = (node_ptr) position.node->prev;
    position.node->prev = last.node->prev;
    last.node->prev = first.node->prev;
    first.node->prev = prev_pos;
}

template <typename T, typename Alloc>
void
Linkedlist<T, Alloc>::splice(iterator position, Linkedlist<T, Alloc>& x)
{
    transfer(position, x.begin(), x.end());
} 

template <typename T, typename Alloc>
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
            iterator last = --end();
            transfer(first, last, end());
        }
    }
}
