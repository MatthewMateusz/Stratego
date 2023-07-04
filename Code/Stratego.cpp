// StrategoVs2.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "game_operations.h"
#ifdef __linux__
#include <unistd.h>
#include <ncurses.h>
#endif

#include "MenuGenerator.h"

int main()
{
#ifdef __linux__
    initscr();
    raw();
    noecho();
    cbreak();
#endif

   MenuGenerator main_menu{"MAIN MENU"};
   size_t config_id = 0;

   MenuItemConfig new_game_button{};
   new_game_button.type = MenuType::button;
   new_game_button.title = "New Game";
   new_game_button.is_centered = false;
   new_game_button.id = ++config_id;

   MenuItemConfig load_game_button{};
   load_game_button.type = MenuType::button;
   load_game_button.title = "Load Game";
   load_game_button.is_centered = false;
   load_game_button.id = ++config_id;

   MenuItemConfig exit_button{};
   exit_button.type = MenuType::button;
   exit_button.title = "Exit";
   exit_button.is_centered = false;
   exit_button.id = ++config_id;

   main_menu.add_item(new_game_button);
   main_menu.add_item(load_game_button);
   main_menu.add_item(exit_button);

   main_menu.set_selected_by_id(new_game_button.id);

   main_menu.show();

   MenuItemValue selected_option{};
   main_menu.get_selected_value(selected_option);

   stratego_game_operations game_manager;
   game_manager.initialize_file_system();
 
   size_t sel_id = selected_option.id;
   if ( sel_id == new_game_button.id )
   {
      game_manager.setup();
   }
   else if ( sel_id == load_game_button.id )
   {
      game_manager.load();
   }
   else
   {
      return 0;
   }
      
   game_manager.game_loop();
   game_manager.end_game();
 
#ifdef __linux__
    endwin();
#endif
}
