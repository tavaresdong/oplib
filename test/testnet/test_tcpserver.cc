// Transplanted from muduo example
#include <net/TCPServer.h>
#include <net/EventLoop.h>
#include <net/InetAddress.h>
#include <stdio.h>

void onConnection(const oplib::TCPConnectionPtr& conn)
{
  if (conn->connected())
  {
    printf("onConnection(): new connection [%s] from %s\n",
           conn->name().c_str(),
           conn->peerAddr().toHostPort().c_str());
  }
  else
  {
    printf("onConnection(): connection [%s] is down\n",
           conn->name().c_str());
  }
}

void onMessage(const oplib::TCPConnectionPtr& conn,
               const char* data,
               ssize_t len)
{
  printf("onMessage(): received %zd bytes from connection [%s]\n",
         len, conn->name().c_str());
}

int main()
{
  printf("main(): pid = %d\n", getpid());

  oplib::InetAddress listenAddr(9981);
  oplib::EventLoop loop;

  oplib::TCPServer server(&loop, listenAddr, "yuchenServer");
  server.setConnectionCallback(onConnection);
  server.setMessageCallback(onMessage);
  server.start();

  loop.loop();
}
