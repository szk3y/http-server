#include "util.h"

#include <cassert>
#include <cerrno>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

static const size_t kLineLength = 0x100;

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

// return result of last fgetc
int read_line(std::string& s, FILE* fp)
{
  assert(s.empty());
  s.reserve(kLineLength);
  int ch = fgetc(fp);
  int prevch;
  while(ch != EOF) {
    s.push_back(ch);
    prevch = ch;
    ch = fgetc(fp);
    if(prevch == '\r' && ch == '\n') { // check end of line
      s.push_back(ch);
      break;
    }
  }
  s.shrink_to_fit();
  return ch;
}
