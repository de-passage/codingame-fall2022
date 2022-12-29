#include <algorithm>
#include <iostream>
#include <limits>
#include <queue>
#include <stack>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "distance_cache.hpp"
#include "map.hpp"
#include "map_cell.hpp"
#include "player.hpp"
#include "position.hpp"
#include "utils.hpp"

using namespace std;

constexpr int ROBOT_COST = 10;
constexpr int BUILDER_COST = 10;

template <class C> C random_shuffle(C &&container) {
  using std::begin;
  using std::end;
  random_shuffle(begin(container), end(container));
  return container;
}

int get_distance(const position &start, const position &target,
                 distance_cache &cache) {
  if (auto it = cache.find(make_pair(start, target)); it != cache.end()) {
    return it->second;
  }
  return -1;
}

void fill_cache(const map &m, distance_cache &cache, const position &current) {
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

position closest_non_controlled(const map &m, const position &start) {

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

struct position_with_distance_and_strength {
  constexpr position_with_distance_and_strength() = default;
  constexpr position_with_distance_and_strength(position p, int d, int s)
      : coordinates{p}, distance{d}, strength{s} {}
  position coordinates{};
  int distance = 0;
  int strength = 0;
};

struct index_by_distance_then_strength {
  constexpr inline bool
  operator()(const position_with_distance_and_strength &p1,
             const position_with_distance_and_strength &p2) const {
    if (p1.distance == p2.distance)
      return p1.strength < p2.strength;
    return p1.distance > p2.distance;
  }
};

int main(int argc, const char **argv) {
  using namespace std;
  string message = "Bzzzzzt...";

  if (argc > 1) {
    message = argv[1];
  }

  int width;
  int height;
  cerr << boolalpha;
  cin >> width >> height;
  map map(width, height);
  distance_cache cache;

  // game loop
  while (1) {
    cache.clear();
    int my_matter;
    int opp_matter;
    cin >> my_matter >> opp_matter;
    cin >> map;

    priority_queue<map_cell, vector<map_cell>, index_by_bot_number> units;
    priority_queue<position_with_value, vector<position_with_value>,
                   index_by_value>
        build_candidates;
    priority_queue<position_with_distance_and_strength,
                   vector<position_with_distance_and_strength>,
                   index_by_distance_then_strength>
        spawn_candidates;

    vector<const map_cell *> ennemies;
    vector<const map_cell *> spawnable_cells;

    for (int i = 0; i < map.width(); ++i) {
      for (int j = 0; j < map.height(); ++j) {
        const auto &cell = map[i][j];
        if (cell.owner == player::me && cell.units > 0) {
          units.emplace(cell);
        }

        if (cell.owner == player::me) {
          int nb = neighboring_ennemies(map, cell.coordinates);
          if (cell.can_build && nb > 0) {
            build_candidates.emplace(position_with_value(cell.coordinates, nb));
          } else if (cell.can_spawn) {
            spawnable_cells.push_back(addressof(cell));
          }
        } else if (cell.owner == player::opponent && cell.units > 0) {
          ennemies.push_back(addressof(cell));
        }
      }
    }

    for (auto *my_cell : spawnable_cells) {
      int current_radius = numeric_limits<int>::max();
      int current_nb = 0;
      fill_cache(map, cache, my_cell->coordinates);
      for (auto *ennemy : ennemies) {
        int d = get_distance(my_cell->coordinates, ennemy->coordinates, cache);
        if (d > 0 && d < current_radius) {
          current_radius = d;
          current_nb = ennemy->units;
        } else if (d == current_radius) {
          current_nb += ennemy->units;
        }
      }
      spawn_candidates.emplace(position{my_cell->coordinates}, current_radius,
                               current_nb);
    }

    while (my_matter >= BUILDER_COST && !build_candidates.empty()) {
      auto cell = build_candidates.top();
      build_candidates.pop();
      my_matter -= BUILDER_COST;
      cout << "BUILD " << cell.pos << ';';
    }

    while (my_matter >= ROBOT_COST && !spawn_candidates.empty()) {
      auto cell = spawn_candidates.top();
      spawn_candidates.pop();
      int possible = 1;
      if (cell.distance <= 1) {
        possible = min(my_matter / ROBOT_COST, cell.strength);
      }
      my_matter -= (possible * ROBOT_COST);
      cout << "SPAWN " << possible << " " << cell.coordinates << ';';
    }

    while (!units.empty()) {
      auto cell = units.top();
      units.pop();

      auto target = closest_non_controlled(map, cell.coordinates);
      if (target != cell.coordinates) {
        cout << "MOVE " << cell.units << ' ' << cell.coordinates << ' '
             << target << ';';
      }
    }
    cout << "MESSAGE " << message << endl;
  }
}
