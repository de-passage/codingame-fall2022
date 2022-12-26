#include <algorithm>
#include <iostream>
#include <queue>
#include <string>
#include <vector>

#include "map.hpp"
#include "map_cell.hpp"
#include "player.hpp"
#include "position.hpp"
#include "utils.hpp"

using namespace std;

constexpr int ROBOT_COST = 10;
constexpr int BUILDER_COST = 10;

int main(int argc, const char** argv) {
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
        values;

    for (int i = 0; i < map.width(); ++i) {
      for (int j = 0; j < map.height(); ++j) {
        const auto &cell = map[i][j];
        if (cell.owner == player::me && cell.units > 0) {
          units.emplace(cell);
        }
        int cell_value = value_of(map, cell.coordinates);
        if (cell.owner != player::me && cell.recycler == 0) {
          values.emplace(cell.coordinates, cell_value);
        } else if (my_matter >= 10 && cell.owner == player::me &&
                   cell.recycler == 0) {
          if (cell_value > 10 && cell.can_build) {
            cout << "BUILD " << cell.coordinates << ';';
            my_matter -= 10;
          } else if (cell.units == 0 && cell.can_spawn) {
            cout << "SPAWN 1 " << cell.coordinates << ';';
            my_matter -= 10;
          }
        }
      }
    }

    while (!units.empty() && !values.empty()) {
      auto cell = units.top();
      units.pop();
      auto target = values.top();
      values.pop();

      cout << "MOVE " << cell.units << ' ' << cell.coordinates << ' '
           << target.pos << ';';
    }
    cout << "MESSAGE " << message << endl;
  }
}
