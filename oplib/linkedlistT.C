
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
