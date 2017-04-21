#ifndef OPLIB_NET_CONNECTOR_H
#define OPLIB_NET_CONNECTOR_H

#include "EventLoop.h"
#include "InetAddress.h"
#include "EventDispatcher.h"
#include "TimerManager.h"

#include <functional>
#include <memory>

#include <util/Common.h>

namespace oplib
{
namespace net
{
  class Connector : Noncopyable
  {
    public:
     using NewConnectionCallback = std::function<void (int)>;
    
     Connector(EventLoop* loop_, const InetAddress& serverAddr_);
     ~Connector();

     void setNewConnectionCallback(const NewConnectionCallback& cb_)
     { _newConnectionCallback = cb_; }

     void start();
     void restart();
     void stop();


    private:

     enum class State {DISCONNECTED , CONNECTING, CONNECTED};

     void setState(State s_) 
     {
       switch (s_)
       {
           case State::DISCONNECTED:
          printf("SetState to DISCONNECTED\n");
          break;
           case State::CONNECTING:
          printf("SetState to CONNECTING\n");
          break;
           case State::CONNECTED:
          printf("SetState to CONNECTED\n");
          break;
       }
       _state = s_;
     }

     void startInLoop();
     void connect();
     void connecting(int sockfd_);

     void handleWrite();
     void handleError();
     void retry(int sockfd);
     int removeAndResetDispatcher();
     void resetDispatcher();

     static const int initRetryMs;
     static const int maxRetryMs;

     EventLoop* _loop;
     InetAddress _serverAddr;
     bool _connect;
     State _state;
     std::unique_ptr<EventDispatcher> _dispatcher;
     NewConnectionCallback _newConnectionCallback;
     int _retryDelay;

     TimerId _timerId;
  };

  typedef std::shared_ptr<Connector> ConnectorPtr;
}
}

#endif
