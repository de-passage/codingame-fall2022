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
#include "algorithms.hpp"

using namespace std;

constexpr int ROBOT_COST = 10;
constexpr int BUILDER_COST = 10;

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
