#include <net/EventLoop.h>
#include <net/Connector.h>

#include <cstdio>

oplib::EventLoop* gloop;

void connectionCallback(int sockfd_)
{
  printf("Connected.\n");
  gloop->quit();
}

int main(int argc, char** argv)
{
  oplib::EventLoop loop;
  gloop = &loop;

  oplib::InetAddress serverAddr("127.0.0.1", 9981);
  oplib::net::ConnectorPtr connector(new oplib::net::Connector(gloop, serverAddr));
  connector->setNewConnectionCallback(connectionCallback);
  connector->start();

  loop.loop();
}
