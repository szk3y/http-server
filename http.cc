#include "http.h"

#include <cstdio>
#include <string>

HttpHeaderField::HttpHeaderField()
{
}

HttpRequest::HttpRequest()
{
}

static void print_str_member(const char* str, const std::string& member)
{
  fputs(str, stdout);
  fputs(member.c_str(), stdout);
  putchar('\n');
}

void HttpRequest::print()
{
  print_str_member("method = ", method);
  print_str_member("path = ", path);
  print_str_member("version = ", version);
  print_str_member("[body begin]\n", body);
  puts("[body end]\n");
  printf("body_len = %lld\n", length);
}
