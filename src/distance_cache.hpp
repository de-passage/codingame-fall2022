#pragma once

#include <unordered_map>
#include "position.hpp"

struct pair_hash {
  std::size_t operator()(const std::pair<position, position>& p) const {
    constexpr auto h = position_hash{};
    return h(p.first) ^ h(p.second);
  }
};
using distance_cache = std::unordered_map<std::pair<position, position>, int, pair_hash>;
