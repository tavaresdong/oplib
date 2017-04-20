#include <net/TCPServer.h>
#include <net/EventLoop.h>
#include <net/InetAddress.h>
#include <cstdio>
#include <string>

std::string message;

void onConnection(const oplib::TCPConnectionPtr& conn_)
{
  if (conn_->connected())
  {
    printf("onConnection(): new connection [%s] from %s\n",
           conn_->name().c_str(),
           conn_->peerAddr().toHostPort().c_str());
    conn_->send(message);
  }
  else
  {
    printf("onConnection(): connection [%s] is down\n",
           conn_->name().c_str());
  }
}

void onWriteComplete(const oplib::TCPConnectionPtr& conn_)
{
  printf("Sending data complete\n");
}

void onMessage(const oplib::TCPConnectionPtr& conn_,
               oplib::ds::Buffer* buf_,
               oplib::Timestamp receiveTime_)
{
  printf("onMessage(): received %zd bytes from connection [%s] at %s\n",
         buf_->readableBytes(),
         conn_->name().c_str(),
         receiveTime_.toFormatString().c_str());

  buf_->retrieveAll();
}

int main()
{
  printf("main(): pid = %d\n", getpid());

  std::string line;
  for (int i = 33; i < 127; ++i)
  {
    line.push_back(char(i));
  }
  line += line;

  for (size_t i = 0; i < 127-33; ++i)
  {
    message += line.substr(i, 72) + '\n';
  }

  oplib::InetAddress listenAddr(9981);
  oplib::EventLoop loop;

  oplib::TCPServer server(&loop, listenAddr, "MyServer");
  server.setConnectionCallback(onConnection);
  server.setMessageCallback(onMessage);
  server.setWriteCompleteCallback(onWriteComplete);
  server.start();

  loop.loop();
}
