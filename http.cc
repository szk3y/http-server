#include "http.h"

#include <cassert>
#include <cstdio>
#include <cstring>
#include <ctype.h>
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

static void read_line(FILE* fin, std::string& s)
{
  int ch = fgetc(fin);
  int prev_char;
  while(true) {
    s.push_back(ch);
    prev_char = ch;
    ch = fgetc(fin);
    if(prev_char == '\r' && ch == '\n') {
      s.pop_back();
      break;
    }
  }
}

void HttpRequest::read_request_header_field(FILE* fin)
{
  int ch;
  std::string buf;

  while(true) {
    buf.clear();
    read_line(fin, buf);

    // end of HTTP header
    if(buf.empty()) {
      break;
    }

    // new header field
    field.push_back(HttpHeaderField());

    // init iterator
    auto it = buf.begin();

    // read name
    for(; it != buf.end(); it++) {
      ch = *it;
      if(ch == ':') {
        // skip colon
        it++;
        break;
      }
      field.back().name.push_back(ch);
    }

    // skip space
    while(isspace(*it)) {
      it++;
    }

    // read value
    for(; it != buf.end(); it++) {
      ch = *it;
      field.back().value.push_back(ch);
    }
  }
}

void HttpRequest::read_request(FILE* fin)
{
  this->read_request_line(fin);
  this->read_request_header_field(fin);
}

void http_service(FILE* fin, FILE* fout)
{
  HttpRequest* req = new HttpRequest();
  req->read_request(fin);
  req->print();
  UNUSED(fout);
  delete req;
}

void HttpHeaderField::print() const
{
  fputs(name.c_str(), flog);
  fputs(": ", flog);
  fputs(value.c_str(), flog);
  fputc('\n', flog);
}

static void print_str_member(const char* str, const std::string& member)
{
  fputs(str, flog);
  fputs(member.c_str(), flog);
  fputc('\n', flog);
}

void HttpRequest::print() const
{
  print_str_member("method  = ", method);
  print_str_member("path    = ", path);
  print_str_member("version = ", version);
  // print field
  for(const auto x : field) {
    x.print();
  }
  fputs("[body begin]\n", flog);
  fputs(body.c_str(), flog);
  fputs("[body end]\n", flog);
  fprintf(flog, "body_len = %lld\n", length);
}
