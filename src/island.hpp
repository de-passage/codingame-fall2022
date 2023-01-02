#pragma once

#include "position.hpp"
#include "map.hpp"

#include <unordered_set>
#include <vector>

struct island {
  std::unordered_set<position, position_hash> cells;
  int unclaimed_cells = 0;
  int opponent_cells = 0;
  int my_cells = 0;
  int non_disappearing_cells = 0;
};

using island_container = std::vector<island>;

void fill_island(island_container& islands, const map& map, const position& current);
const island& island_containing(const island_container& islands, const position& position);
bool same_island(const island_container& islands, const position& left, const position& right);
