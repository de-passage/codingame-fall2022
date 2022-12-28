#include <algorithm>
#include <iostream>
#include <queue>
#include <string>
#include <vector>
#include <stack>
#include <unordered_set>

#include "map.hpp"
#include "map_cell.hpp"
#include "player.hpp"
#include "position.hpp"
#include "utils.hpp"

using namespace std;

constexpr int ROBOT_COST = 10;
constexpr int BUILDER_COST = 10;

template<class C>
C random_shuffle(C&& container) {
  using std::begin; using std::end;
  random_shuffle(begin(container), end(container));
  return container;
}

position closest_non_controlled(const map& m, const position& start) {

  unordered_set<position, position_hash> visited;
  stack<position> to_visit;
  visited.emplace(start);
  to_visit.emplace(start);
  vector<position> pos = {{-1,0}, {1,0}, {0,-1}, {0,1}};

  while(!to_visit.empty()) {
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

int neighboring_ennemies(const map& map, const position& source) {
  int s = 0;
  for (position offset : {position{0,1}, {0,-1}, {1,0}, {-1,0}}) {
    auto pos = source + offset;
    if (map.valid(pos)) {
      auto& cell = map.at(pos);
      if (cell.owner != player::me) {
        s += cell.units;
      }
    }
  }
  return s;
}



int main(int argc, const char** argv) {
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

  // game loop
  while (1) {
    int my_matter;
    int opp_matter;
    cin >> my_matter >> opp_matter;
    cin >> map;

    priority_queue<map_cell, vector<map_cell>, index_by_bot_number> units;
    priority_queue<position_with_value, vector<position_with_value>,
                   index_by_value>
        build_candidates;
    priority_queue<position_with_value, vector<position_with_value>,
                   index_by_value>
        spawn_candidates;

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
            spawn_candidates.emplace(position_with_value(cell.coordinates, cell.units - nb));
          }
        }
      }
    }

    while (my_matter >= 10 && !build_candidates.empty()) {
      auto cell = build_candidates.top();
      build_candidates.pop();
      my_matter -= 10;
      cout << "BUILD " << cell.pos << ';';
    }

    while (my_matter >= 10 && !spawn_candidates.empty()) {
      auto cell = spawn_candidates.top();
      spawn_candidates.pop();
      int possible = min(my_matter / 10, cell.value);
      my_matter -= (possible * 10);
      cout << "SPAWN " << possible << " " << cell.pos << ';';
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
