//
//  Point.h
//  nanoflann_test
//
//  Created by Scott Schafer on 5/10/17.
//  Copyright © 2017 Scott Schafer. All rights reserved.
//

#ifndef Point_h
#define Point_h
#include <math.h>
#include <algorithm>

template <typename T> inline
T abs(T v) { return v < 0 ? -v : v; }


/**
 * I'm unclear as to whether it's better (speed-wise) to pass by value or reference. But I'll trust this:
 * http://cpp-next.com/archive/2009/08/want-speed-pass-by-value/
 */

#define POINT_REF Point
//#define POINT_REF const Point &

typedef struct Point {
  float x, y;
  
  Point() {
    x = y = 0;
  }
  
  Point(float x, float y) : x(x), y(y) {
  }
  
  void reset() {
    x = y = 0;
  }
  
  Point operator + (POINT_REF p2) {
    return Point(x + p2.x, y + p2.y);
  }
  
  Point & operator += (POINT_REF p2) {
    x += p2.x;
    y += p2.y;
    return *this;
  }
  
  float squaredDistance(POINT_REF p2) {
    float x = this->x - p2.x;
    float y = this->y - p2.y;
    return x * x + y * y;
  }
  
  float dot(POINT_REF p2) {
    return x * p2.x + y * p2.y;
  }
  
  inline float distance(POINT_REF p2) {
    return accurateDistance(p2);
  }
  
  inline float distance() {
    return sqrtf(x*x+y*y);
  }
  
  inline float fastLength()
  {
    
    return std::max(abs(x),abs(y));
  }
  
  inline float fastDistance(POINT_REF p2)
  {
    float xd = x - p2.x;
    float yd = y - p2.y;
    
    return std::max(abs(xd),abs(yd));
  }
  
  inline float accurateDistance(POINT_REF p2)
  {
    float xd = x - p2.x;
    float yd = y - p2.y;
    
    return sqrtf(xd*xd+yd*yd);
  }
  
  inline float accurateLength() {
    return sqrtf(x*x+y*y);
  }
  
  /*
  void randomize() {
    x = Random::randRange(0, MAX_COORD);
    y = Random::randRange(0, MAX_COORD);
  }
  
  void randomize(float min, float max) {
    x = Random::randRange(min, max);
    y = Random::randRange(min, max);
  }
   */
  
  void set(float x, float y) {
    this->x = x;
    this->y = y;
  }
} Point;

inline float accurateDistance(float x1, float y1, float x2, float y2) {
  x1 -= x2;
  y1 -= y2;
  
  return sqrtf(x1*x1+y1*y1);
}

inline float accurateDistance(float x1, float y1) {
  return sqrtf(x1*x1+y1*y1);
}

typedef Point * PointPtr;


#endif /* Point_h */
