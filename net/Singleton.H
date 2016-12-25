/*
 * Singleton.H
 *
 *  Created on: Dec 25, 2016
 *      Author: yuchend
 */

#ifndef OPLIB_NET_SINGLETON_H_
#define OPLIB_NET_SINGLETON_H_

#include <pthread.h>

namespace oplib
{
  template <typename T>
  class Singleton
  {
  public:
    Singleton() = delete;
    ~Singleton() = delete;
    Singleton(const Singleton& rhs_) = delete;
    Singleton& operator=(const Singleton& rhs_) = delete;

    static T* instance()
    {
      // Only execute once by one thread
      pthread_once(&_once, init);
      return _instance;
    }
  private:
    static void init(void)
    {
      _instance = new T();
    }

    static T* _instance;
    static pthread_once_t _once;
  };

  template <typename T>
  T* Singleton<T>::_instance = nullptr;

  template <typename T>
  pthread_once_t Singleton<T>::_once = PTHREAD_ONCE_INIT;

}

#endif /* NET_SINGLETON_H_ */
