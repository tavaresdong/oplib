#ifndef OPLIB_DS_SET_H
#define OPLIB_DS_SET_H

#include <functional>
#include <algorithm>

#include "RBTree.H"

namespace oplib
{
namespace ds
{

namespace detail
{
  template <typename T>
  struct Identity 
  {
    const T& operator () (const T& val_)
    { return val_; }
  };
}

  template <
            typename T, 
            typename Comp = std::less<T>,
            class Alloc = std::allocator<T>
           >
  class Set
  {
   public:
    using key_type = T;
    using value_type = T;
    using key_compare = Comp;
    using value_compare = Comp;
    using allocator_type = Alloc;
   private:
    using imp_type = oplib::ds::RBTree<T, T, detail::Identity<T>, Comp, Alloc>;
   public:
    using reference = typename imp_type::reference;
    using const_reference = typename imp_type::const_reference;
    using pointer = typename imp_type::pointer;
    using const_pointer = typename imp_type::const_pointer;
    using iterator = typename imp_type::iterator;
    using const_iterator = typename imp_type::const_iterator;
    using size_type = typename imp_type::size_type;


   public:
    // Constructor/Destructors
    explicit Set(const key_compare& comp_ = key_compare(),
                 const allocator_type& alloc_ = allocator_type())
    : _impl(comp_, alloc_)
    {}

    template <typename InputIterator>
    Set(InputIterator first_, InputIterator last_,
        const key_compare& comp_ = key_compare(),
        const allocator_type& alloc_ = allocator_type())
    : _impl(comp_, alloc_)
    {
      std::for_each(first_, last_, [&] (const auto& val_) {
                                     _impl.insertUnique(val_);
                                   });
    }

    ~Set() {}

   public:
    // Accessors
    size_type size() const { return _impl.size(); }

   private:
    imp_type _impl;
  };

}
}
#endif
