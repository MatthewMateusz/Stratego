#include "MenuGenerator.h"

#include "ctype.h"
#ifdef __linux__
#include <unistd.h>
#include <ncurses.h>
#endif

#include "game_operations.h"
#include "io.h"

MenuGenerator::MenuGenerator(const std::string& subtitle_new)
{
   subtitle = subtitle_new;
}

MenuGenerator::~MenuGenerator()
{
   for ( std::unique_ptr<MenuItem>& menu_item : menu_items )
   {
      menu_item.reset();
   };
}

MenuError MenuGenerator::display()
{
   stratego_io::print(header);
   
   size_t subtitle_length = subtitle.size();
   size_t padding = (MAX_WIDTH - subtitle_length)/2;

   std::string menu{"\n"};
   add_space(menu, padding);
   menu.append(subtitle);
   add_space(menu, padding);
   menu.append("\n\n");

   size_t most_width = 0;
   for ( std::unique_ptr<MenuItem>& menu_item : menu_items )
   {
      if ( menu_item->width() > most_width )
      {
         most_width = menu_item->width();
      }
   }

   for ( std::unique_ptr<MenuItem>& menu_item : menu_items )
   {
      menu_item->print(menu, most_width);
   }

   stratego_io::print(menu);

   
   return MenuError::NONE;
};

MenuError MenuGenerator::add_item(const MenuItemConfig& config)
{

   switch( config.type )
   {
      case MenuType::button:
         menu_items.push_back(std::make_unique<MenuGenerator::MenuButton>(config.title, config.id, config.is_centered));
         break;

      case MenuType::label:
         menu_items.push_back(std::make_unique<MenuGenerator::MenuLabel>(config.title, config.is_centered));
         break;

      case MenuType::text:
         menu_items.push_back(std::make_unique<MenuGenerator::MenuText>(config.title, config.id, config.is_centered));
         break;

      default:
      case MenuType::none:
         return MenuError::INVALID_TYPE;
         break;
   } 

   return MenuError::NONE;
}

MenuError MenuGenerator::get_selected_value(MenuItemValue& value)
{
   for ( std::unique_ptr<MenuItem>& menu_item : menu_items )
   {
      if ( menu_item->selected() )
      {
         value.id = menu_item->get_id();
         if ( menu_item->is_editable() )
         {
            menu_item->get_entry(value.field);
         }
         return MenuError::NONE;
      } 
   }
   return MenuError::NOTHING_SELECTED;
}

MenuError MenuGenerator::get_value_by_id(MenuItemValue& value, size_t id)
{
   for ( std::unique_ptr<MenuItem>& menu_item : menu_items )
   {
      if ( menu_item->get_id() == id )
      {
         value.id = menu_item->get_id();
         if ( menu_item->is_editable() )
         {
            menu_item->get_entry(value.field);
         }
         return MenuError::NONE;
      }
   }

   return MenuError::INVALID_ID;
}

MenuError MenuGenerator::set_selected_by_id(size_t id)
{
   bool found = false;
   for ( std::unique_ptr<MenuItem>& menu_item : menu_items )
   {
      if ( menu_item->selected() )
      {
         menu_item->set_select(false);
      }

      if ( menu_item->get_id() == id )
      {
         menu_item->set_select(true);
         found = true;
      }
   }

   return found ? MenuError::NONE : MenuError::INVALID_ID;
}

MenuError MenuGenerator::move_selection(MenuDirection direction)
{
   bool found = false;

   switch ( direction )
   {
      case MenuDirection::up:
         for ( auto item = menu_items.rbegin(); item != menu_items.rend(); ++item )
         {
            if ( (*item)->selected() && item != menu_items.rend() - 1 )
            {
               found = true;
               (*item)->set_select(false);
               continue;
            }

            if ( found && (*item)->is_selectable() )
            {
               (*item)->set_select(true);
               break;
            }
         }
         return found ? MenuError::NONE : MenuError::SELECTION_CHANGE_FAIL;
         break;

      case MenuDirection::down:
         for ( auto item = menu_items.begin(); item != menu_items.end(); ++item )
         {
            if ( (*item)->selected() && item != menu_items.end() - 1 )
            {
               found = true;
               (*item)->set_select(false);
               continue;
            }

            if ( found && (*item)->is_selectable() )
            {
               (*item)->set_select(true);
               break;
            } 
         }
         return found ? MenuError::NONE : MenuError::SELECTION_CHANGE_FAIL;
         break;

      default:
         return MenuError::INVALID_DIRECTION;
         break;
   }
}

MenuError MenuGenerator::input(int input)
{
   switch ( input )
   {
      case UP:
         return move_selection(MenuDirection::up);
         break;

      case DOWN:
         return move_selection(MenuDirection::down);
         break;

      case ENTER:
         set_active(false);
         return MenuError::NONE;
         break;

      case 8:
      case 127:
         return pop_char();
         break;

      default:
         if ( iscntrl(input) == 0 )
         {
            return push_char(input);
         } 
         return MenuError::NON_ENTRY_CHAR;
         break;
   }
}

MenuError MenuGenerator::show()
{
   while ( is_active() )
   {
      stratego_io::clear();
      display();
      stratego_io::update();

      input(stratego_io::getchar());
   }

   return MenuError::NONE;
}

MenuError MenuGenerator::push_char(char c)
{
   for ( std::unique_ptr<MenuItem>& menu_item : menu_items )
   {
      if ( menu_item->selected() )
      {
         if ( menu_item->is_editable() )
         {
            return menu_item->push_char(c) ? MenuError::NONE : MenuError::ENTRY_FULL;
         }
         else
         {
            return MenuError::NOT_AN_ENTRY;
         }
         break;
      }
   }
   return MenuError::NOTHING_SELECTED;
}

MenuError MenuGenerator::pop_char()
{
   for ( std::unique_ptr<MenuItem>& menu_item : menu_items )
   {
      if ( menu_item->selected() )
      {
         if ( menu_item->is_editable() )
         {
            return menu_item->pop_char() ? MenuError::NONE : MenuError::ENTRY_EMPTY;
         }
         else
         {
            return MenuError::NOT_AN_ENTRY;
         }
         break;
      }
   }
   return MenuError::NOTHING_SELECTED;
}


MenuGenerator::MenuLabel::MenuLabel(const std::string& name, bool new_is_centered)
{
   title = name;
   id = 0;
   is_centered = new_is_centered; 
}

MenuError MenuGenerator::MenuLabel::get_value(std::string& output){ return MenuError::NO_VALUE; }

MenuType MenuGenerator::MenuLabel::get_type(){ return MenuType::label; }

void MenuGenerator::MenuLabel::print(std::string& output, size_t padding)
{
   if ( is_centered )
   {
      size_t length = title.size();
      size_t pad = (MAX_WIDTH - length)/2;
      add_space(output, pad);
      output.append(title);
      add_space(output, pad);
   }
   else
   {
      size_t pad = (MAX_WIDTH - padding)/2;
      size_t pad_rest = (padding - title.size());

      add_space(output, pad);
      output.append(title);
      add_space(output, pad_rest + pad);
   }
   output.push_back('\n');
}

size_t MenuGenerator::MenuLabel::width(){ return is_centered ? 0 : title.size(); }

void MenuGenerator::MenuLabel::set_select(bool select) { return; }


MenuGenerator::MenuButton::MenuButton(const std::string& new_name, size_t new_id, bool new_is_centered)
{
   title = new_name;
   id = new_id;
   is_centered = new_is_centered;
}

MenuError MenuGenerator::MenuButton::get_value(std::string& output){ return MenuError::NO_VALUE; }

MenuType MenuGenerator::MenuButton::get_type(){ return MenuType::button; }

size_t MenuGenerator::MenuButton::width(){ return is_centered ? 0 : title.size() + 3; }

void MenuGenerator::MenuButton::print(std::string& output, size_t padding)
{
   std::string prenum = is_selected ? " > " : "   ";

   size_t length = title.size() + 3;
   if ( is_centered )
   {
      size_t pad = (MAX_WIDTH - length)/2;

      add_space(output, pad);
      output.append(prenum);
      output.append(title);
      add_space(output, pad);
   }
   else
   {
      size_t pad = (MAX_WIDTH - padding)/2;
      size_t pad_rest = (padding - width());
   
      add_space(output, pad);
      output.append(prenum);
      output.append(title);
      add_space(output, pad_rest + pad);
   }
   output.push_back('\n');
}

void MenuGenerator::MenuButton::set_select(bool select)
{
   is_selected = select;
}


MenuGenerator::MenuText::MenuText(const std::string& new_name, size_t new_id, bool new_is_centered)
{
   title = new_name;
   id = new_id;
   is_centered = new_is_centered;
}

MenuError MenuGenerator::MenuText::get_value(std::string& output)
{
   output = text_entry;
   return MenuError::NONE;
}

size_t MenuGenerator::MenuText::width()
{
   return 3 + title.size() + max_length + 3;
}

void MenuGenerator::MenuText::print(std::string& output, size_t padding)
{
   std::string prenum = is_selected ? " > " : "   ";

   size_t length = width();
   if ( is_centered )
   {
      size_t pad = (MAX_WIDTH - length)/2;

      add_space(output, pad);
      output.append(prenum);
      output.append(title);
      output.append(":[");
      output.append(text_entry);
      add_space(output, 32 - text_entry.size());
      output.append("]");
      add_space(output, pad);
   }
   else
   {
      size_t pad = (MAX_WIDTH - padding)/2;
      size_t pad_rest = (padding - width())/2;

      add_space(output, pad);
      output.append(prenum);
      output.append(title);
      output.append(":[");
      output.append(text_entry);
      add_space(output, 32 - text_entry.size());
      output.append("]");
      add_space(output, pad + pad_rest);
      
      add_space(output, pad);
   }
   output.push_back('\n');
}

void MenuGenerator::MenuText::set_select(bool select)
{
   is_selected = select;
}

bool MenuGenerator::MenuText::push_char(char c)
{
   if ( text_entry.size() < max_length )
   {
      text_entry.push_back(c);
      return true;
   }
   else
   {
      return false;
   }
}

bool MenuGenerator::MenuText::pop_char()
{
   if ( text_entry.size() > 0 )
   {
      text_entry.pop_back();
      return true;
   }
   else
   {
      return false;
   }
}
