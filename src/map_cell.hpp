#pragma once
#include <iostream>

#include "player.hpp"
#include "position.hpp"

struct map_cell {
  int scrap_amount;
  player owner; // 1 = me, 0 = foe, -1 = neutral
  int units;
  int recycler;
  int can_build;
  int can_spawn;
  int in_range_of_recycler;
  position coordinates;
};

inline std::istream &operator>>(std::istream &in, map_cell &cell) {
  in >> cell.scrap_amount >> cell.owner >> cell.units >> cell.recycler >>
      cell.can_build >> cell.can_spawn >> cell.in_range_of_recycler;
  in.ignore();
  return in;
}
