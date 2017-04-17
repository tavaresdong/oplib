#ifndef OPLIB_POLLER_H
#define OPLIB_POLLER_H

#include <util/Timestamp.h>
#include <util/Common.h>

#include <vector>
#include <map>

// This structure is defined in <poll.h>
// at global namespace
struct pollfd;

namespace oplib
{
  class EventDispatcher;
  class EventLoop;

  class Poller : Noncopyable
  {
   public: 
    using EventDispatcherList = std::vector<EventDispatcher*>;
    Poller(EventLoop* loop_);
    ~Poller();

    void updateEventDispatcher(EventDispatcher* dispatcher_);
    void removeEventDispatcher(EventDispatcher* dispatcher_);

    void inLoopThreadOrDie();

    Timestamp poll(int timeout_, EventDispatcherList *activeDispatchers_);

   private:
    void fillDispatchers(int activeEventCnt_, EventDispatcherList* list_) const;
    
    // Saved pollfds for polling 
    using PollfdList = std::vector<struct pollfd>;

    // fd -> EventDispatcher
    using EventDispatcherMap = std::map<int, EventDispatcher*>;

    EventLoop* _loop;
    PollfdList _pollfds;
    EventDispatcherMap _dispatchers;
  };
}

#endif
