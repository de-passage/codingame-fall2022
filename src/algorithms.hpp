#pragma once

#include <unordered_set>
#include <stack>
#include <vector>
#include <algorithm>

#include "position.hpp"
#include "map.hpp"

position closest_non_controlled(const map &m, const position &start) {
  using namespace std;

  unordered_set<position, position_hash> visited;
  stack<position> to_visit;
  visited.emplace(start);
  to_visit.emplace(start);
  vector<position> pos = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

  while (!to_visit.empty()) {
    position current = to_visit.top();
    to_visit.pop();
    random_shuffle(begin(pos), end(pos));
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

int neighboring_ennemies(const map &map, const position &source) {
  using namespace std;
  int s = 0;
  for (position offset : {position{0, 1}, {0, -1}, {1, 0}, {-1, 0}}) {
    auto pos = source + offset;
    if (map.valid(pos)) {
      auto &cell = map.at(pos);
      if (cell.owner != player::me) {
        s += cell.units;
      }
    }
  }
  return s;
}
