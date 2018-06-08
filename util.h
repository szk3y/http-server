#pragma once

#include <string>

#define UNUSED(x) (void)(x)

void fprintf_exit(const char* fmt, ...);
void perror_exit(const char* s);
int read_line(std::string& s, FILE* fp);
