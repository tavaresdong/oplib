#ifndef TRAITS_H_
#define TRAITS_H_

#include <cstddef>

struct input_iterator_tag {};
struct output_iterator_tag {};
struct forward_iterator_tag : public input_iterator_tag {};
struct bidirectional_iterator_tag : public forward_iterator_tag {};
struct random_access_iterator_tag : public bidirectional_iterator_tag {};

namespace oplib 
{
    template <typename Iterator>
    struct iterator_traits
    {
        typedef typename Iterator::iterator_category  iterator_category;
        typedef typename Iterator::value_type         value_type;
        typedef typename Iterator::difference_type    difference_type;
        typedef typename Iterator::pointer            pointer;
        typedef typename Iterator::reference          reference;
    };

    template <typename T>
    struct iterator_traits<T*>
    {
        typedef random_access_iterator_tag            iterator_category;
        typedef T                                     value_type;
        typedef std::ptrdiff_t                        difference_type;
        typedef T*                                    pointer;
        typedef T&                                    reference;
    };

    template <typename T>
    struct iterator_traits<const T*>
    {
        typedef random_access_iterator_tag            iterator_category;
        typedef T                                     value_type;
        typedef std::ptrdiff_t                        difference_type;
        typedef const T*                                    pointer;
        typedef const T&                                    reference;
    };

    // Use auto to deduce the returned type
    template <typename Iterator>
    auto iterator_category(const Iterator&)
    {
        typedef typename iterator_traits<Iterator>::iterator_category cate;
        return cate();
    }

    template <typename Iterator>
    auto distance_type(const Iterator&)
    {
        return static_cast<typename iterator_traits<Iterator>::difference_type*>(0);
    }

    template <typename Iterator>
    auto value_type(const Iterator&)
    {
        return static_cast<typename iterator_traits<Iterator>::value_type*>(0);
    }


    /* Type Traits */
    struct true_type {};
    struct false_type {};

    // For simplicity, I only consider if an object is POD
    // that is, it can be treated as primitive types.
    template <typename T>
    struct type_traits
    {
        typedef false_type is_pod_type;
    };

    template <>
    struct type_traits<int>
    {
        typedef true_type is_pod_type;
    };

    template <>
    struct type_traits<char>
    {
        typedef true_type is_pod_type;
    };

    template <>
    struct type_traits<double>
    {
        typedef true_type is_pod_type;
    };

    template <>
    struct type_traits<float>
    {
        typedef true_type is_pod_type;
    };

    template <>
    struct type_traits<unsigned int>
    {
        typedef true_type is_pod_type;
    };

    template <>
    struct type_traits<long>
    {
        typedef true_type is_pod_type;
    };

    template <>
    struct type_traits<unsigned long>
    {
        typedef true_type is_pod_type;
    };

    template <>
    struct type_traits<short>
    {
        typedef true_type is_pod_type;
    };

    template <>
    struct type_traits<unsigned short>
    {
        typedef true_type is_pod_type;
    };

    template <>
    struct type_traits<long double>
    {
        typedef true_type is_pod_type;
    };

    // Partial specialization for pointers, they are also treated as POD
    template <typename T>
    struct type_traits<T*>
    {
        typedef true_type is_pod_type;
    };
}

#endif
