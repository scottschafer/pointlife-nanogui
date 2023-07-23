kernel void applyPointPhysics(
                              __global float* cellData,
                              __global float* settingsData
                              ) {
  int i = get_global_id(0);
  
  CellVO * pCellVO = (CellVO*) (cellData + i * CELL_VO_SIZE);
  if (pCellVO->connectionCount < 0) {
    return;
  }

  SettingsVO * pSettings = (SettingsVO*) (settingsData);
  float maxVelocity = pSettings->maxVelocity;
  float wallDamper = pSettings->wallDamper;

  float x = pCellVO->x;
  float y = pCellVO->y;

  float vx = pCellVO->velX *= pSettings->velocityDamper;
  float vy = pCellVO->velY *= pSettings->velocityDamper;
  //float vx = pCellVO->velX;
  //float vy = pCellVO->velY;

  float squaredDist = vx * vx + vy * vy;
  
  if (squaredDist > maxVelocity * maxVelocity) {
    //if (i == 0) printf("MAX_VELOCITY = %f, squaredDist = %f, adjusting\n", MAX_VELOCITY, squaredDist);
    
    float dist = sqrt(squaredDist);
    float scale = maxVelocity / dist;
    vx *= scale;
    vy *= scale;
    //if (i == 0) printf("scale = %f, scaled velX = %f\n", scale, vx);
  }
  VALIDATE_VELOCITY(vx);
  VALIDATE_VELOCITY(vy);
  
  x += vx;
  y += vy;

  float threshhold = CELL_SIZE * .1;//4;
  
  if (x < threshhold) {
    if (pCellVO->velX < 0) {
      pCellVO->velX = -pCellVO->velX * wallDamper;
      x += pCellVO->velX;
    }
    else {
      x = threshhold;
    }
  }
  
  if (x > (WORLD_DIM - threshhold)) {
    if (pCellVO->velX > 0) {
      pCellVO->velX = -pCellVO->velX * wallDamper;
      x += pCellVO->velX;
    }
    else {
      x = WORLD_DIM - threshhold;
    }
  }
  
  if (y < threshhold) {
    if (pCellVO->velY < 0) {
      pCellVO->velY = -pCellVO->velY * wallDamper;
      y += pCellVO->velY;
    }
    else {
      y = threshhold;
    }
  }
  if (y > (WORLD_DIM - threshhold)) {
    if (pCellVO->velY > 0) {
      pCellVO->velY = -pCellVO->velY * wallDamper;
      y += pCellVO->velY;
    }
    else {
      y = WORLD_DIM - threshhold;
    }
  }
  VALIDATE_COORD(x);
  VALIDATE_COORD(y);
  
  pCellVO->x = x;
  pCellVO->y = y;
  
  
  
    // apply wheel force

//  if (pCellVO->vertexId == 2) {
//
//    CellVO * pCellR = (CellVO*) (cellData + pCellVO->connections[0] * CELL_VO_SIZE);
//    CellVO * pCellL = (CellVO*) (cellData + pCellR->connections[0] * CELL_VO_SIZE);
//
//    float xR = pCellR->x;
//    float yR = pCellR->y;
//
//    float xL = pCellL->x;
//    float yL = pCellL->y;
//
//    float vx = (xR + xL) / 2.0f - x;
//    float vy = (yR + yL) / 2.0f - y;
//
//    float scale = 0.01f;
//    float scaleL = pCellVO->localData[WHEEL_LEFT] * scale;
//    float scaleR = pCellVO->localData[WHEEL_RIGHT] * scale;
//
////    pCellR->velX -= (xR - x) * scaleR;
////    pCellR->velY -= (yR - y) * scaleR;
////
////    pCellL->velX -= (xL - x) * scaleL;
////    pCellL->velY -= (yL - y) * scaleL;
//
//    pCellR->velX -= vx * scaleR;
//    pCellR->velY -= vy * scaleR;
//
//    pCellL->velX -= vx * scaleL;
//    pCellL->velY -= vy * scaleL;
//
//
//    //pCellVO->velX += (vxA + vxB) * scale;
//    //pCellVO->velY += (vyA + vyB) * scale;
//  }

  if (pCellVO->vertexId == 2) {

    float maxDim = (float) WORLD_DIM - 1.0f;
    float cosa = cos(2.094395102393195f);
    float sina = sin(2.094395102393195f);
    
    float CENTER_DIST = CELL_SIZE / 2.0f; // * sqrtf(3.0f) / 2.0f;

    // A is top, B and C rotated 120 degrees clockwise
    CellVO * pCellC = pCellVO;
    CellVO * pCellB = pCellC - 1;
    CellVO * pCellA = pCellC - 2;
    
    float xA = pCellA->x;
    float xB = pCellB->x;
    float xC = pCellC->x;
    
    float yA = pCellA->y;
    float yB = pCellB->y;
    float yC = pCellC->y;
    
    float scale;

    float cx = (xA + xB + xC) / 3.0f;
    float cy = (yA + yB + yC) / 3.0f;

    float dx = xA - cx;
    float dy = yA - cy;

    float dist = sqrt(dx*dx + dy*dy);
//    if (dist > 0.0001f) {
//      float scale = CENTER_DIST / dist;
//
//      dx *= scale;
//      dy *= scale;
//
//      pCellA->x = fmin(fmax(cx + dx, 0),maxDim);
//      pCellA->y = fmin(fmax(cy + dy, 0),maxDim);
//      VALIDATE_COORD(pCellA->x);
//      VALIDATE_COORD(pCellA->y);
//
//      float ndx = dx * cosa - dy * sina;
//      float ndy = dx * sina + dy * cosa;
//
//      pCellB->x = fmin(fmax(cx + ndx, 0),maxDim);
//      pCellB->y = fmin(fmax(cy + ndy, 0),maxDim);
//      VALIDATE_COORD(pCellB->x);
//      VALIDATE_COORD(pCellB->y);
//
//      dx = ndx * cosa - ndy * sina;
//      dy = ndx * sina + ndy * cosa;
//
//      pCellC->x = fmin(fmax(cx + dx, 0),maxDim);
//      pCellC->y = fmin(fmax(cy + dy, 0),maxDim);
//      VALIDATE_COORD(pCellC->x);
//      VALIDATE_COORD(pCellC->y);
//    }
    
    // apply wheel force
    float x = xA, y = yA;
    CellVO * pCellR = pCellB;
    CellVO * pCellL = pCellC;

    float xR = pCellR->x;
    float yR = pCellR->y;

    float xL = pCellL->x;
    float yL = pCellL->y;

    scale = 0.01f;
    float scaleL = pCellA->localData[WHEEL_LEFT] * scale;
    float scaleR = pCellA->localData[WHEEL_RIGHT] * scale;

#if 1
    float vx = (xR + xL) / 2.0f - x;
    float vy = (yR + yL) / 2.0f - y;
    pCellR->velX -= vx * scaleR;
    pCellR->velY -= vy * scaleR;

    pCellL->velX -= vx * scaleL;
    pCellL->velY -= vy * scaleL;
    
    pCellA->velX = pCellR->velX + pCellL->velX;
    pCellA->velY = pCellR->velY + pCellL->velY;
//    pCellVO->velX -= (vx * scaleR + vx * scaleL) / 1.0f;
//    pCellVO->velY -= (vy * scaleR + vy * scaleL) / 1.0f;
#else
    pCellR->velX -= (xR - x) * scaleR;
    pCellR->velY -= (yR - y) * scaleR;

    pCellL->velX -= (xL - x) * scaleL;
    pCellL->velY -= (yL - y) * scaleL;

//    pCellVO->velX = pCellR->velX + pCellL->velX;
//    pCellVO->velY = pCellR->velY + pCellL->velY;

#endif

    //pCellVO->velX += (vxA + vxB) * scale;
    //pCellVO->velY += (vyA + vyB) * scale;
  }

return;
  
  
  // now adjust to the first connection
  
  CellVO * pCells = (CellVO*) cellData;
  int toIndex = pCellVO->connections[0];
  int fromIndex = i;
  
    float x1 = x;//pCells[fromIndex].x;
    float y1 = y;//pCells[fromIndex].y;
  
    float x2 = pCells[toIndex].x;
    float y2 = pCells[toIndex].y;
  
    float xd = x1 - x2;
    float yd = y1 - y2;
    float distanceSquared = (xd * xd + yd * yd);
  
    if (distanceSquared == 0) {
      distanceSquared = CELL_SIZE / 10000.0f;
      distanceSquared *= distanceSquared;
    }
  
    float minDist = CELL_SIZE; //pConnection->minDist;
    float maxDist = CELL_SIZE; //pConnection->maxDist;
  
    if (maxDist < minDist) {
      maxDist = minDist;
    }
    float targestDistance = 0;
    if (distanceSquared < minDist*minDist) {
      targestDistance = minDist;
    }
    else if (distanceSquared > maxDist*maxDist) {
      targestDistance = maxDist;
    }
  
    if (targestDistance) {
      float distance = sqrt(distanceSquared);
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
      
      pCells[fromIndex].x = nx1;
      pCells[fromIndex].y = ny1;
      
      pCells[toIndex].x = nx2;
      pCells[toIndex].y = ny2;
      
      pCells[fromIndex].velX += nx1 - x1;// + vadj;
      pCells[fromIndex].velY += ny1 - y1;// + vadj;
      
      pCells[toIndex].velX += nx2 - x2;// + vadj;
      pCells[toIndex].velY += ny2 - y2;// + vadj;
    }
  
}
