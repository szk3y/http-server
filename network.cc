#include "network.h"

#include <cstring>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "util.h"

#define MAX_BACKLOG 100

int accept_loop(int listen_fd)
{
  while(1) {
    struct sockaddr_storage addr;
    socklen_t addrlen = sizeof(addr);
    int sock = accept(listen_fd, (struct sockaddr*)&addr, &addrlen);
    if(sock < 0) {
      perror_exit("accept");
    }
    pid_t pid = fork();
    if(pid == 0) {
      return sock;
    }
  }
}

int sock_listen(const char* port)
{
  struct addrinfo hints;
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;
  struct addrinfo* res;
  int err = getaddrinfo(NULL, port, &hints, &res);
  if(err != 0) {
    fprintf_exit("getaddrinfo: %s\n", gai_strerror(err));
  }
  for(struct addrinfo* ai = res; ai; ai = ai->ai_next) {
    int sock = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
    if(sock < 0) {
      continue;
    }
    if(bind(sock, ai->ai_addr, ai->ai_addrlen) < 0) {
      close(sock);
      continue;
    }
    if(listen(sock, MAX_BACKLOG) < 0) {
      close(sock);
      continue;
    }
    // success
    freeaddrinfo(res);
    return sock;
  }
  fprintf_exit("sock_listen: all candidates failed");
}
