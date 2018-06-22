#pragma once

#include <string>

#include "http.h" // Dictionary

#define UNUSED(x) (void)(x)

void fprintf_exit(const char* fmt, ...);
void perror_exit(const char* s);
Dictionary::const_iterator find_field(const Dictionary& dict, const std::string& s);
