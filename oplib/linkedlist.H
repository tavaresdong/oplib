#ifndef LINKEDLIST_H_
#define LINKEDLIST_H_

#include "alloc.H"
#include "memutil.H"

#include <algorithm>
#include <utility>

// Use std::distance method, and bidirectional_iterator_tag TODO, make my own distance
#include <iterator>

namespace oplib
{
    template <typename T>
    struct _list_node
    {
        using void_pointer = void*;
        void_pointer prev;
        void_pointer next;
        T data;
    };

    template <typename T, typename Ref, typename Ptr>
    struct _list_iterator
    {
        typedef _list_iterator<T, T&, T*> iterator;
        typedef _list_iterator<T, Ref, Ptr> self;

        // std::bidirectional_iterator_tag, combined to use with std::advance
        typedef std::bidirectional_iterator_tag iterator_category;
        typedef T value_type;
        typedef Ref reference;
        typedef Ptr pointer;
        typedef _list_node<T>* node_ptr;
        typedef size_t size_type;
        typedef ptrdiff_t difference_type;

        node_ptr node;
        
        _list_iterator(node_ptr x) : node(x) { }
        _list_iterator() : node(nullptr) { };
        _list_iterator(const iterator& iter) : node(iter.node) { }

        bool operator == (const self& x) { return node == x.node; }
        bool operator != (const self& x) { return node != x.node; }
        
        reference operator * () { return node->data; }
        reference operator -> () { return &(operator*()); }
        
        self& operator ++ () { node = node_ptr(node->next); return *this; }
        self  operator ++ (int) { self tmp = *this; ++*this; return tmp; }
    
        self& operator -- () { node = node_ptr(node->prev); return *this; }
        self  operator -- (int) { self tmp = *this; --*this; return tmp; }
    };
    

    template <typename T, typename Alloc = opallocator<T> >
    class Linkedlist
    {
    public:
        typedef T value_type;
        typedef value_type* pointer;
        typedef value_type& reference;
        typedef const value_type* const_pointer;
        typedef const value_type& const_reference;
        typedef _list_iterator<value_type, reference, pointer> iterator;
        typedef size_t size_type;

    protected:
        using list_node = _list_node<T>;
        using NodeAlloc = opallocator<list_node>;
        using node_ptr = list_node*;

        NodeAlloc allocator;
        node_ptr sentinel;
    
        node_ptr get_node() { return allocator.allocate(1); }
        node_ptr create_node(const value_type& x)
        {
            node_ptr np = get_node();
            oplib::_construct(&np->data, x);
            return np;
        }
        void destroy_node(node_ptr np) { oplib::_destroy(&np->data); }
        void put_node(node_ptr np) { allocator.deallocate(np, 1); }
        
        void empty_init()
        {
            sentinel = get_node();
            sentinel->next = sentinel;
            sentinel->prev = sentinel;
        }

    public:
        // Constructors Destructors
        Linkedlist() { empty_init(); }
        ~Linkedlist() { /* TODO  */}
        iterator begin() const { return (node_ptr)((*sentinel).next); }
        iterator end() const { return sentinel; }
        bool empty() const { return sentinel->next == sentinel; }
        size_type size() const
        {
            return std::distance(begin(), end());
        }
        reference front() { return *(begin()); }
        reference back() { return *(--end()); }

        // Memory manipulation
        iterator insert(iterator position, const value_type& x);
        void push_back(const value_type& x) { insert(end(), x); }
        void push_front(const value_type& x) { insert(begin(), x); }
        iterator erase(iterator position);
    };

    #include "linkedlistT.C"
}

#endif
