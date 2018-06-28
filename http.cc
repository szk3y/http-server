#include "http.h"

#include <cassert>
#include <cstdio>
#include <cstring>
#include <ctype.h>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "util.h"

extern FILE* flog;
extern const char* docroot;

static const int kDefaultStringLength = 0x100;
static const int kMaxContentLength = 0x1000000;

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

void HttpRequest::read_request_body(FILE* fin)
{
  auto it = find_field(field, "Content-length");
  int content_length = std::stoi(it->second);
  if(content_length < 0) {
    fprintf_exit("read_request_body: negative content length found\n");
  }
  if(kMaxContentLength < content_length) {
    fprintf_exit("read_request_body: content length is too long\n");
  }
  assert(body.empty());
  body.resize(content_length);
  fread(&body[0], 1, body.size(), fin);
}

bool HttpRequest::path_has_dot_dot() const
{
  return strstr(path.c_str(), "..") == NULL;
}

bool HttpRequest::method_has_request_body() const
{
  return method.compare("HEAD") != 0;
}

bool HttpRequest::method_is_implemented() const
{
  return method.compare("HEAD");
}

void HttpRequest::read_request(FILE* fin)
{
  this->read_request_line(fin);
  this->read_request_header_field(fin);
  if(this->method_has_request_body()) {
    this->read_request_body(fin);
  }
}

void HttpResponse::set_status(StatusCode code)
{
  switch(code) {
    case Ok:
      status_code = Ok;
      status_msg = "OK";
      break;
    case BadRequest:
      status_code = BadRequest;
      status_msg = "Bad Request";
      break;
    case NotFound:
      status_code = NotFound;
      status_msg = "Not Found";
      break;
    default:
      fprintf_exit("HttpResponse::set_status: Unknown status found '%d'\n", code);
  }
}

void HttpResponse::bad_request()
{
  this->set_status(BadRequest);
}

static void build_response_to_head(HttpResponse& res, const HttpRequest& req)
{
  UNUSED(req);
  res.set_status(Ok);
}

static void response_not_found(HttpResponse& res)
{
  res.set_status(NotFound);
}

// TODO: set other header fields
static void build_response_to_get(HttpResponse& res, const HttpRequest& req)
{
  FILE* fp;
  std::string path;
  struct stat st;

  if(req.path_has_dot_dot()) {
    res.bad_request();
    return;
  }
  // set path
  if(req.path.compare("/")) {
    path = docroot + std::string("index.html");
  } else {
    path = docroot + req.path;
  }

  // determine whether file exists or not and open it
  if(access(path.c_str(), F_OK) == -1) {
    response_not_found(res);
    return;
  }
  fp = fopen(path.c_str(), "rb");
  if(fp == NULL) {
    perror_exit("fopen in build_response_to_get");
  }

  // read file
  fstat(fileno(fp), &st);
  assert(res.body.empty());
  res.body.resize(st.st_size);
  fread(&res.body[0], 1, res.body.size(), fp);

  fclose(fp);
}

static void build_response(HttpResponse& res, const HttpRequest& req)
{
  res.version = "HTTP/1.1";
  if(req.method.compare("HEAD") == 0) {
    build_response_to_head(res, req);
  } else if(req.method.compare("GET") == 0) {
    build_response_to_get(res, req);
  } else {
    fprintf_exit("Unknown method '%s'\n", req.method.c_str());
  }
}

void http_service(FILE* fin, FILE* fout)
{
  HttpRequest* req = new HttpRequest();
  req->read_request(fin);
  req->print();
  HttpResponse* res = new HttpResponse();

  build_response(*res, *req);
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
