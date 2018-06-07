#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "util.h"

FILE* flog;

typedef void (*sighandler_t)(int);

void wait_child(int sig)
{
  UNUSED(sig);
  int status;
  pid_t child = wait(&status);
  printf("%d: ", child);
  if(WIFEXITED(status)) {
    printf("exit, status=%d\n", status);
  } else if(WIFSIGNALED(status)) {
    printf("signal, sig=%d\n", WTERMSIG(status));
  } else {
    printf("abnormal exit\n");
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
  fin  = stdin;
  fout = stdout;
  flog = stderr;
  init_sighandler();
  //http_service(fin, fout);
  fclose(fin);
  fclose(fout);
  return 0;
}
