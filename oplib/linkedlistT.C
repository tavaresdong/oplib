
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
