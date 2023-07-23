

/**
 * Apply cell actions
 **/
kernel void applyBones(
                       __global float* boneData,
                       __global float* cellData,
                       __global float* settingsData
                       ) {
  int iBone = get_global_id(0);
  
  CellVO * pCells = (CellVO*) cellData;
  BoneVO * pBone = (BoneVO*) (boneData + iBone * BONE_VO_SIZE);
  SettingsVO * pSettings = (SettingsVO*) (settingsData);
  
  int numBoneCells = pBone->numConnections;
  if (numBoneCells <= 0) return;
  
  // first determine the best fit line for all the cell points
  // float _slope;
  // float _yInt;
  
  float sumX=0;
  float sumY=0;
  float sumXY=0;
  float sumX2=0;
  
  float x;
  float y;
  float xd;
  float yd;
  
  for (int i = 0; i < numBoneCells; i++) {
    int iCell = pBone->connectedCells[i];
    
    sumX += pCells[iCell].x;
    sumY += pCells[iCell].y;
    sumXY += pCells[iCell].x * pCells[iCell].y;
    sumX2 += pCells[iCell].x * pCells[iCell].x;
  }
  
  float xMean = sumX / float(numBoneCells);
  // float yMean = sumY / float(numBoneCells);
  // float denominator = sumX2 - sumX * xMean;
  
  /*
   if ((denominator < 0 ? -denominator : denominator) < 1e-7f) {
   //pBone->mStart.set(0,0);
   //pBone->mEnd.set(0,0);
   return; //it seems a vertical line
   }
   */
  
  // _slope = (sumXY - sumX * yMean) / denominator;
  // _yInt = yMean - _slope * xMean;
  
  CellVO * pFirstCell = &pCells[pBone->connectedCells[0]];
  CellVO * pLastCell = &pCells[pBone->connectedCells[numBoneCells - 1]];
  
  float vx = pLastCell->x - pFirstCell->x;
  float vy = pLastCell->y - pFirstCell->y;
  
  // calc the center point from the bone
  float xc = sumX / float(numBoneCells);
  float yc = sumY / float(numBoneCells);
  
  // now determine the sin and cos from the line
  float c = sqrt(vx * vx + vy * vy);
  float sinA;
  float cosA;
  
  sinA = vy / c;
  cosA = vx / c;
  
  // if we haven't set unrotated the bone cell positions, do it now
  int adjCells = pBone->positionsSet;
  
  if (! pBone->positionsSet) {
    pBone->positionsSet = 1;
    
    xd = pFirstCell->x - pLastCell->x;
    yd = pFirstCell->y - pLastCell->y;
    pBone->length = sqrt(xd*xd + yd*yd);
    
    float angle = asin(sinA);
    cosA = cos(-angle);
    sinA = sin(-angle);
    
    for (int i = 0; i < numBoneCells; i++) {
      int iCell = pBone->connectedCells[i];
      
      pBone->renderX[i] = pCells[iCell].x;
      pBone->renderY[i] = pCells[iCell].y;
      
      float x = pCells[iCell].x - xc; // relative to center
      float y = pCells[iCell].y - yc;
      
      // we need to rotate in opposite direction from the initial rotation vector.
      pBone->cellX[i] = x * cosA - y * sinA;
      pBone->cellY[i] = y * cosA + x * sinA;
    }
  }
  else {
    for(int i=0; i<numBoneCells; i++) {
      float x = pBone->cellX[i];
      float y = pBone->cellY[i];
      
      pBone->renderX[i] = xc + x * cosA - y * sinA;
      pBone->renderY[i] = yc + y * cosA + x * sinA;
    }
  }
  
  float boneLength = pBone->length;
  
  x = xc - (boneLength/2) * cosA;
  y = yc - (boneLength/2) * sinA;
  
  xd = (boneLength / float(numBoneCells)) * cosA;
  yd = (boneLength / float(numBoneCells)) * sinA;
  
  if (adjCells) {
    for (int j = 0; j < numBoneCells; j++) {
      
      int iCell = pBone->connectedCells[j];
      CellVO * pCell = &pCells[iCell];
      
      float bx = pBone->renderX[j];
      float by = pBone->renderY[j];
      
      float scale = 0.5f;
      
      float vx = scale * (bx - pCell->x);
      float vy = scale * (by - pCell->y);
      
      float d = vx * vx + vy * vy;
      if (d > 0 && d < CELL_SIZE) {
        pCell->velX += vx;
        pCell->velY += vy;
      }
      
      //pCell->velX += vx * scale;
      //pCell->velY += vy * scale;
    }
  }
}
