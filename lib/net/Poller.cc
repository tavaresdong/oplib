#include "Poller.h"
#include "EventDispatcher.h"
#include "EventLoop.h"

#include <poll.h>
#include <stdlib.h>
#include <algorithm>
#include <assert.h>

namespace oplib
{
  Poller::Poller(EventLoop* loop_)
  : _loop(loop_)
  {} 

  Poller::~Poller() 
  {}

  void Poller::inLoopThreadOrDie()
  {
    _loop->inLoopThreadOrDie();
  }

  Timestamp Poller::poll(int timeout_, EventDispatcherList *activeDispatchers_)
  {
    // can only be called from loop thread
    inLoopThreadOrDie();
    
    struct pollfd* begin = _pollfds.data();
    int numEvents = ::poll(begin, _pollfds.size(), timeout_);
    Timestamp pollReturnTime { Timestamp::now() };
    if (numEvents > 0)
    {
      fillDispatchers(numEvents, activeDispatchers_);
    }
    else if (numEvents < 0)
    {
      // Error Happened, TODO: use error log instead
      abort();
    }
    return pollReturnTime;
  }

  void Poller::fillDispatchers(int activeEventCnt_, EventDispatcherList* list_) const
  {
    // We cannot handleEvent here, because that
    // may insert into the _pollfds while we iterate
    for (auto iter = _pollfds.begin();
         iter != _pollfds.end() && activeEventCnt_ > 0;
         ++iter)
    {
      if (iter->revents != 0)
      {
        auto dpiter = _dispatchers.find(iter->fd); 
        dpiter->second->setRevents(iter->revents);
        list_->push_back(dpiter->second);
        --activeEventCnt_;
      }
    }
  }

  void Poller::updateEventDispatcher(EventDispatcher* dispatcher_)
  {
    // Update the eventDispatcher in _dispatchers
    // especially the events they are interested in
    if (dispatcher_->index() < 0)
    {
      // A new dispatcher
      struct pollfd pfd;
      pfd.fd = dispatcher_->fd();
      pfd.events = dispatcher_->events();
      pfd.revents = dispatcher_->revents();
      _pollfds.push_back(pfd);
      dispatcher_->setIndex(static_cast<int>(_pollfds.size()) - 1);
      _dispatchers[pfd.fd] = dispatcher_;
    }
    else
    {
      int index = dispatcher_->index();
      auto& pfd = _pollfds[index];
      pfd.events = static_cast<short>(dispatcher_->events());
      pfd.revents = 0;
      if (dispatcher_->isIgnored())
      {
        pfd.events = -static_cast<short>(dispatcher_->events()) - 1;
      }
    }
  }

  void Poller::removeEventDispatcher(EventDispatcher* dispatcher_)
  {
    // Update the eventDispatcher in _dispatchers
    // especially the events they are interested in
    if (dispatcher_->index() < 0)
    {
      // TODO log error
      //
      abort();
    }
    else
    {
      assert(_dispatchers.find(dispatcher_->fd()) != _dispatchers.end());
      assert(_dispatchers[dispatcher_->fd()] == dispatcher_);
      int index = dispatcher_->index();
      auto n = _dispatchers.erase(dispatcher_->fd());
      assert(n == 1);
      UNUSED(n);
      if (implicit_cast<size_t>(index) == _pollfds.size() - 1)
      {
        _pollfds.pop_back();
      }
      else
      {
        int endDispatcherFd = _pollfds.back().fd;
        std::iter_swap(_pollfds.begin() + index, _pollfds.end() - 1);
        if (endDispatcherFd < 0)
        {
          endDispatcherFd = -endDispatcherFd - 1;
        }
        _dispatchers[endDispatcherFd]->setIndex(index);
        _pollfds.pop_back();
      }
    }
  }
}
