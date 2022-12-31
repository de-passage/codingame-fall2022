#pragma once

#include <unordered_map>
#include <queue>

#include "position.hpp"
#include "map.hpp"

struct pair_hash {
  std::size_t operator()(const std::pair<position, position>& p) const {
    constexpr auto h = position_hash{};
    return h(p.first) ^ h(p.second);
  }
};
using distance_cache = std::unordered_map<std::pair<position, position>, int, pair_hash>;

int get_distance(const position &start, const position &target,
                 distance_cache &cache);

void fill_cache(const map &m, distance_cache &cache, const position &current);

void generate_distance_matrix(const map &m, distance_cache &cache);
