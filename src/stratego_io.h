#ifndef IO_H
#define IO_H

#include <string>

#ifdef WIN
#include <windows.h>
#else

#endif

//
// Reimplementation of original stratego_io using RAII concepts to manage C objects
//
class stratego_io
{
#ifdef WIN
   HANDLE hOut;
#endif

public:
   stratego_io();
   ~stratego_io();

   void print(const std::string& output);
   int get_char();
   void clear();
   void reset();
   void get_line(std::string& input);
   void set_xy(size_t x, size_t y);
};

#endif
