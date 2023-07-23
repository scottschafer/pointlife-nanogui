#ifndef CELL_VO
#include "CellVO.h"
#endif

#ifndef VO
#define VO
/**
Types of cells:
 **/

typedef struct ConnectionVO {
  int iCellFrom;
  int iCellTo;
  float minDist;
  float maxDist;
} ConnectionVO;

typedef struct OrganismVO {
  int iStartConnection;
  int numConnections;
  float energy;
} OrganismVO;

typedef struct BoneVO {
  int numConnections;
  int positionsSet;
  float length;
  int connectedCells[MAX_BONE_CONNECTIONS];
  float cellX[MAX_BONE_CONNECTIONS];
  float cellY[MAX_BONE_CONNECTIONS];
  float renderX[MAX_BONE_CONNECTIONS];
  float renderY[MAX_BONE_CONNECTIONS];
} BoneVO;


typedef struct SettingsVO {
  float maxVelocity;
  float velocityDamper;
  float wallDamper;
  float centerPressure;
  float moveDuration;
  float moveForce;
} SettingsVO;

void setDefaults(SettingsVO * pSetting);
#endif
