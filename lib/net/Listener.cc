#include "Listener.h"
#include "InetAddress.h"
#include "SocketUtils.h"
#include "EventLoop.h"

namespace oplib
{
  Listener::Listener(EventLoop* loop_, const InetAddress& listenAddr_)
  : _loop(loop_), _listening(false), _listenSock(socketutils::createOrDie()),
    _dispatcher(_loop, _listenSock.fd())
  {
    // Reuse the address when closed
    _listenSock.setReuseAddr(true);

    // Bind the socket to listenAddr_
    _listenSock.bindAddress(listenAddr_);

    // ListenSocket fd is readable, call handleRead
    _dispatcher.setReadCallback(std::bind(&Listener::handleRead, this));
  }

  void Listener::listen()
  {
    // Manipulate EventLoop data structure(loop->poller->pollfds)
    _loop->inLoopThreadOrDie();
    _listening = true;
    _listenSock.listen();
    _dispatcher.enableReading();   
  }

  void Listener::handleRead()
  {
    _loop->inLoopThreadOrDie();
    InetAddress peer(0);

    // Make use of Socket's destructor to guarantee resource release (RAII)
    auto peerSock = std::make_unique<Socket>(_listenSock.accept(&peer));
    if (peerSock->fd() >= 0)
    {
      if (_newConnectionCb)
      {
        _newConnectionCb(std::move(peerSock), peer);
      }
    }
    // As the connfd is managed by unique_ptr, we don't need 
    // to release it manually
  }
}
