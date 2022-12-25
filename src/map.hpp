#pragma once

#include "map_cell.hpp"
#include <istream>
#include <vector>

struct map {
  using container = std::vector<std::vector<map_cell>>;

  map() = default;
  map(int h, int w) : height_{h}, width_{w}, map_{} {}

  map_cell &at(const position &pos) { return map_[pos.x][pos.y]; }
  const map_cell &at(const position &pos) const {
    return const_cast<map *>(this)->at(pos);
  }

  bool valid(const position &pos) const {
    return pos.x >= 0 && pos.y >= 0 & pos.x < height() && pos.y < width();
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
    int width;
    int height;
    in >> width >> height;
    in.ignore();
    map.map_.resize(height);
    for (int i = 0; i < height; ++i) {
      map.map_[i].resize(width);
      for (int j = 0; j < width; ++j) {
        map.map_[i][j].coordinates = {i, j};
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
