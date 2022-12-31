#pragma once

#include "position.hpp"
#include "map.hpp"

int neighboring_ennemies(const map &map, const position &source);
int neighboring_allies(const map &map, const position &source);
position closest_non_controlled(const map &m, const position &start);
