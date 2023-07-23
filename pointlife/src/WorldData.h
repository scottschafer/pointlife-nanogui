//
//  WorldData.h
//  ClusterBotzGPU
//
//  Created by Scott Schafer on 5/14/17.
//  Copyright © 2017 Scott Schafer. All rights reserved.
//

#ifndef WorldData_h
#define WorldData_h

#include <vector>
#include <math.h>

#include "../EasyCL/EasyCL.h"
#include "constants.h"
#include "FlexArrays.h"

#include "gpuOrganism/core/_inc/VO_size.h"

using namespace std;
using namespace easycl;

namespace cbtz {
  class OrganismCTOR;
};

class WorldData {
public:
  WorldData();
  
  void init();
  
  void copyToDevice();
  void copyFromDevice();
  
  void copyToRenderer();
  
  /*
  float distance(int iCell1, int iCell2) {
    float xd = cellPosX[iCell1] - cellPosX[iCell2];
    float yd = cellPosY[iCell1] - cellPosY[iCell2];
    return sqrt(xd*xd + yd*yd);
  }
  */

  void beginCreatureConstruction();
  void endCreatureConstruction();
  void add(cbtz::OrganismCTOR *);
  
  vector<BaseArray*> allArrays;

  vector<BaseArray*> organismArrays;
  vector<BaseArray*> cellArrays;
  vector<BaseArray*> connectionArrays;
  
  int turn;
  int totalCellCount;
  int totalOrganismCount;
  bool inited;

  // ---- cell and organism data
  FloatArray  cellData;
  FloatArray  connectionData;
  FloatArray  organismData;
  FloatArray  boneData;
  FloatArray  cellDataToRender;
  FloatArray  settingsData;

  // ---- collision grid information
  IntArray collisionGrid;

  // ---- rendering data
  FloatArray renderData;

  void validate() {
    CellVO * pCellVO2 = (CellVO *) cellData.data();
    int count = totalCellCount;
    float minVel = -MAX_VELOCITY*20.0f;
    float maxVel = MAX_VELOCITY*20.0f;
    while (count--) {
      CellVO * pCellVO = pCellVO2++;
      if (pCellVO->connectionCount < 0) {
        continue;
      }
      if (pCellVO->velX >= minVel && pCellVO->velX <=maxVel) {
        continue;
      }
      throw "error";
    }
  }
  
private:
  void registerOrganismArray(BaseArray &a) { allArrays.push_back(&a); organismArrays.push_back(&a); }
  void registerCellArray(BaseArray &a) { allArrays.push_back(&a); cellArrays.push_back(&a); }
  void registerConnectionArray(BaseArray &a) { allArrays.push_back(&a); connectionArrays.push_back(&a); }
  void registerArray(BaseArray &a) { allArrays.push_back(&a); }
};


#endif /* WorldData_h */
