#pragma once

#include <cstdio>
#include <list>
#include <map>
#include <string>
#include <unordered_map>

// Which is better, map or unordered_map?
// typedef std::map<std::string, std::string> Dictionary;
typedef std::unordered_map<std::string, std::string> Dictionary;

class HttpRequest {
  public:
    HttpRequest() = default;
    long long length;
    std::string version;
    std::string method;
    std::string path;
    std::string body;
    Dictionary field;
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
    Dictionary field;
    std::string body;
    void send(FILE* fout) const;
    void print() const;
};

void http_service(FILE* fin, FILE* fout);
