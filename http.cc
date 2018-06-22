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
  s.clear();
  s.reserve(kDefaultStringLength);
  while(true) {
    s.push_back(ch);
    prev_char = ch;
    ch = fgetc(fin);
    if(prev_char == '\r' && ch == '\n') {
      s.pop_back();
      break;
    }
  }
  s.shrink_to_fit();
}

void HttpRequest::read_request_header_field(FILE* fin)
{
  int ch;
  std::string buf;
  std::string key;
  std::string value;

  while(true) {
    read_line(fin, buf);

    // determine the end of HTTP header
    if(buf.empty()) {
      break;
    }

    // init iterator
    auto it = buf.begin();

    // read name
    key.clear();
    for(; it != buf.end(); it++) {
      ch = *it;
      if(ch == ':') {
        // skip colon
        it++;
        break;
      }
      key.push_back(ch);
    }

    // skip space
    while(isspace(*it)) {
      it++;
    }

    // read value
    value.clear();
    for(; it != buf.end(); it++) {
      ch = *it;
      value.push_back(ch);
    }

    // add new a key-value pair
    field[key] = value;
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
  HttpResponse* res = new HttpResponse();

  // TODO: make a response
  res->version = "HTTP/1.1";
  res->status_code = 200;
  res->status_msg = "OK";
  res->print();

  res->send(fout);
  delete res;
  delete req;
}

static void print_str_member(const char* str, const std::string& member)
{
  fputs(str, flog);
  fputs(member.c_str(), flog);
  fputc('\n', flog);
}

static void print_dictionary(const Dictionary& dict)
{
  for(const auto& dpair: dict) {
    fprintf(flog, "%s: %s\n", dpair.first.c_str(), dpair.second.c_str());
  }
}

static void send_dictionary(const Dictionary& dict, FILE* fout)
{
  for(const auto& dpair: dict) {
    fprintf(fout, "%s: %s\r\n", dpair.first.c_str(), dpair.second.c_str());
  }
}

void HttpRequest::print() const
{
  print_str_member("method  = ", method);
  print_str_member("path    = ", path);
  print_str_member("version = ", version);
  print_dictionary(field);
  fputs("[body begin]\n", flog);
  fputs(body.c_str(), flog);
  fputs("[body end]\n", flog);
  fprintf(flog, "body_len = %lld\n", length);
}

void HttpResponse::send(FILE* fout) const
{
  fprintf(fout, "%s %d %s\r\n", version.c_str(), status_code, status_msg.c_str());
  send_dictionary(field, fout);
  fputs("\r\n", fout);
  fputs(body.c_str(), fout);
}

void HttpResponse::print() const
{
  fprintf(flog, "version = %s\n", version.c_str());
  fprintf(flog, "status_code = %d\n", status_code);
  fprintf(flog, "status_msg  = %s\n", status_msg.c_str());
  print_dictionary(field);
  fputs("[body begin]\n", flog);
  fputs(body.c_str(), flog);
  fputs("[body end]\n", flog);
}
