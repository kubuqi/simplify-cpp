# simplify-cpp
C++ port of simplify-js (https://github.com/mourner/simplify-js) 

# Usage

#include "../simplify.h"

struct Point {
    float x;
    float y;
};

std::vector<Point> result = simplify<Point, float>(points, 5);
  
