#include "http.h"

#include <cassert>
#include <cstdio>
#include <string>

#include "util.h"

extern FILE* flog;

static const int kDefaultStringLength = 0x100;

void HttpRequest::parse_request_line(std::string& request_line)
{
  // read method
  assert(method.empty());
  method.reserve(kDefaultStringLength);
  auto it = request_line.begin();
  for(; it != request_line.end(); it++) {
    char ch = *it;
    if(ch == ' ') {
      break;
    }
    method.push_back(ch);
  }
  method.shrink_to_fit();

  // read path
  assert(path.empty());
  path.reserve(kDefaultStringLength);
  for(it++; it != request_line.end(); it++) {
    char ch = *it;
    if(ch == ' ') {
      break;
    }
    path.push_back(ch);
  }
  path.shrink_to_fit();

  // read version
  assert(version.empty());
  version.reserve(kDefaultStringLength);
  bool prev_is_cr = false;
  for(it++; it != request_line.end(); it++) {
    char ch = *it;
    if(ch == '\r') {
      prev_is_cr = true;
      continue;
    }
    if(prev_is_cr) {
      if(ch == '\n') {
        break;
      } else {
        version.push_back('\r');
      }
    }
    version.push_back(ch);
    prev_is_cr = false;
  }
}

void HttpRequest::read_request_line(FILE* fin)
{
  std::string request_line;
  read_line(request_line, fin);
  this->parse_request_line(request_line);
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
