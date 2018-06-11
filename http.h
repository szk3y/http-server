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
    void send(FILE* fout) const;
};

class HttpRequest {
  public:
    HttpRequest() = default;
    long long length;
    std::string version;
    std::string method;
    std::string path;
    std::string body;
    // TODO: use hash map
    std::list<HttpHeaderField> field;
    void print() const;
    void read_request(FILE* fin);
  private:
    void read_request_line(FILE* fin);
    void read_request_header_field(FILE* fin);
};

class HttpResponse {
  public:
    HttpResponse() = default;
    long long length;
    std::string version;
    int status_code;
    std::string status_msg;
    // TODO: use hash map
    std::list<HttpHeaderField> field;
    std::string body;
    void send(FILE* fout) const;
    void print() const;
};

void http_service(FILE* fin, FILE* fout);
