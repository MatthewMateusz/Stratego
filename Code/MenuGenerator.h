#ifndef MENU_GENERATOR_H
#define MENU_GENERATOR_H

#include <string>
#include <memory>
#include <vector>

enum class MenuDirection
{
   up,
   down,
   none
};

enum class MenuType
{
   button,
   label,
   text,
   none
};

enum class MenuError
{
   NONE,
   GENERIC_ERROR,
   INVALID_TYPE,
   INVALID_ID,
   INVALID_DIRECTION,
   SELECTION_CHANGE_FAIL,
   NOTHING_SELECTED,
   ENTRY_FULL,
   ENTRY_EMPTY,
   NOT_AN_ENTRY,
   NO_VALUE,
   NON_ENTRY_CHAR
};

struct MenuItemConfig
{
   MenuType type{MenuType::none};
   std::string title{};
   bool is_centered{true};
   size_t id{0};
};

struct MenuItemValue
{
   std::string field{}; 
   size_t id{0};
};

class MenuGenerator
{
public:
   MenuGenerator(const std::string& subtitle_new);
   ~MenuGenerator();

   MenuError add_item(const MenuItemConfig& config);
   MenuError get_selected_value(MenuItemValue& value);
   MenuError get_value_by_id(MenuItemValue& value, size_t id);

   MenuError display(); 

   MenuError set_selected_by_id(size_t id);

   MenuError move_selection(MenuDirection direction);

   MenuError input(int input);

   MenuError push_char(char c);
   MenuError pop_char();

   MenuError show();

   void set_active(bool set){ active = set; }
   bool is_active() { return active; }

   const static size_t MAX_WIDTH = 127;

protected:
   class MenuItem
   {
   public:
      MenuItem(){};
      virtual ~MenuItem() {}

      virtual size_t get_id(){ return id; };
      virtual MenuError get_value(std::string& output)=0;
      virtual MenuType get_type()=0;
      virtual size_t width()=0;
      virtual void print(std::string& output, size_t padding = 0)=0;

      virtual bool is_selectable()=0;
      virtual void set_select(bool select)=0;
      virtual bool selected() { return is_selected; }

      virtual bool is_editable()=0;
      virtual bool push_char(char c)=0;
      virtual bool pop_char()=0;
      virtual void get_entry(std::string& output)=0;

   protected:
      size_t id = 0;
      bool is_centered = false;
      bool is_selected = false;

   };

   class MenuButton : public MenuItem
   {
   public:
      MenuButton(const std::string& new_name, size_t new_id, bool new_is_centered);
      virtual ~MenuButton() {}

      virtual size_t get_id(){ return id; }
      virtual MenuError get_value(std::string& output);
      virtual MenuType get_type();
      virtual size_t width();
      virtual void print(std::string& output, size_t padding = 0);
      virtual bool is_selectable(){ return true; }
      virtual void set_select(bool select);

      virtual bool is_editable(){ return false; }
      virtual bool push_char(char c){ return false; }
      virtual bool pop_char(){ return false; }
      virtual void get_entry(std::string& output) { return; }

   private:
      std::string title{};

   };

   class MenuLabel : public MenuItem
   {
   public:
      MenuLabel(const std::string& name, bool new_is_centered);
      virtual ~MenuLabel() {}

      virtual MenuError get_value(std::string& output);
      virtual MenuType get_type();
      virtual size_t width();
      virtual void print(std::string& output, size_t padding);
      virtual bool is_selectable() { return false; }
      virtual void set_select(bool select);

      virtual bool is_editable(){ return false; }
      virtual bool push_char(char c){ return false; }
      virtual bool pop_char(){ return false; }
      virtual void get_entry(std::string& output) { return; }

   private:
      std::string title{};

   };

   class MenuText : public MenuItem
   {
   public:
      MenuText(const std::string& new_name, size_t new_id, bool new_is_centered);
      ~MenuText() {}

      virtual size_t get_id(){ return id; };
      virtual MenuError get_value(std::string& output);
      virtual MenuType get_type(){ return MenuType::text; }
      virtual size_t width();
      virtual void print(std::string& output, size_t padding = 0);

      virtual bool is_selectable(){ return true; }
      virtual void set_select(bool select);

      virtual bool is_editable(){ return true; }
      virtual bool push_char(char c);
      virtual bool pop_char();
      virtual void get_entry(std::string& output) { output = text_entry; }

   private:
      const size_t max_length = 32;

      std::string title{};
      std::string text_entry{};


   };   

private:
   std::string subtitle{};
   bool active = true;

   std::vector<std::unique_ptr<MenuItem>> menu_items{};

   static inline void add_space(std::string& input, size_t amount){
      for ( size_t i = 0; i < amount; ++i )
      {
         input.push_back(' ');
      }
   };


   const std::string header{R"(_____/\\\\\\\\\\\______________________________________________________________________________________________________
 ___/\\\/////////\\\____________________________________________________________________________________________________
  __\//\\\______\///______/\\\_______________________________________/\\\_______________________/\\\\\\\\________________
   ___\////\\\__________/\\\\\\\\\\\__/\\/\\\\\\\___/\\\\\\\\\_____/\\\\\\\\\\\_____/\\\\\\\\___/\\\////\\\_____/\\\\\____
    ______\////\\\______\////\\\////__\/\\\/////\\\_\////////\\\___\////\\\////____/\\\/////\\\_\//\\\\\\\\\___/\\\///\\\__
     _________\////\\\______\/\\\______\/\\\___\///____/\\\\\\\\\\_____\/\\\_______/\\\\\\\\\\\___\///////\\\__/\\\__\//\\\_
      __/\\\______\//\\\_____\/\\\_/\\__\/\\\__________/\\\/////\\\_____\/\\\_/\\__\//\\///////____/\\_____\\\_\//\\\__/\\\__
       _\///\\\\\\\\\\\/______\//\\\\\___\/\\\_________\//\\\\\\\\/\\____\//\\\\\____\//\\\\\\\\\\_\//\\\\\\\\___\///\\\\\/___
        ___\///////////_________\/////____\///___________\////////\//______\/////______\//////////___\////////______\/////_____)""\n"};

};

#endif
