#pragma once

#include "map_cell.hpp"
#include <istream>
#include <vector>

struct map {
  using container = std::vector<std::vector<map_cell>>;

  map(int h, int w) : height_{h}, width_{w}, map_{} {
    map_.resize(height_);
    for (int i = 0; i < width_; ++i) {
      map_[i].resize(height_);
      for (int j = 0; j < height_; ++j) {
        map_[i][j].coordinates = {i, j};
      }
    }
  }

  map_cell &at(const position &pos) { return map_[pos.x][pos.y]; }
  const map_cell &at(const position &pos) const {
    return const_cast<map *>(this)->at(pos);
  }

  /// Return true iff the position is inside the boundaries of the map
  bool valid(const position &pos) const {
    return pos.x >= 0 && pos.y >= 0 & pos.x < width_ && pos.y < height_;
  }

  /// Return true iff the position can be moved to (valid & non grass)
  bool walkable(const position& pos) const {
    return valid(pos) && at(pos).scrap_amount > 0;
  }

  int height() const { return height_; }

  int width() const { return width_; }

  const std::vector<map_cell>& operator[](int x) {
    return map_[x];
  }

  const std::vector<map_cell>& operator[](int x) const {
    return map_[x];
  }

private:
  int height_;
  int width_;

  container map_;

  friend inline std::istream &operator>>(std::istream &in, map &map) {
    for (int i = 0; i < map.width_; ++i) {
      for (int j = 0; j < map.height_; ++j) {
        auto &cell = map.map_[i][j];
        in >> cell;
      }
    }
    return in;
  }
};

inline bool valid(const map &map, const position &pos) {
  return map.valid(pos);
}
