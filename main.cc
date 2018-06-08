#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "http.h"
#include "network.h"
#include "util.h"

// file pointer to output log
FILE* flog = stdout;

typedef void (*sighandler_t)(int);

void wait_child(int sig)
{
  UNUSED(sig);
  int status;
  pid_t child = wait(&status);
  printf("%d: ", child);
  if(WIFEXITED(status)) {
    fprintf(flog, "exit, status=%d\n", status);
  } else if(WIFSIGNALED(status)) {
    fprintf(flog, "signal, sig=%d\n", WTERMSIG(status));
  } else {
    fprintf(flog, "abnormal exit\n");
  }
}

void signal_exit(int sig)
{
  fprintf_exit("exit by signal(%d)\n", sig);
}

void set_sighandler(int sig, sighandler_t sighandler)
{
  struct sigaction sa;
  sa.sa_handler = sighandler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;
  sigaction(sig, &sa, NULL);
}

void init_sighandler()
{
  set_sighandler(SIGCHLD, wait_child);
  set_sighandler(SIGPIPE, signal_exit);
}

int main()
{
  FILE *fin, *fout;
  int listen_fd, accept_fd;
  init_sighandler();
  listen_fd = sock_listen("8080");
  accept_fd = accept_loop(listen_fd);

  fin  = fdopen(dup(accept_fd), "rb");
  fout = fdopen(dup(accept_fd), "wb");
  http_service(fin, fout);
  fclose(fin);
  fclose(fout);
  close(accept_fd);
  return 0;
}
