#pragma once

#include <map>
#include <string>
#include <unordered_map>

#include "http.h" // Dictionary

#define UNUSED(x) (void)(x)

// Which is better, map or unordered_map?
// typedef std::map<std::string, std::string> Dictionary;
typedef std::unordered_map<std::string, std::string> Dictionary;

void fprintf_exit(const char* fmt, ...);
void perror_exit(const char* s);
Dictionary::const_iterator find_field(const Dictionary& dict, const std::string& s);
