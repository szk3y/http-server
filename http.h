#pragma once

#include <cstdio>
#include <string>
#include <list>

class HttpHeaderField {
  public:
    HttpHeaderField() = default;
    std::string name;
    std::string value;
    void print() const;
};

class HttpRequest {
  public:
    HttpRequest() = default;
    long long length;
    std::string version;
    std::string method;
    std::string path;
    std::string body;
    std::list<HttpHeaderField> field;
    void print() const;
    void read_request(FILE* fin);
  private:
    void read_request_line(FILE* fin);
    void read_request_header_field(FILE* fin);
};

void http_service(FILE* fin, FILE* fout);
