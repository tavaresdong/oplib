#include <net/TCPServer.h>
#include <net/EventLoop.h>
#include <net/InetAddress.h>

#include <cstdio>

void onConnection(const oplib::TCPConnectionPtr& conn_)
{
  if (conn_->connected())
  {
    printf("onConnection(): new connection [%s] from %s\n",
           conn_->name().c_str(),
           conn_->peerAddr().toHostPort().c_str());
  }
  else
  {
    printf("onConnection(): connection [%s] is down\n",
           conn_->name().c_str());
  }
}

// Echo function
void onMessage(const oplib::TCPConnectionPtr& conn_,
               oplib::ds::Buffer* buf_,
               oplib::Timestamp receiveTime_)
{
  printf("onMessage(): received %zd bytes from connection [%s] at %s\n",
         buf_->readableBytes(),
         conn_->name().c_str(),
         receiveTime_.toFormatString().c_str());

  conn_->send(buf_->retrieveAsString());
}

int main()
{
  printf("main(): pid = %d\n", getpid());

  oplib::InetAddress listenAddr(9981);
  oplib::EventLoop loop;

  oplib::TCPServer server(&loop, listenAddr, "MyServer");
  server.setConnectionCallback(onConnection);
  server.setMessageCallback(onMessage);
  server.start();

  loop.loop();
}
