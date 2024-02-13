#ifndef STRATEGO_MOVES_H

enum class stratego_move
{
   none,

   // Menu interactions
   menu_up,
   menu_down,
   menu_enter,
   menu_exit,
   save

   //Game movements
   left,
   right,
   up,
   down,
   select,
   cont,

   // Game place selection
   piece_9,
   piece_8,
   piece_7,
   piece_6,
   piece_5,
   piece_4,
   piece_3,
   piece_2,
   piece_1,
   piece_bomb,
   piece_scout,
   piece_flag
};

#endif
