
/*
 * (c) 2019, Luke Zhang
 * C++ version of Simplify.js, a high-performance JS polyline simplification library. See mourner.github.io/simplify-js
 */

#include <vector>

// square distance between 2 points
// to suit your point format, run search/replace for '.x' and '.y';
template <typename Point, typename Distance> 
Distance getSqDist(const Point &p1, const Point &p2) {
    Distance dx = p1.x - p2.x;
    Distance dy = p1.y - p2.y;
    return dx * dx + dy * dy;
}

// square distance from a point to a segment
template <typename Point, typename Distance> 
Distance getSqSegDist(const Point &p, const Point &p1, const Point &p2) {
    Distance x = p1.x;
    Distance y = p1.y;
    Distance dx = p2.x - x;
    Distance dy = p2.y - y;

    if (dx != 0 || dy != 0) {
        Distance t = ((p.x - x) * dx + (p.y - y) * dy) / (dx * dx + dy * dy);

        if (t > 1) {
            x = p2.x;
            y = p2.y;

        } else if (t > 0) {
            x += dx * t;
            y += dy * t;
        }
    }

    dx = p.x - x;
    dy = p.y - y;

    return dx * dx + dy * dy;
}

// asic distance-based simplification.
template <typename Point, typename Distance> 
std::vector<Point> simplifyRadialDist(const std::vector<Point> &points, Distance tolerance) {
    std::vector<Point> simplified;

    // Special handling for only 0, 1 or 2 points.
    if (points.size() <= 2) {
        for (size_t i=0; i<points.size(); i++)
            simplified.push_back(points[i]);
        return simplified;
    }

    // Always take the first point.
    simplified.push_back(points[0]);

    // Find the points within given distance.
    Distance sqrTolerance = tolerance * tolerance;
    size_t i=1;
    for (; i<points.size()-1; i++) {
        if (getSqDist<Point, Distance>(points[i], points[i-1]) > sqrTolerance) {
            simplified.push_back(points[i]);
        }
    }

    // Always take the last point.
    simplified.push_back(points[i]);
    return simplified;
}

template <typename Point, typename Distance> 
void simplifyDPStep(std::vector<Point> &points, size_t first, size_t last, Distance sqTolerance, std::vector<Point> &out) {
  Distance maxSqDist = sqTolerance;
  size_t index = 0;

  for (size_t i = first + 1; i < last; i++) {
    Distance sqDist = getSqSegDist<Point, Distance>(points[i], points[first], points[last]);
    if (sqDist > maxSqDist) {
      index = i;
      maxSqDist = sqDist;
    }
  }

  if (maxSqDist > sqTolerance) {
    if (index - first > 1)
        simplifyDPStep(points, first, index, sqTolerance, out);

    out.push_back(points[index]);

    if (last - index > 1)
        simplifyDPStep(points, index, last, sqTolerance, out);
  }
}

// simplification using Ramer-Douglas-Peucker algorithm
template <typename Point, typename Distance> 
std::vector<Point> simplifyDouglasPeucker(std::vector<Point> &points, Distance sqTolerance) {
    std::vector<Point> simplified;
    simplified.push_back(points[0]);

    size_t last = points.size() - 1;
    simplifyDPStep(points, 0, last, sqTolerance, simplified);

    simplified.push_back(points[last]);
    return simplified;
}

// both algorithms combined for awesome performance.
template <typename Point, typename Distance> 
std::vector<Point> simplify(const std::vector<Point> &points, const Distance &tolerance=1, bool highestQuality=true) {
    std::vector<Point> simplified;

    if (points.size() <= 2) {
        for (size_t i=0; i<points.size(); i++)
            simplified.push_back(points[i]);
        return simplified;
    }

    Distance sqTolerance = tolerance * tolerance;

    std::vector<Point> result = highestQuality ? points : simplifyRadialDist(points, sqTolerance);
    result = simplifyDouglasPeucker(result, sqTolerance);

    return result;
}
