#include <string>

#include "stratego_io.h"

int main(int argc, char* argv[])
{
   bool headless = false;
   if ( argc > 1 )
   {
      std::string exe_option{ argv[1] };
      headless = exe_option.compare("headless") == 0;
   }

   if ( headless )
   {

   }
   else
   {

   }

   return 0;
}
