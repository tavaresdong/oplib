
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
