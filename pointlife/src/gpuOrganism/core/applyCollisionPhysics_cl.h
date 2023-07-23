/**
 * Apply collisions for an individual cell
 **/

kernel void applyCollisionPhysics(
                                  __global float* settingsData,
                                  __global int * collisionGrid,
                                  __global float* cellData,
                                  __global float* organismData
                                  ) {
  
  SettingsVO * pSettings = (SettingsVO*) (settingsData);
  OrganismVO * pOrganisms = (OrganismVO*) organismData;
  
  float maxVelocity = pSettings->maxVelocity;
  float maxVelocitySquared = maxVelocity * maxVelocity;
  
  int iCell = get_global_id(0);
  
  CellVO * pCellVOs = (CellVO*) cellData;
  CellVO * pCellVO = &pCellVOs[iCell];
  
  if (pCellVO->connectionCount < 0) return;
  
  float x1 = pCellVO->x;
  float y1 = pCellVO->y;
  float squaredCellDist = CELL_SIZE * CELL_SIZE;
  
  // test for collisions with neighboring cells
  int fx = max(0, int(x1 - CELL_SIZE));
  int fy = max(0, int(y1 - CELL_SIZE));
  int tx = min(WORLD_DIM-1, int(x1 + CELL_SIZE));
  int ty = min(WORLD_DIM-1, int(y1 + CELL_SIZE));
  
  float x2;
  float y2, xd, yd, squaredDist;
  
  float minSquaredDistance = CELL_SIZE / 10.0f;
  
  for (int cy = fy; cy <= ty; cy++) {
    int baseIndex = cy * WORLD_DIM;
    
    for (int cx = fx; cx <= tx; cx++) {
      int iCellInGrid = collisionGrid[baseIndex + cx];
      
      CellVO * pWithCell = 0;
      
      while (iCellInGrid != -1) {
      #if CPU
        if (iCellInGrid < 0 || iCellInGrid >= NUM_CELLS) {
          throw "out of bounds";
        }
      #endif

        if (pWithCell) {
          iCellInGrid = pWithCell->nextInGrid;
        }
        if (iCellInGrid == -1) {
          break;
        }
        pWithCell = &pCellVOs[iCellInGrid];
        
        if (iCellInGrid == iCell) {
          continue; // can't collide with self
        }
        
        x2 = pWithCell->x;
        y2 = pWithCell->y;
        xd = x1 - x2;
        yd = y1 - y2;
        
        squaredDist = xd*xd + yd*yd;
        
        if (squaredDist > squaredCellDist) {
          continue; // Too far away
        }
        
        if (squaredDist < minSquaredDistance) {
          squaredDist = minSquaredDistance;
        }
        
        int isSameOrganism = 0;
        
        if (pCellVO->organismIndex == pWithCell->organismIndex) {
        return;
          isSameOrganism = 1;
          int isConnected = 0;
          
          // if the cell we've collided with is one of our immediate connections, ignore it
//          if (iCellInGrid == pCellVO->connections[0] ||
//            iCellInGrid == pCellVOs[pCellVO->connections[0]].connections[0]) {
//            continue;
//          }
          /*
          for (int k = 0; k < pCellVO->connectionCount; k++) {
            CellVO * pTestCell = &pCellVOs[pCellVO->connections[k]];
            if (pTestCell == pWithCell) {
              isConnected = 1;
              break;
            }
          }
          if (isConnected) {
            continue;
          }
          */
        } else {
          // we've collided with a cell of a different critter.
//          if (pCellVO->actionType == actionBite) {
//            float energyTransfer = (pCellVO->velX * pCellVO->velX + pCellVO->velY * pCellVO->velY) * 20.f + .5f;
//            
//            pOrganisms[pCellVO->organismIndex].energy += energyTransfer;
//            pOrganisms[pWithCell->organismIndex].energy -= energyTransfer;
//            pCellVO->activationDuration = 50;
//          }
        }
        
#if 1
        // swap the velocities of the collided cells
        float vx = pCellVO->velX;
        float vy = pCellVO->velY;
        
        pCellVO->velX = pWithCell->velX;
        pCellVO->velY = pWithCell->velY;
        pWithCell->velX = vx;
        pWithCell->velY = vy;
        
        VALIDATE_VELOCITY(pCellVO->velX);
        VALIDATE_VELOCITY(pCellVO->velY);
        VALIDATE_VELOCITY(pWithCell->velX);
        VALIDATE_VELOCITY(pWithCell->velY);
#endif
        
#if 1
        int isConnected = false;
        // move the cells apart
        float targestDistance = CELL_SIZE;
        if (pCellVO->organismIndex != pWithCell->organismIndex) {
          //targestDistance *= 1.5f;
        }
        else {
          //targestDistance = 0;
          //targestDistance *= 1.1f;
          for (int i = 0; i < pCellVO->connectionCount; i++) {
            if (pCellVO->connections[i] == iCellInGrid) {
              isConnected = true;
              break;
            }
          }
        }
        
        float distance = sqrt(squaredDist);
        if (!isConnected && distance < targestDistance) {
          float fractionAdjust = (targestDistance - distance) / distance;
          fractionAdjust /= 2.0f;
          float maxDim = (float) WORLD_DIM - 1.0f;
          
          float nx1 = x1 + xd * fractionAdjust;
          float nx2 = x2 - xd * fractionAdjust;
          float ny1 = y1 + yd * fractionAdjust;
          float ny2 = y2 - yd * fractionAdjust;
          
          nx1 = fmin(fmax(nx1, 0),maxDim);
          ny1 = fmin(fmax(ny1, 0),maxDim);
          nx2 = fmin(fmax(nx2, 0),maxDim);
          ny2 = fmin(fmax(ny2, 0),maxDim);
          
          float damper = isSameOrganism ? .25f : .45f;
          
          pCellVO->velX += (nx1 - x1) * damper;
          pCellVO->velY += (ny1 - y1) * damper;
          pWithCell->velX += (nx2 - x2) * damper;
          pWithCell->velY += (ny2 - y2) * damper;
          
          pCellVO->x = nx1;
          pCellVO->y = ny1;
          
          pWithCell->x = nx2;
          pWithCell->y = ny2;
        }
        
#endif
        
      }
    }
  }
}

/*
 #if 0
 // if the cell we've collided with is one of our immediate connections, ignore it
 int k = 0;
 for (; k < pCellVO->connectionCount; k++) {
 CellVO * pTestCell = &pCellVOs[pCellVO->connections[k]];
 if (pTestCell == pWithCell) {
 break;
 }
 }
 k = pCellVO->connectionCount;
 
 if (k == pCellVO->connectionCount) {
 float distance = sqrt(squaredDist);
 float targestDistance = CELL_SIZE;
 
 if (distance == 0) {
 printf("! distance\n");
 }
 
 //float collisionAdj = 1.0f;//sqrt(squaredCellDist / squaredDist) / 10.0f;
 
 //float fractionAdjust = (targestDistance - distance) / distance;
 //fractionAdjust /= 2.0f;
 
 float fractionAdjust = CELL_SIZE / (CELL_SIZE + sqrt(squaredDist));
 //fractionAdjust = 1.0f;
 
 //float fractionAdjust = squaredCellDist / (squaredDist+squaredCellDist);
 //fractionAdjust *= .5f;
 
 //fractionAdjust = .5f;
 
 float nx1 = x1 + xd * fractionAdjust;
 float nx2 = x2 - xd * fractionAdjust;
 float ny1 = y1 + yd * fractionAdjust;
 float ny2 = y2 - yd * fractionAdjust;
 
 
 
 // the closer we are, the more of a kick we will give
 //float collisionAdj = sqrt(CELL_SIZE / distance);
 //float collisionAdj = 1.0f;//sqrt(squaredCellDist / squaredDist) / 10.0f;
 //collisionAdj *= .75f;
 
 float collisionAdj = .5f;
 
 
 float cAdj1 = 2.0f / float(pCellVO->connectionCount);
 float cAdj2 = 2.0f / float(pWithCell->connectionCount);
 
 cAdj1 *= cAdj1;
 cAdj2 *= cAdj2;
 
 pCellVO->velX += (nx1 - x1) * cAdj1;
 pCellVO->velY += (ny1 - y1) * cAdj1;
 
 float v2 = pCellVO->velX * pCellVO->velX + pCellVO->velY * pCellVO->velY;
 
 VALIDATE_VELOCITY(pCellVO->velX);
 VALIDATE_VELOCITY(pCellVO->velY);
 
 pWithCell->velX += (nx2 - x2) * cAdj2;
 pWithCell->velY += (ny2 - y2) * cAdj2;
 
 v2 = pWithCell->velX * pWithCell->velX + pWithCell->velY * pWithCell->velY;
 
 VALIDATE_VELOCITY(pWithCell->velX);
 VALIDATE_VELOCITY(pWithCell->velY);
 }
 #endif
 */
