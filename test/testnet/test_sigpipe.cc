#include <net/TCPServer.h>
#include <net/EventLoop.h>
#include <net/InetAddress.h>
#include <cstdio>
#include <string>

#include <unistd.h>

std::string message1;
std::string message2;

void onConnection(const oplib::TCPConnectionPtr& conn_)
{
  if (conn_->connected())
  {
    printf("onConnection(): new conn_ection [%s] from %s\n",
           conn_->name().c_str(),
           conn_->peerAddr().toHostPort().c_str());
    ::sleep(5);

    // If peer is down, writing to peer will cause sigpipe
    // the default handling for sigpipe is to kill the process
    // The network library need to ignore the sigpipe signal
    conn_->send(message1);
    conn_->send(message2);
    conn_->shutdown();
  }
  else
  {
    printf("onConnection(): conn_ection [%s] is down\n",
           conn_->name().c_str());
  }
}

void onMessage(const oplib::TCPConnectionPtr& conn_,
               oplib::ds::Buffer* buf_,
               oplib::Timestamp receiveTime_)
{
  printf("onMessage(): received %zd bytes from conn_ection [%s] at %s\n",
         buf_->readableBytes(),
         conn_->name().c_str(),
         receiveTime_.toFormatString().c_str());

  buf_->retrieveAll();
}

int main(int argc, char* argv[])
{
  printf("main(): pid = %d\n", getpid());

  int len1 = 100;
  int len2 = 200;

  if (argc > 2)
  {
    len1 = atoi(argv[1]);
    len2 = atoi(argv[2]);
  }

  message1.resize(len1);
  message2.resize(len2);
  std::fill(message1.begin(), message1.end(), 'A');
  std::fill(message2.begin(), message2.end(), 'B');

  oplib::InetAddress listenAddr(9981);
  oplib::EventLoop loop;

  oplib::TCPServer server(&loop, listenAddr, "MyServer");
  server.setConnectionCallback(onConnection);
  server.setMessageCallback(onMessage);
  server.start();

  loop.loop();
}

