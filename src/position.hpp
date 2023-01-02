#pragma once
#include <iostream>

struct position {
  constexpr position() = default;
  constexpr position(const position &) = default;
  constexpr position(position &&) = default;
  position &operator=(const position &pos) = default;
  position &operator=(position &&pos) = default;
  constexpr position(int x, int y) : x{x}, y{y} {}
  int x = 0;
  int y = 0;

  friend position operator+(const position &p1, const position &p2) {
    return {p1.x + p2.x, p1.y + p2.y};
  }

  friend position operator-(const position &p1, const position &p2) {
    return {p1.x - p2.x, p1.y - p2.y};
  }

  friend position &operator+=(position &p1, const position &p2) {
    return p1 = {p1 + p2};
  }

  friend bool operator==(const position &p1, const position &p2) {
    return p1.x == p2.x && p1.y == p2.y;
  }

  friend bool operator!=(const position &p1, const position &p2) {
    return !(p1 == p2);
  }
};

inline std::ostream &operator<<(std::ostream &out, const position &p) {
  return out << p.y << ' ' << p.x;
}

struct position_hash {
  inline std::size_t operator()(position p) const {
    return std::hash<int>()(p.x) ^ std::hash<int>()(p.y);
  }
};
