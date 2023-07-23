//
//  utils.cpp
//  ClusterBotzGPU
//
//  Created by Scott Schafer on 5/13/17.
//  Copyright © 2017 Scott Schafer. All rights reserved.
//

#include <iostream>
#include <vector>
#include <GLUT/glut.h>
#include <cstdlib>
#include <pthread.h>
#include <unistd.h>
#include <limits.h>

#include "constants.h"
#include "utils.h"
#include "gpuOrganism/core/_inc/VO_size.h"
#include "gpuOrganism/core/_inc/CellTypes.h"
#include "../EasyCL/EasyCL.h"

#define STRINGIFY(TOSTRINGIFY) #TOSTRINGIFY

easycl::CLKernel * gKernel;

const char * src_vo = "";
//#include "VO.h"
//;

using namespace std;

float VALIDATE_COORD(float x) {
    if (! (x >= 0 && x < WORLD_DIM)) {
      printf("aha\n");
    }
    return x;
}

float VALIDATE_VELOCITY(float x) {
    if (! (x >= -WORLD_DIM * 1000 && x < WORLD_DIM * 1000)) {
      printf("aha\n");
    }
    return x;
}


int globalId = 0;

static auto programStartTime = std::chrono::high_resolution_clock::now();

long curMicroseconds() {
  auto end_time = chrono::high_resolution_clock::now();

	return std::chrono::duration_cast<chrono::microseconds>(end_time - programStartTime).count();
}

double curSeconds() {
  return double(curMicroseconds()) / 1000000.0;
}

using namespace std;
inline std::string toString(int i) {
  char buffer[100];
  sprintf(buffer, "%d", i);
  return buffer;
}

inline std::string toString(float i) {
  char buffer[100];
  sprintf(buffer, "%ff", i);
  return buffer;
}

inline std::string toString(std::string i) {
  return i;
}

template<class T>
inline std::string generateDefine(std::string symbol, T val) {
  std::string str;
  str += "#define " + symbol + " " + toString(val) + "\n";
  return str;
}

std::string getHeaders() {
  std::string str;
  
  str += generateDefine("NUM_CELLS", NUM_CELLS);
  str += generateDefine("MAX_BONES", MAX_BONES);
  str += generateDefine("MAX_BONE_CONNECTIONS", MAX_BONE_CONNECTIONS);
  // str += generateDefine("MAX_CELLS_PER_ORGANISM", MAX_CELLS_PER_ORGANISM);
  str += generateDefine("MAX_VELOCITY", MAX_VELOCITY);
  str += generateDefine("MAX_CONNECTIONS", MAX_CONNECTIONS);
  str += generateDefine("WORLD_DIM", WORLD_DIM);
  str += generateDefine("WALL_DAMPER", WALL_DAMPER);
  str += generateDefine("CELL_SIZE", CELL_SIZE);
  str += generateDefine("CPU", 0);
  // str += generateDefine("APPLY_CONNECTIONS_IN_POINT_PHYSICS", APPLY_CONNECTIONS_IN_POINT_PHYSICS);

  str += generateDefine("VALIDATE_COORD(x)", "");
  str += generateDefine("VALIDATE_VELOCITY(x)", "");

  str += "\n";
  
  str += src_vo;
  str += '\n';
  str += generateDefine("CELL_VO_SIZE", int(CELL_VO_SIZE));
  str += generateDefine("CONNECTION_VO_SIZE", int(CONNECTION_VO_SIZE));
  str += generateDefine("ORGANISM_VO_SIZE", int(ORGANISM_VO_SIZE));
  str += generateDefine("BONE_VO_SIZE", int(BONE_VO_SIZE));

  str += generateDefine("actionMove", int(actionMove));
  str += generateDefine("actionBite", int(actionBite));
  str += generateDefine("actionEye", int(actionEye));
  str += generateDefine("actionNerve", int(actionNerve));
  str += generateDefine("actionTimer", int(actionTimer));


  str += generateDefine("sigIncrement", int(sigIncrement));
  str += generateDefine("sigDecrement", int(sigDecrement));
  str += generateDefine("sigToggle", int(sigToggle));
  str += generateDefine("sigClear", int(sigClear));
  str += generateDefine("sigRebroadcast", int(sigRebroadcast));
  
  
  
  /*
  ,        // fires when facing the specified target
  ,      // fires when cell is compressed (according to param)
         // fires periodically (according to param)
};

// a cell can have a rule for each connection controlling how it will respond to a signal from
enum {
   = 1,     // increment some counter when receiving a signal
  ,
  ,
  ,
  ,
};

// cells can pass signals

// a connection can send a signal to connected cells.


// connection types
enum {
  connectionPassthrough = 1,
  connectionInvert,
  connectionNull
*/

/*
  
  str += generateDefine("RCMD_SET_LINE_COLOR", int(RCMD_SET_LINE_COLOR));
  str += generateDefine("RCMD_SET_FILL_COLOR", int(RCMD_SET_FILL_COLOR));
  str += generateDefine("RCMD_SET_LINE_WIDTH", int(RCMD_SET_LINE_WIDTH));
  str += generateDefine("RCMD_CIRCLE", int(RCMD_CIRCLE));
  str += generateDefine("RCMD_LINE", int(RCMD_LINE));
*/

  str += generateDefine("INT_MAX", int(INT_MAX));
  
  return str;
}




