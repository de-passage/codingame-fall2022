#pragma once

#include <iostream>

enum class player { me, opponent, neutral };

inline std::istream& operator>>(std::istream& in, player& pl) {
    int i;
    in >> i;
    if (i == 0) pl = player::opponent;
    else if (i > 0) pl = player::me;
    else pl = player::neutral;
    return in;
}
