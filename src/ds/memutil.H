#ifndef MEMUTIL_H_
#define MEMUTIL_H_

#include "traits.H"
#include "alloc.H"

namespace oplib
{
    template<typename ForwardIter, typename Size, typename T>
    inline auto _uninitialized_fill_n_aux(ForwardIter first, Size n, const T& x, true_type) 
    {
        for (; n > 0; --n, ++first)
        {
            *first = x;
        }
        return first;
    }

    template<typename ForwardIter, typename Size, typename T>
    inline auto _uninitialized_fill_n_aux(ForwardIter first, Size n, const T& x, false_type)
    {
        ForwardIter cur = first;
        for (; n > 0; --n, ++cur)
        {
            _construct(&*cur, x);
        }
        return cur;
    }

    template<typename ForwardIter, typename Size, typename T, typename Tv>
    inline auto _uninitialized_fill_n(ForwardIter first, Size n, const T& x, Tv*)
    {
        typedef typename type_traits<Tv>::is_pod_type is_pod;
        return _uninitialized_fill_n_aux(first, n, x, is_pod());
    }

    template<typename ForwardIter, typename Size, typename T>
    inline auto uninitialized_fill_n(ForwardIter first, Size n, const T& x)
    {
        return _uninitialized_fill_n(first, n, x, value_type(first));
    }
}

#endif
