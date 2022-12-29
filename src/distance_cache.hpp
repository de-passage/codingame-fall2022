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
                 distance_cache &cache) {
  using namespace std;
  if (auto it = cache.find(make_pair(start, target)); it != cache.end()) {
    return it->second;
  }
  return -1;
}

void fill_cache(const map &m, distance_cache &cache, const position &current) {
  using namespace std;
  constexpr position offsets[] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
  queue<position> to_visit;
  cache[make_pair(current, current)] = 0;
  to_visit.emplace(current);

  while (!to_visit.empty()) {
    const auto visiting = to_visit.front();
    int current_distance = get_distance(current, visiting, cache);
    to_visit.pop();

    for (auto offset : offsets) {
      const auto next = offset + current;
      if (m.walkable(visiting) && get_distance(current, next, cache) < 0) {
        cache[make_pair(current, next)] = current_distance + 1;
        to_visit.push(next);
      }
    }
  }
}

void generate_distance_matrix(const map &m, distance_cache &cache) {
  for (int i = 0; i < m.width(); ++i) {
    for (int j = 0; j < m.width(); ++j) {
      fill_cache(m, cache, position{i, j});
    }
  }
}
