#include "algorithms.hpp"
#include "map_cell.hpp"
#include "utils.hpp"

#include <algorithm>
#include <stack>
#include <unordered_set>
#include <vector>

position closest_non_controlled(const map &m, const position &start,
                                const std::vector<const map_cell *> &ennemies) {
  using namespace std;

  unordered_set<position, position_hash> visited;
  stack<position> to_visit;
  visited.emplace(start);
  to_visit.emplace(start);
  vector<position> pos = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

  while (!to_visit.empty()) {
    position current = to_visit.top();
    to_visit.pop();
    sort(begin(pos), end(pos),
         [&](const position &left, const position &right) {
           auto left_threat = most_threatening(left, ennemies);
           auto right_threat = most_threatening(right, ennemies);

           if (left_threat.first == right_threat.first)
             return left_threat.second < right_threat.second;
           return left_threat.first < right_threat.first;
         });
    for (position offset : pos) {
      position next_pos = current + offset;
      if (m.walkable(next_pos)) {
        if (m.at(next_pos).owner != player::me) {
          return next_pos;
        } else if (!visited.count(next_pos)) {
          visited.emplace(next_pos);
          to_visit.push(next_pos);
        }
      }
    }
  }
  return start;
}

constexpr inline int neighboring_units(const map &map, const position &source,
                                       player pl) {
  return count_neighbors(map, source, [pl](const map_cell &cell) {
    return cell.owner == pl ? cell.units : 0;
  });
}

int neighboring_ennemies(const map &map, const position &source) {
  return neighboring_units(map, source, player::opponent);
}

int neighboring_allies(const map &map, const position &source) {
  return neighboring_units(map, source, player::me);
}

int neighboring_recyclers(const map &map, const position &source) {
  return count_neighbors(map, source,
                         [](const map_cell &cell) { return cell.recycler; });
}

std::pair<int, int>
most_threatening(const position &current,
                 const std::vector<const map_cell *> &ennemies) {
  int current_radius = std::numeric_limits<int>::max();
  int current_nb = 0;
  for (auto *ennemy : ennemies) {
    int d = distance_squared(current, ennemy->coordinates);
    if (d > 0 && d < current_radius) {
      current_radius = d;
      current_nb = ennemy->units;
    } else if (d == current_radius) {
      current_nb += ennemy->units;
    }
  }
  return std::make_pair(current_radius, current_nb);
}

bool will_disappear(const map &map, const position &cell) {
  auto &mcell = map.at(cell);
  return mcell.in_range_of_recycler == 1 && mcell.scrap_amount == 1;
}

int walkable_neighbors(const map &map, const position &source) {
  return count_neighbors(map, source, [](const map_cell &cell) {
    return cell.walkable() ? 1 : 0;
  });
}
