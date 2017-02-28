#include "Poller.h"
#include "EventDispatcher.h"
#include "EventLoop.h"

#include <poll.h>
#include <stdlib.h>


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

  Timestamp Poller::poll(int timeout_ /*in ms*/, EventDispatcherList *activeDispatchers_)
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
}