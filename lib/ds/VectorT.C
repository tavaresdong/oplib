template <typename T, typename Alloc>
Vector<T, Alloc>& Vector<T, Alloc>::operator = (Vector<T, Alloc>&& rhs)
{
  if (this != &rhs)
  {
    destroy(start, finish);
    deallocate();
    start = rhs.start;
    finish = rhs.finish;
    cp_ = std::move(rhs.cp_);

    rhs.start = nullptr;
    rhs.finish = nullptr;
    rhs.cp_.first() = nullptr;
  }
  return *this;
}

template <typename T, typename Alloc>
Vector<T, Alloc>::Vector(Vector<T, Alloc>&& v)
: start(v.start), finish(v.finish), cp_(std::move(v.cp_))
{
  // Reset member of v, so it can be 
  // safely freed.
  v.start = nullptr;
  v.finish = nullptr;
  v.cp_.first() = nullptr;
}

template <typename T, typename Alloc>
template <typename InputIter>
Vector<T, Alloc>::Vector(InputIter first, InputIter last, const Alloc& alloc)
: cp_(nullptr, alloc)
{
  using IterTag = typename std::iterator_traits<InputIter>::iterator_category;
  constructIterDispatch(first, last, IterTag());
}

template <typename T, typename Alloc>
template <typename Iter>
void Vector<T, Alloc>::constructIterDispatch(Iter first,
                                             Iter last, 
                                             std::random_access_iterator_tag)
{
  size_t sz = std::distance(first, last);
  start = getAllocator().allocate(sz);
  std::uninitialized_copy_n(first, sz, start);
  finish = start + sz;
  cp_.first() = finish;
}

template <typename T, typename Alloc>
template <typename Iter>
void Vector<T, Alloc>::constructIterDispatch(Iter first,
                                             Iter last, 
                                             std::input_iterator_tag)
{
  for (; first != last; ++first) push_back(*first);
}

template <typename T, typename Alloc>
Vector<T, Alloc>::Vector(std::initializer_list<value_type> il, const Alloc& alloc)
: cp_(nullptr, alloc)
{
  auto sz = il.size();
  start = getAllocator().allocate(sz);
  std::uninitialized_copy_n(il.begin(), sz, start);
  finish = start + sz;
  cp_.first() = finish;
}

template <typename T, typename Alloc>
void Vector<T, Alloc>::swap(Vector& v)
{
  std::swap(start, v.start);
  std::swap(finish, v.finish);
  std::swap(cp_, v.cp_);
}

template <typename T, typename Alloc>
void Vector<T, Alloc>::insertAux(iterator position_, const value_type& x_)
{
  if (finish != cp_.first())
  {
    getAllocator().construct(finish, *(finish - 1));
    ++finish;
    value_type copy_ele = x_;
    std::copy_backward(position_, finish - 2, finish - 1);
    *position_ = copy_ele;
  }
  else
  {
    const size_type old_size = size();
    const size_type new_size = old_size == 0 ? 1 : old_size * 2;

    iterator new_start = getAllocator().allocate(new_size);
    iterator new_finish = new_start;

    try 
    {
      new_finish = std::uninitialized_copy(start, position_, new_start);
      getAllocator().construct(new_finish, x_);
      ++new_finish;
      new_finish = std::uninitialized_copy(position_, finish, new_finish);
    }
    catch (...)
    {
      destroy(new_start, new_finish);
      for (; new_start < new_finish; ) getAllocator().destroy(++new_start);
      getAllocator().deallocate(new_start, new_size);
      throw;
    }

    destroy(start, finish);
    deallocate();

    start = new_start;
    finish = new_finish;
    cp_.first() = new_start + new_size;
  }
}

template <typename T, typename Alloc>
void Vector<T, Alloc>::insert(iterator position, size_type n, const T& x)
{
  if (n > 0)
  {
    if (size_type(cp_.first() - finish) >= n)
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
        std::uninitialized_fill_n(finish, n - elems_after_pos, x_copy);
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
      iterator new_start = getAllocator().allocate(len);
      iterator new_finish = new_start;
      try {
        new_finish = std::uninitialized_copy(start, position, new_start);
        new_finish = std::uninitialized_fill_n(new_finish, n, x);
        new_finish = std::uninitialized_copy(position, finish, new_finish);
      }
      catch (...)
      {
        destroy(new_start, new_finish);
        getAllocator().deallocate(new_start, len);
        throw;
      }
      
      // Change to new space
      destroy(start, finish);
      deallocate();
      start = new_start;
      finish = new_finish;
      cp_.first() = start + len;
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

// TODO reserve method
template <typename T, typename Alloc>
void Vector<T, Alloc>::reserve(size_type sz)
{
  // The space to reserve is smaller than current capacity
  // No need to expand
  if (sz < capacity()) return;

  iterator new_start = getAllocator().allocate(sz);
  iterator new_finish = new_start;
  try {
    new_finish = std::uninitialized_copy(start, finish, new_start);
  }
  catch (...)
  {
    destroy(new_start, new_finish);
    getAllocator().deallocate(new_start, sz);
    throw;
  }
  
  // Destroy & deallocate won't throw
  destroy(start, finish);
  deallocate();
  start = new_start;
  finish = new_finish;
  cp_.first() = start + sz;
}

template <typename T, typename Alloc>
Vector<T, Alloc>::Vector(const Vector<T, Alloc>& v)
{
  start = getAllocator().allocate(v.size());
  finish = start + v.size();
  cp_.first() = finish;
  std::uninitialized_copy(v.begin(), v.end(), start);
}

template <typename T, typename Alloc>
Vector<T, Alloc>& Vector<T, Alloc>::operator = (const Vector<T, Alloc>& rhs)
{
  if (this != &rhs)
  {
    const size_type len = rhs.size();
    iterator new_start = getAllocator().allocate(len);
    iterator new_finish = new_start;
    try {
      new_finish = std::uninitialized_copy(rhs.begin(), rhs.end(), new_start);
    }
    catch (...)
    {
      destroy(new_start, new_finish);
      getAllocator().deallocate(new_start, len);
      throw;
    }
    destroy(start, finish);
    deallocate();
    start = new_start;
    finish = new_finish;
    cp_.first() = start + len;
  }
  return *this;
}
