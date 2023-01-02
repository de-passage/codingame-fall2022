#include "island.hpp"
#include "position.hpp"

#include <memory>
#include <queue>
#include <set>
#include <cassert>

void fill_island(island_container &islands, const map &map,
                 const position &current) {
  using namespace std;

  auto &cell = map.at(current);
  if (!cell.walkable())
    return;

  for (const island &island : islands) {
    if (island.cells.count(current)) {
      return;
    }
  }

  unordered_set<position, position_hash> visited;
  queue<position> to_visit;

  island this_island;

  visited.emplace(current);
  to_visit.push(current);

  while (!to_visit.empty()) {
    auto current = to_visit.front();
    auto& current_cell = map.at(current);
    to_visit.pop();
    this_island.cells.emplace(current);

    switch (current_cell.owner) {
    case player::me:
      this_island.my_cells++;
      break;
    case player::opponent:
      this_island.opponent_cells++;
      break;
    case player::neutral:
      this_island.unclaimed_cells++;
      break;
    }

    if (current_cell.in_range_of_recycler == 0) this_island.non_disappearing_cells += 1;

    for (position offset : {position{1, 0}, {-1, 0}, {0, 1}, {0, -1}}) {
      auto next = current + offset;
      if (!map.valid(next)) continue;
      auto &next_cell = map.at(next);
      if (next_cell.walkable() && !visited.count(next)) {
        visited.emplace(next);
        to_visit.push(next);
      }
    }
  }
  islands.emplace_back(move(this_island));
}

const island& island_containing(const island_container& islands, const position& position) {
  for (auto& island : islands) {
    if (island.cells.count(position)) {
      return island;
    }
  }
  assert(false && "Position without island");
}

bool same_island(const island_container &islands, const position &left, const position &right) {
  for (auto& island : islands) {
    bool has_left = island.cells.count(left) != 0;
    bool has_right = island.cells.count(right) != 0;
    if (has_left != has_right) return false;
    if (has_left && has_right) return true;
  }
  return false;
}
