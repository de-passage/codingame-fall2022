#include <algorithm>
#include <chrono>
#include <iostream>
#include <limits>
#include <ostream>
#include <queue>
#include <stack>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "algorithms.hpp"
#include "distance_cache.hpp"
#include "island.hpp"
#include "map.hpp"
#include "map_cell.hpp"
#include "player.hpp"
#include "position.hpp"
#include "utils.hpp"

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

  constexpr const char *lyrics[] = {"I am a bot", "and I live in a CPU",
                                    "compute,compute,compute!", "...", "compute,compute,compute!", "Again!"};
  unsigned int lyric_offset = 0;
  constexpr unsigned int MAX_LYRICS_OFFSET = size(lyrics);
  const auto next_lyrics = [&lyric_offset, &lyrics]() {
    const char *ret = lyrics[lyric_offset];
    lyric_offset = (lyric_offset + 1) % MAX_LYRICS_OFFSET;
    return ret;
  };

  int my_matter;
  int opp_matter;
  cin >> my_matter >> opp_matter;
  cin >> map;

  vector<const map_cell *> ennemies;
  vector<const map_cell *> my_cells;
  for (int i = 0; i < map.width(); ++i) {
    for (int j = 0; j < map.height(); ++j) {
      const auto &cell = map[i][j];
      if (cell.owner == player::me) {
        if (cell.can_build) {
          cout << "BUILD " << cell.coordinates << ';';
        } else if (cell.units > 0) {
          my_cells.push_back(addressof(cell));
        }
      } else if (cell.owner == player::opponent && cell.units > 0) {
        ennemies.push_back(addressof(cell));
      }
    }
  }
  for (auto &c : my_cells) {
    cout << "MOVE 1 " << c->coordinates << ' ' << ennemies.front()->coordinates << ';';
  }
  cout << "MESSAGE Let's go!" << endl;

  // game loop
  while (1) {
    cache.clear();
    my_cells.clear();
    ennemies.clear();
    int my_matter;
    int opp_matter;
    cin >> my_matter >> opp_matter;
    cin >> map;

    priority_queue<const map_cell *, vector<const map_cell *>,
                   index_by_bot_number>
        units;
    priority_queue<position_with_value, vector<position_with_value>,
                   index_by_value>
        build_candidates;
    priority_queue<position_with_distance_and_strength,
                   vector<position_with_distance_and_strength>,
                   index_by_distance_then_strength>
        spawn_candidates;

    for (int i = 0; i < map.width(); ++i) {
      for (int j = 0; j < map.height(); ++j) {
        const auto &cell = map[i][j];

        // find all my units
        if (cell.owner == player::me && cell.units > 0) {
          units.emplace(addressof(cell));
        }

        if (cell.owner == player::me) {
          if (cell.can_build || cell.can_spawn) {
            my_cells.push_back(addressof(cell));
          }
        } else if (cell.owner == player::opponent && cell.units > 0) {
          ennemies.push_back(addressof(cell));
        }
      }
    }

    island_container islands;
    for (int i = 0; i < map.width(); ++i) {
      for (int j = 0; j < map.height(); ++j) {
        fill_island(islands, map, position{i, j});
      }
    }

    for (auto *cell : my_cells) {
      int enemies = neighboring_ennemies(map, cell->coordinates);
      if (cell->can_build && enemies > 0 &&
          walkable_neighbors(map, cell->coordinates) > 1) {
        int allies = neighboring_allies(map, cell->coordinates);
        build_candidates.emplace(
            position_with_value(cell->coordinates, allies - enemies));
      } else if (cell->can_spawn &&
                 has_invadable_neighbor(map, cell->coordinates) &&
                 island_containing(islands, cell->coordinates)
                         .non_disappearing_cells != 0) {
        auto [current_radius, current_nb] =
            most_threatening(islands, cell->coordinates, ennemies);
        spawn_candidates.emplace(position{cell->coordinates}, current_radius,
                                 current_nb);
      }
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
      auto &island = island_containing(islands, cell.coordinates);
      if (will_disappear(map, cell.coordinates) || island.opponent_cells == 0)
        continue;
      int possible = 1;
      if (cell.distance <= 1) {
        possible = max(min(my_matter / ROBOT_COST, cell.strength), 1);
      }
      my_matter -= (possible * ROBOT_COST);
      cout << "SPAWN " << possible << " " << cell.coordinates << ';';
    }

    while (!units.empty()) {
      auto cell = units.top();
      units.pop();

      auto target =
          closest_non_controlled(islands, map, cell->coordinates, ennemies);
      if (target != cell->coordinates) {
        auto threat = neighboring_ennemies(map, target);
        int response = cell->units;
        if (threat < cell->units && !will_disappear(map, cell->coordinates)) {
          response -= threat;
        }
        cout << "MOVE " << response << ' ' << cell->coordinates << ' ' << target
             << ';';
      }
    }
    cout << "MESSAGE " << next_lyrics() << endl;
    cerr << flush;
  }
}
