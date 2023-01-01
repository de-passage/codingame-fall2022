#pragma once

#include "island.hpp"
#include "map.hpp"
#include "position.hpp"

int neighboring_ennemies(const map &map, const position &source);
int neighboring_allies(const map &map, const position &source);
position closest_non_controlled(const island_container& islands, const map &map, const position &start,
                                const std::vector<const map_cell *> &ennemies);
int walkable_neighbors(const map& map, const position& source);
std::pair<int, int>
most_threatening(const island_container &islands,const position &start,
                 const std::vector<const map_cell *> &ennemies);

bool will_disappear(const map& map, const position& cell);
bool can_reach_uncontrolled(const map &m, const position &start);

template <class F>
constexpr int count_neighbors(const map &map, const position &source, F &&func) {
  using namespace std;
  int s = 0;
  for (position offset : {position{0, 1}, {0, -1}, {1, 0}, {-1, 0}}) {
    auto pos = source + offset;
    if (map.valid(pos)) {
      auto &cell = map.at(pos);
      s += func(cell);
    }
  }
  return s;
}
