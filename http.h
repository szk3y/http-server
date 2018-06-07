#pragma once

#include <string>
#include <list>

class HttpHeaderField {
  public:
    HttpHeaderField();
    std::string name;
    std::string value;
};

class HttpRequest {
  public:
    HttpRequest();
    long long length;
    std::string version;
    std::string method;
    std::string path;
    std::string body;
    std::list<HttpHeaderField> field;
    void print();
};
