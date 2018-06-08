#include "util.h"

#include <cassert>
#include <cerrno>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

extern FILE* flog;

void fprintf_exit(const char* fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  vfprintf(flog, fmt, ap);
  va_end(ap);
  exit(1);
}

void perror_exit(const char* s)
{
  fprintf(flog, "%s: %s", s, strerror(errno));
  exit(1);
}
