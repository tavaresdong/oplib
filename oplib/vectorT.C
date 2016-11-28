
template <typename T, typename Alloc>
void Vector<T, Alloc>::insert_aux(iterator position, const value_type& x)
{
    if (finish != eos)
    {
        allocator.construct(finish, *(finish - 1));
        ++finish;
        value_type copy_ele = x;
        std::copy_backward(position, finish - 2, finish - 1);
        *position = copy_ele;
    }
    else
    {
        const size_type old_size = size();
        const size_type new_size = old_size == 0 ? 1 : old_size * 2;

        iterator new_start = allocator.allocate(new_size);
        iterator new_finish = new_start;

        try {
            new_finish = std::uninitialized_copy(start, position, new_start);
            allocator.construct(new_finish, x);
            ++new_finish;
            new_finish = std::uninitialized_copy(position, finish, new_finish);
        }
        catch (...)
        {
            allocator.destroy(new_start, new_finish);
            allocator.deallocate(new_start, new_size);
            throw;
        }

        allocator.destroy(start, finish);
        deallocate();

        start = new_start;
        finish = new_finish;
        eos = new_start + new_size;
    }
}

template <typename T, typename Alloc>
void Vector<T, Alloc>::insert(iterator position, size_type n, const T& x)
{
    if (n > 0)
    {
        if (size_type(eos - finish) >= n)
        {
            // enough space
            T x_copy = x;
            iterator old_finish = finish;
            const size_type elems_after_pos = finish - position;
            if (elems_after_pos > n)
            {
                std::uninitialized_copy(finish - n, finish, finish);
                finish += n;
                std::copy_backward(position, old_finish - n, old_finish);
                std::fill(position, position + n, x_copy);
            }
            else
            {
                _uninitialized_fill_n(finish, n - elems_after_pos, x_copy);
                finish += (n - elems_after_pos);
                std::uninitialized_copy(position, old_finish, position);
                finish += elems_after_pos;
                std::fill(position, old_finish, x_copy);
            }
        }
        else
        {
            const size_type old_size = size();
            const size_type len = old_size + std::max(old_size, n);
            iterator new_start = allocator.allocate(len);
            iterator new_finish = new_start;
            try {
                new_finish = std::uninitialized_copy(start, position, new_start);
                new_finish = _uninitialized_fill_n(new_finish, n, x);
                new_finish = std::uninitialized_copy(position, finish, new_finish);
            }
            catch (...)
            {
                allocator.destroy(new_start, new_finish);
                allocator.deallocate(new_start, len);
                throw;
            }
            
            // Change to new space
            allocator.destroy(start, finish);
            deallocate();
            start = new_start;
            finish = new_finish;
            eos = start + len;
        }
    }
}

template <typename T, typename Alloc>
void Vector<T, Alloc>::resize(size_type new_size, const T& x)
{
    if (new_size < size())
        erase(begin() + new_size, end());
    else
        insert(end(), new_size - size(), x);
}

template <typename T, typename Alloc>
Vector<T, Alloc>::Vector(const Vector<T, Alloc>& v)
{
    start = allocator.allocate(v.size());
    finish = start + v.size();
    eos = finish;
    std::uninitialized_copy(v.begin(), v.end(), start);
}

template <typename T, typename Alloc>
Vector<T, Alloc>& Vector<T, Alloc>::operator = (const Vector<T, Alloc>& rhs)
{
    if (this != &rhs)
    {
        const size_type len = rhs.size();
        iterator new_start = allocator.allocate(len);
        iterator new_finish = new_start;
        try {
            new_finish = std::uninitialized_copy(rhs.begin(), rhs.end(), new_start);
        }
        catch (...)
        {
            allocator.destroy(new_start, new_finish);
            allocator.deallocate(new_start, len);
            throw;
        }
        allocator.destroy(start, finish);
        deallocate();
        start = new_start;
        finish = new_finish;
        eos = start + len;
    }
}
