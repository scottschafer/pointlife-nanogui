kernel void applyConnectionPhysics(
                                   __global float* organismData,
                                   __global float* connectionData,
                                   __global float* cellData,
                                   __global float* settingsData
                                   ) {
  
  SettingsVO * pSettings = (SettingsVO*) (settingsData);
  int iOrg = get_global_id(0);
  
  OrganismVO * pOrganisms = (OrganismVO*) organismData;
  ConnectionVO * pConnections = (ConnectionVO*) connectionData;
  CellVO * pCells = (CellVO*) cellData;
  
  OrganismVO * pOrganism = &pOrganisms[iOrg];
  int i = pOrganism->iStartConnection;
  if (i < 0) return;
  int remaining = pOrganism->numConnections;
  
  ConnectionVO * pConnection = &pConnections[i];
  
  float vadj = .0001f;
  
  while (remaining--) {
    
    int fromIndex = pConnection->iCellFrom;
    int toIndex = pConnection->iCellTo;
    
    if (fromIndex < 0 || toIndex < 0) return;
    
    
    float x1 = pCells[fromIndex].x;
    float y1 = pCells[fromIndex].y;
    
    float x2 = pCells[toIndex].x;
    float y2 = pCells[toIndex].y;
    
    float xd = x1 - x2;
    float yd = y1 - y2;
    float distanceSquared = (xd * xd + yd * yd);
    
    if (distanceSquared == 0) {
      distanceSquared = CELL_SIZE / 10000.0f;
      distanceSquared *= distanceSquared;
    }
    
    float minDist = pConnection->minDist;
    float maxDist = pConnection->maxDist;
    
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
    
    ++pConnection;
  }
};


