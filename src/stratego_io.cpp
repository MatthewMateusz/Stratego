#include "stratego_io.h"

#ifdef WIN
#include <conio.h>
#else
#include<ncurses.h>
#endif

stratego_io::stratego_io()
{
   #ifdef WIN
   hOut = GetStdHandle(STD_OUTPUT_HANDLE);
   #else
   initscr();
   raw();
   noecho();
   cbreak();
   #endif
}

stratego_io::~stratego_io()
{
   #ifdef WIN
   // Right now nothing, but leave space here for future expandability
   #else
   endwin();
   #endif
}

const size_t RAW_BUFFER_SIZE = 256;

void stratego_io::print(const std::string& output)
{
   #ifdef WIN
   std::cout << output;
   #else
   addstr(output.c_str());
   refresh();
   #endif
}

int stratego_io::get_char()
{
   int input = 0;
   #ifdef WIN
   input = _getch();
   #else
   input = getch();
   if ( input == 27 )
   {
      input = getch();
      input = getch();
   }
   #endif
   return input;
}

void stratego_io::clear()
{
   #ifdef WIN
   system("cls");
   #else
   erase();
   #endif
}

void stratego_io::reset()
{
   #ifdef WIN
   COORD position;
   position.X = 0;
   position.Y = 0;

   SetConsoleCursorPosition(hOut, position);
   #else
   erase();
   #endif
}

void stratego_io::get_line(std::string& input)
{
   input.clear();
   #ifdef WIN
   std::getline(std::cin, input);
   #else
   echo();
   char raw_input[RAW_BUFFER_SIZE];
   getnstr(raw_input, RAW_BUFFER_SIZE);
   noecho();
   input = raw_input;
   #endif
}

void set_xy(size_t x, size_t y)
{
   #ifdef WIN
   COORD position;
   position.x = x;
   position.y = y;

   SetConsoleCursorPOsition(hOut, position);
   #else
   move(y, x); // NOTE: ncurses uses y first then x
   #endif
}
