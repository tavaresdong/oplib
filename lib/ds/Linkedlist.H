#ifndef OPLIB_LINKEDLIST_H_
#define OPLIB_LINKEDLIST_H_

#include "alloc.H"
#include "memutil.H"
#include <memory>

#include <algorithm>
#include <utility>

#include <iostream>

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
  

  template <typename T, template <typename> class Alloc = opallocator >
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
    using node_ptr = list_node*;
    using NodeAlloc = Alloc<list_node>;

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

    void transfer(iterator position, iterator first, iterator last);

  public:
    // Constructors Destructors
    Linkedlist() { empty_init(); }
    ~Linkedlist();
    iterator begin() const { return (node_ptr)((*sentinel).next); }
    iterator end() const { return sentinel; }
    bool empty() const { return sentinel->next == sentinel; }
    size_type size() const
    {
      return std::distance(begin(), end());
    }
    reference front() { return *(begin()); }
    reference back() { return *(--end()); }

    // Element manipulation
    iterator insert(iterator position, const value_type& x);
    void push_back(const value_type& x) { insert(end(), x); }
    void push_front(const value_type& x) { insert(begin(), x); }
    iterator erase(iterator position);
    void pop_front() { erase(begin()); }
    void pop_back() { erase(--end()); }
    void clear();
    void remove(const value_type& x);

    template <typename UnaryPredicate>
    void remove_if(UnaryPredicate up);
    void unique();

    void splice(iterator position, Linkedlist<T, Alloc>& x);
    void splice(iterator position, Linkedlist<T, Alloc>& nomatter, iterator i);
    void splice(iterator position, iterator first, iterator last);

    void reverse();

    // Reverse a range of elemens: [first. last)
    void reverse(iterator first, iterator last);

    // Rotate a list k positions:
    // 1 2 3 4 5 rotate(2) -> 4 5 1 2 3
    void rotate(size_t k);

    // x and *this list should all be sorted!
    void merge(Linkedlist<T, Alloc>& x);
    void sort();
  };

  #include "LinkedlistT.C"
}

#endif
