#pragma once

#include "map_cell.hpp"
#include "map.hpp"
#include "position.hpp"

struct index_by_bot_number {
  bool operator()(const map_cell &left, const map_cell &right) const {
    return left.units < right.units;
  }
  bool operator()(const map_cell *left, const map_cell *right) const {
    return left->units < right->units;
  }
};

struct position_with_value {
    position_with_value() = default;
    position_with_value(const position& pos, int v) : pos{pos}, value{v} {}
    position pos;
    int value;
};

struct position_with_value_hash {
  std::size_t operator()(const position_with_value& p) const {
    return position_hash{}(p.pos) ^ std::hash<int>{}(p.value);
  }
};

struct index_by_value {
  bool operator()(const position_with_value &left, const position_with_value &right) const {
    return left.value < right.value;
  }
};

struct index_by_lesser_value {
  bool operator()(const position_with_value &left, const position_with_value &right) const {
    return left.value < right.value;
  }
};

// scrap value as if populated by a recycler
inline int value_of(const map& map, const position& pos) {
    int sum = 0;
    for (auto p : {position{1,0}, {0,1}, {-1,0}, {0,-1}, {0,0}}) {
        auto c = pos + p;
        if (valid(map, c)) {
          for (auto pp : {position{1, 0}, {0, 1}, {-1, 0}, {0, -1}, {0, 0}}) {
            auto cc = c +pp;
            if (valid(map, cc) && map[cc.x][cc.y].recycler > 0) return 0;
            sum += map[c.x][c.y].scrap_amount;
          }
        }
    }
    return sum;
}

inline size_t distance_squared(const position& p1, const position& p2) {
  int dx = p1.x - p2.x;
  int dy = p1.y - p2.y;
  return dx*dx + dy*dy;
}
