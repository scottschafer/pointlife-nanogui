//
//  WorldData.cpp
//  ClusterBotzGPU
//
//  Created by Scott Schafer on 5/15/17.
//  Copyright © 2017 Scott Schafer. All rights reserved.
//

#include "WorldData.h"
#include "constants.h"
#include "../EasyCL/EasyCL.h"
#include "WorldData.h"
#include "Renderer.h"
#include "utils.h"
#include "gpuOrganism/applyCollisionPhysics.h"
#include "Genome.h"
// #include "CreatureConstructor.h"
// #include "OrganismCTOR.h"
#include "Globals.h"

extern EasyCL * cl;

float randCoord() {
  return frand() * WORLD_DIM;
}

float randVelocity() {
  return (frand() - .5f) * MAX_VELOCITY;
}

WorldData::WorldData() {
  turn = 0;
}

void WorldData :: init() {
  
  //cellDataToRender.setFlags(CL_MEM_WRITE_ONLY);// | CL_MEM_HOST_READ_ONLY);
  
  registerArray(cellData);
  registerArray(connectionData);
  registerArray(organismData);
  /*
  registerArray(boneData);
  registerArray(renderData);
  */
  registerArray(settingsData);
  
  //  renderData.resize(RENDER_BUFFER_SIZE);
  
  inited = false;
  
  settingsData.resize(sizeof(SettingsVO));
  SettingsVO * pSettings = (SettingsVO*) settingsData.data();
  setDefaults(pSettings);
  
  /*
   registerArray(renderData);
   
   renderData.resize(RENDER_BUFFER_SIZE);
   for (int i = 0; i < RENDER_BUFFER_SIZE; i++) {
   renderData[i] = 0;
   }
   */
  
  registerArray(collisionGrid);
  
  RenderLock rl;
  
  srand(0);
  
  collisionGrid.resize(WORLD_DIM*WORLD_DIM);
  for (int i = 0; i < WORLD_DIM*WORLD_DIM; i++) {
    collisionGrid[i] = -1;
  }
  
  // create organisms
  float spacing = float(WORLD_DIM) / float(NUM_TRIBOTS_SQRT + 1.0f);
  float triHeight = CELL_SIZE * sqrtf(3.0f) / 2;
  
  // allocate cells as be multiple of workgroup size
  int numCellsAlloc;
  
  numCellsAlloc = NUM_CELLS;
  numCellsAlloc += WORKGROUP_SIZE - 1;
  numCellsAlloc /= WORKGROUP_SIZE;
  numCellsAlloc *= WORKGROUP_SIZE;
  numCellsAlloc *= CELL_VO_SIZE;
//   = WORKGROUP_SIZE * int((NUM_CELLS * CELL_VO_SIZE + WORKGROUP_SIZE - 1) / WORKGROUP_SIZE);
  cellData.resize(numCellsAlloc);
  CellVO * pCell = (CellVO*) cellData.data();
  cellData.set(-1.0f);

  // force cells to be multiple of workgroup size
  int numConnectionsAlloc = WORKGROUP_SIZE * int((NUM_CELLS * CONNECTION_VO_SIZE * MAX_CONNECTIONS + WORKGROUP_SIZE - 1) / WORKGROUP_SIZE);
  connectionData.resize(numConnectionsAlloc);
  ConnectionVO * pConnection = (ConnectionVO*) connectionData.data();
  connectionData.set(-1.0f);

  // force cells to be multiple of workgroup size
  int numOrganismsAlloc = WORKGROUP_SIZE * int((NUM_TRIBOTS * ORGANISM_VO_SIZE + WORKGROUP_SIZE - 1) / WORKGROUP_SIZE);
  organismData.resize(numOrganismsAlloc);
  OrganismVO * pOrganism = (OrganismVO*) organismData.data();
  organismData.set(-1.0f);

  int cellIndex = 0;
  int organismIndex = -1;

  float xc = -spacing / 2.0f;
  for (int xi = 0; xi < NUM_TRIBOTS_SQRT; xi++) {
    xc += spacing;
    float yc = -spacing / 2.0f;
    for (int yi = 0; yi < NUM_TRIBOTS_SQRT; yi++) {
      yc += spacing;
      
      ++organismIndex;
      pOrganism->energy = INITIAL_ENERGY;

      for (int vertex = 0; vertex < 3; vertex++) {
        float x, y;
        int connectWith;
        
        switch (vertex) {
          default:
          case 0:
            x = xc;
            y = yc - CELL_SIZE / 2.0f + triHeight;
            connectWith = cellIndex + 1;
            break;
            
          case 1:
            x = xc + CELL_SIZE / 2.0f;
            y = yc - CELL_SIZE / 2.0f;
            connectWith = cellIndex + 1;
            break;
            
          case 2:
            x = xc - CELL_SIZE / 2.0f;
            y = yc - CELL_SIZE / 2.0f + triHeight;
            connectWith = cellIndex - 2;
            break;
        }
        
        pCell->x = x;
        pCell->y = y;
        pCell->connectionCount = 1;
        pCell->connections[0] = connectWith;
        pCell->organismIndex = organismIndex;
        pCell->vertexId = vertex;
        //pCell->actionType = actionEye;

        pCell->localData[WHEEL_LEFT] = 0; //-1.0f; //randRange(-1.0f, 1.0f);
        pCell->localData[WHEEL_RIGHT] = 0; //1.0f; //randRange(-1.0f, 1.0f);
        
        if (cellIndex == 0) {
          pCell->localData[WHEEL_LEFT] = .2;//.5f; //randRange(-1.0f, 1.0f);
          pCell->localData[WHEEL_RIGHT] = .2f; //randRange(-1.0f, 1.0f);
        }
        

        pCell->velX = 0; //randRange(-MAX_VELOCITY/2.0f, MAX_VELOCITY/2.0f);
        pCell->velY = 0; // randRange(-MAX_VELOCITY/2.0f, MAX_VELOCITY/2.0f);

        pConnection->iCellFrom = cellIndex;
        pConnection->iCellTo = connectWith;
        pConnection->maxDist = pConnection->minDist = CELL_SIZE;
        ++pConnection;
        
        ++pOrganism;

        ++pCell;
        ++cellIndex;
      }
    }
  }
  
  if (! cellData.inRange(pCell)) {
    printf("debug");
  }
  
  if (! connectionData.inRange(pConnection)) {
    printf("debug");
  }
  
  if (! organismData.inRange(pOrganism)) {
    printf("debug");
  }

  
  // force cells to be multiple of workgroup size
//  int numCells = WORKGROUP_SIZE * int((cellData.size() / CELL_VO_SIZE + WORKGROUP_SIZE - 1) / WORKGROUP_SIZE);
//
//  for (size_t i = 0; i < cellArrays.size(); i++) {
//    while (cellArrays[i]->size() < numCells) {
//      cellArrays[i]->appendNegativeOne();
//    }
//  }
  
  this->totalCellCount = NUM_CELLS;// / CELL_VO_SIZE;
  this->totalOrganismCount = NUM_TRIBOTS; // organismData.size() / ORGANISM_VO_SIZE;
  
//  while (cellData.size() < numCells * CELL_VO_SIZE) {
//    cellData.appendNegativeOne();
//  }
  
  /*
  int numConnections = WORKGROUP_SIZE * int((connectionData.size() / CONNECTION_VO_SIZE + WORKGROUP_SIZE - 1) / WORKGROUP_SIZE);
  while (connectionData.size() < numConnections * CONNECTION_VO_SIZE) {
    connectionData.appendNegativeOne();
  }
  
  int numOrganisms = WORKGROUP_SIZE * int((organismData.size() / ORGANISM_VO_SIZE + WORKGROUP_SIZE - 1) / WORKGROUP_SIZE);
  while (organismData.size() < numOrganisms * ORGANISM_VO_SIZE) {
    organismData.appendNegativeOne();
  }
  
  int numBones = WORKGROUP_SIZE * int((boneData.size() / BONE_VO_SIZE + WORKGROUP_SIZE - 1) / WORKGROUP_SIZE);
  while (boneData.size() < numBones * BONE_VO_SIZE) {
    boneData.appendNegativeOne();
  }
  */
  
  
  
  /*
   // force connections to be multiple of workgroup size
   //  int numConnections = WORKGROUP_SIZE * int((connectionFromIndices.size() + WORKGROUP_SIZE - 1) / WORKGROUP_SIZE);
   while (connectionFromIndices.size() < numConnections) {
   for (int i = 0; i < connectionArrays.size(); i++) {
   connectionArrays[i]->appendNegativeOne();
   }
   }
   
   while (organismStartConnectionIndex.size() < 1000) {
   for (int i = 0; i < organismArrays.size(); i++) {
   organismArrays[i]->appendNegativeOne();
   }
   }
   */
}

#if 0
void WorldData :: add(cbtz::OrganismCTOR * o) {
  map<CellCTOR*, int> cellsToIndex;
  
  int organismIndex = int(organismData.size() / ORGANISM_VO_SIZE);
  
  //organismEnergy.push_back(1000);
  //organismStartConnectionIndex.push_back((int) connectionFromIndices.size());
  //organismConnectionLength.push_back((int) o->connections.size());
  
  for (int j = 0; j < ORGANISM_VO_SIZE; j++) {
    organismData.push_back(-1.0f);
  }
  
  OrganismVO * pOrganismVO = (OrganismVO*) (organismData.data() + organismData.size() - ORGANISM_VO_SIZE);
  pOrganismVO->iStartConnection = connectionData.size() / CONNECTION_VO_SIZE;
  pOrganismVO->numConnections = 0;
  pOrganismVO->energy = INITIAL_ENERGY;
  
  // add cells
  for (int i = 0; i < int(o->cells.size()); i++) {
    CellCTOR * pCell = o->cells[i];
    pCell->mVO.velX = pCell->mVO.velY = 0;
    if (pCell->mNumConnections > 2) {
      switch (pCell->mVO.actionType) {
        case actionMove:
        case actionEye:
          pCell->mVO.actionType = 0;
          break;
      }
    }
    
    float vs = CELL_VO_SIZE;
    cellsToIndex[pCell] = (int) cellData.size() / CELL_VO_SIZE;
    
    for (int j = 0; j < CELL_VO_SIZE; j++) {
      cellData.push_back(-1.0f);
    }
    
    CellVO * pCellVO = (CellVO*) (cellData.data() + cellData.size() - CELL_VO_SIZE);
    CellVO & outCell = *pCellVO;
    
    outCell = pCell->mVO;
    //::memcpy(&outCell, &pCell->mVO, sizeof(pCellVO));
    
    outCell.x = VALIDATE_COORD(pCell->mPos.x);
    outCell.y = VALIDATE_COORD(pCell->mPos.y);
    outCell.velX = randRange(-MAX_VELOCITY/2, MAX_VELOCITY/2);
    outCell.velY = randRange(-MAX_VELOCITY/2, MAX_VELOCITY/2);
    
    outCell.activationDuration = 0;
    outCell.velX = 0;//randRange(-MAX_VELOCITY/2, MAX_VELOCITY/2);
    outCell.velY = 0;//randRange(-MAX_VELOCITY/2, MAX_VELOCITY/2);
    
    outCell.connectionCount = 0;
    //outCell.actionType = pCell->mAction;
    outCell.organismIndex = organismIndex;
    
    /*
     cellPosX.push_back(VALIDATE_COORD(pCell->mPos.x));
     cellPosY.push_back(VALIDATE_COORD(pCell->mPos.y));
     cellVelX.push_back(randRange(-MAX_VELOCITY/2, MAX_VELOCITY/2));
     cellVelY.push_back(randRange(-MAX_VELOCITY/2, MAX_VELOCITY/2));
     cellType.push_back(pCell->mAction);
     cellConnectionCount.push_back(0);
     for (int j = 0; j < MAX_CONNECTIONS; j++) {
     cellConnections.push_back(-1);
     }
     cellNextInGrid.push_back(-1);
     cellOrganismIndices.push_back(organismIndex);
     */
  }
  
  // add connections
  for (int i = 0; i < o->connections.size(); i++) {
    ConnectionCTOR c = o->connections[i];
    
    int iCell1 = cellsToIndex[c.cell1];
    int iCell2 = cellsToIndex[c.cell2];
    float dist = c.cell1->mPos.distance(c.cell2->mPos);
    
    for (int j = 0; j < CONNECTION_VO_SIZE; j++) {
      connectionData.push_back(-1);
    }
    ConnectionVO * pConnectionVO = (ConnectionVO*) (connectionData.data() + connectionData.size() - CONNECTION_VO_SIZE);
    
    pConnectionVO->iCellFrom = iCell1;
    pConnectionVO->iCellTo = iCell2;
    pConnectionVO->minDist = dist;
    pConnectionVO->maxDist = dist;
    
    ++pOrganismVO->numConnections;
    
    if (dist < CELL_SIZE * .5f || dist > CELL_SIZE * 1.5f) {
      printf("problem!");
    }
    
    CellVO & outCell1 = * (CellVO*) (cellData.data() + iCell1 * CELL_VO_SIZE);
    CellVO & outCell2 = * (CellVO*) (cellData.data() + iCell2 * CELL_VO_SIZE);
    
    int connectionCount1 = outCell1.connectionCount;
    int connectionCount2 = outCell2.connectionCount;
    
    if (connectionCount1 < MAX_CONNECTIONS) {
      outCell1.connections[connectionCount1] = iCell2;
      //outCell1.connectionMinDistance[connectionCount1] = dist;//connectionMinDistances[i];
      //outCell1.connectionMaxDistance[connectionCount1] = dist;//connectionMaxDistances[i];
      outCell1.connectionCount++;
    }
    
    if (connectionCount2 < MAX_CONNECTIONS) {
      outCell2.connections[connectionCount2] = iCell1;
      //outCell2.connectionMinDistance[connectionCount2] = dist;//connectionMinDistances[i];
      //outCell2.connectionMaxDistance[connectionCount2] = dist;//connectionMaxDistances[i];
      outCell2.connectionCount++;
    }
    
    /*
     if (cellConnectionCount[iCell1] < MAX_CONNECTIONS) {
     cellConnections[iCell1 * MAX_CONNECTIONS + cellConnectionCount[iCell1]++] = iCell2;
     }
     
     if (cellConnectionCount[iCell2] < MAX_CONNECTIONS) {
     cellConnections[iCell2 * MAX_CONNECTIONS + cellConnectionCount[iCell2]++] = iCell1;
     }
     */
  }
  
  
  // add bones
  for (int i = 0; i < int(o->bones.size()); i++) {
    BoneCTOR & srcBone = o->bones[i];
    for (int j = 0; j < BONE_VO_SIZE; j++) {
      boneData.push_back(-1);
    }
    
    BoneVO * pBoneVO = (BoneVO*) (boneData.data() + boneData.size() - BONE_VO_SIZE);
    BoneVO & outBone = *pBoneVO;
    
    outBone.numConnections = int(srcBone.cells.size());
    outBone.positionsSet = 0;
    
    for (int k = 0; k < outBone.numConnections; k++) {
      outBone.connectedCells[k] = cellsToIndex[srcBone.cells[k]];
    }
  }
  
  /*
   // add connections
   for (int i = 0; i < o->connections.size(); i++) {
   ConnectionCTOR c = o->connections[i];
   
   connectionFromIndices.push_back(cellsToIndex[c.cell1]);
   connectionToIndices.push_back(cellsToIndex[c.cell2]);
   float dist = c.cell1->mPos.distance(c.cell2->mPos);
   
   float minDist = dist;// * .95f; // randRange(dist * .9f, dist)
   float maxDist = dist;// * 1.05f; // randRange(dist, dist * 1.1f)
   connectionMinDistances.push_back(minDist);
   connectionMaxDistances.push_back(maxDist);
   }
   
   
   for (int i = 0; i < (cellData.size() / CELL_VO_SIZE); i++) {
   ConnectionCTOR c = o->connections[i];
   CellVO * pCellVO = (CellVO*) (cellData.data() + i * CELL_VO_SIZE);
   printf("%f\n", pCellVO->x);
   }
   */
}

void WorldData :: beginCreatureConstruction() {
  //  prepareForCollision(*this);
}

void WorldData :: endCreatureConstruction() {
}
#endif


void WorldData :: copyToDevice() {
  if (! USE_GPU) {
    return;
  }
  this->settingsData.copyToDevice();
  for (size_t i = 0; i < allArrays.size(); i++) {
    allArrays[i]->copyToDevice();
  }
}

void WorldData :: copyFromDevice() {
  if (! USE_GPU) {
    return;
  }
  for (size_t i = 0; i < allArrays.size(); i++) {
    allArrays[i]->copyFromDevice();
  }
}

int frame = 0;

void WorldData::copyToRenderer() {
  
  if (! Renderer::isInited()) {
    return;
  }
  return;
  /*
   ++frame;
   if (frame % 2) {
   return;
   }
   */
#if USE_GPU
  //copyFromDevice();
  cellData.copyFromDevice();
#endif
  
  RenderLock rl;
  
  Renderer & r = Renderer::instance();
  r.clearRenderList();
  
  float focusX = 0, focusY = 0;
  int numFocusCells = 0;
  
  CellVO * pCellVO = (CellVO*) cellData.data();
  int numCells = cellData.size() / CELL_VO_SIZE;
  
  ConnectionVO * pConnectionVO = (ConnectionVO*) connectionData.data();
  int numConnections = connectionData.size() / CONNECTION_VO_SIZE;
  
  for (int i = 0; i < numConnections; i++) {
    int iFrom = pConnectionVO[i].iCellFrom;
    if (iFrom < 0) {
      break;
    }
    
    int iTo = pConnectionVO[i].iCellTo;
    r.addLine(pCellVO[iFrom].x, pCellVO[iFrom].y, pCellVO[iTo].x, pCellVO[iTo].y, Color(1,1,0,1), 1);
  }
  
  for (int i = 0; i < numCells; i++) {
    
    float cx = pCellVO[i].x;
    if (cx == -1) {
      break;
    }
    float cy = pCellVO[i].y;
    
    if (pCellVO[i].organismIndex == Globals::focusedCritter) {
      ++numFocusCells;
      focusX += cx;
      focusY += cy;
    }
    
//    if (pCellVO[i].activationDuration > 0) {
//      float x2 = cx + pCellVO[i].actionX * 20.0f;
//      float y2 = cy + pCellVO[i].actionY * 20.0f;
//      
//      r.addLine(cx, cy, x2, y2, Color(0,0,1,1), 2);
//    }
    
    r.addCircle(cx, cy, CELL_SIZE / 2.0f, Color(1,1,1,1), 1, Color(1,1,1,1));
    
#if 0
    /*
     if (cellActivationDuration[i] > 0) {
     float x2 = cx + cellActionX[i] * 20.0f;
     float y2 = cy + cellActionY[i] * 20.0f;
     
     r.addLine(cx, cy, x2, y2, Color(0,0,1,1), 2);
     }
     */
  }
  
  float focusX = 0;
  float focusY = 0;
  int focusCount = 0;
  
  for (int i = 0; i < organismStartConnectionIndex.size(); i++) {
    int index = organismStartConnectionIndex[i];
    int length = organismConnectionLength[i];
    while (length-- > 0) {
      int from = connectionFromIndices[index];
      int to = connectionToIndices[index];
      ++index;
      
      float x1 = cellPosX[from];
      float y1 = cellPosY[from];
      float x2 = cellPosX[to];
      float y2 = cellPosY[to];
      
      //      r.addLine(x1, y1, x2, y2, (i == Globals::focusedCritter) ? Color(1,0,1,1) : Color(1,1,0,1), 1);
      
      
      if (i == Globals::focusedCritter) {
        focusX += x1 + x2;
        focusY += y1 + y2;
        focusCount += 2;
      }
    }
  }
  
  float critterX = focusX / float(focusCount);
  float critterY = focusY / float(focusCount);
  
  Globals::focusX = critterX;
  Globals::focusY = critterY;
#endif
  
}

if (numFocusCells) {
  Globals::focusX = focusX / float(numFocusCells);
  Globals::focusY = focusY / float(numFocusCells);
}
}

