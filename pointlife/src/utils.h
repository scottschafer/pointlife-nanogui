//
//  utils.h
//  ClusterBotzGPU
//
//  Created by Scott Schafer on 5/22/17.
//  Copyright © 2017 Scott Schafer. All rights reserved.
//

#ifndef utils_h
#define utils_h
#include <iostream>

extern int globalId;

long curMicroseconds();
double curSeconds();

std::string getHeaders();


inline float frand() {
  return float(rand()) / float(RAND_MAX);
}

inline float randRange(float minV, float maxV) {
  return minV + (maxV - minV) * float(rand()) / float(RAND_MAX);
}

// inclusive
inline int randRange(int minV, int maxV) {
  return minV + rand() % (maxV - minV + 1);
}

#endif /* utils_h */
