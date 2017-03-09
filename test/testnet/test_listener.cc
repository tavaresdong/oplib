// Transplated from muduo's testcases
#include <net/Listener.h>
#include <net/EventLoop.h>
#include <net/InetAddress.h>
#include <net/Socket.h>
#include <net/SocketUtils.h>
#include <stdio.h>
#include <memory>

void newConnection(std::unique_ptr<oplib::Socket> sock, const oplib::InetAddress& peerAddr)
{
  printf("newConnection(): accepted a new connection from %s\n",
         peerAddr.toHostPort().c_str());
  ::write(sock->fd(), "How are you?\n", 13);
}

void newConnection2(std::unique_ptr<oplib::Socket> sock, const oplib::InetAddress& peerAddr)
{
  printf("newConnection(): accepted a new connection from %s\n",
         peerAddr.toHostPort().c_str());
  ::write(sock->fd(), "How old are you?\n", 17);
}

int main()
{
  printf("main(): pid = %d\n", getpid());

  oplib::InetAddress listenAddr(9981);
  oplib::InetAddress listenAddr2(9982);
  oplib::EventLoop loop;

  oplib::Listener listener(&loop, listenAddr);
  oplib::Listener listener2(&loop, listenAddr2);
  listener.setNewConnectionCallback(newConnection);
  listener2.setNewConnectionCallback(newConnection2);
  listener.listen();
  listener2.listen();

  loop.loop();
}
