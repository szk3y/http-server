#include "http.h"

#include <cassert>
#include <cstdio>
#include <string>

#include "util.h"

extern FILE* flog;

static const int kDefaultStringLength = 0x100;

void HttpRequest::read_request_line(FILE* fin)
{
  char ch, prev_char;

  // read method
  assert(method.empty());
  method.reserve(kDefaultStringLength);
  while((ch = fgetc(fin)) != ' ') {
    method.push_back(ch);
  }
  method.shrink_to_fit();

  // read path
  assert(path.empty());
  path.reserve(kDefaultStringLength);
  while((ch = fgetc(fin)) != ' ') {
    path.push_back(ch);
  }
  path.shrink_to_fit();

  // read version
  assert(version.empty());
  version.reserve(kDefaultStringLength);
  while(true) {
    prev_char = ch;
    ch = fgetc(fin);
    if(prev_char == '\r' && ch == '\n') {
      version.pop_back(); // remove '\r'
      break;
    }
    version.push_back(ch);
  }
  version.shrink_to_fit();
}

void HttpRequest::read_request_header_field(FILE* fin)
{
}

void HttpRequest::read_request(FILE* fin)
{
  this->read_request_line(fin);
}

void http_service(FILE* fin, FILE* fout)
{
  HttpRequest* req = new HttpRequest();
  req->read_request(fin);
  req->print();
  UNUSED(fout);
  delete req;
}

static void print_str_member(const char* str, const std::string& member)
{
  fputs(str, flog);
  fputs(member.c_str(), flog);
  putchar('\n');
}

void HttpRequest::print()
{
  print_str_member("method  = ", method);
  print_str_member("path    = ", path);
  print_str_member("version = ", version);
  fputs("[body begin]", flog);
  fputs(body.c_str(), flog);
  fputs("[body end]", flog);
  fprintf(flog, "body_len = %lld\n", length);
}
