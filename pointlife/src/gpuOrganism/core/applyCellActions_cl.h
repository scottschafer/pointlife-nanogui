
inline void fireSignal(__global float* cellData, int i) {
  
  // fire a signal from this cell to other cells
  CellVO * pCellVOs = (CellVO*) cellData;
  CellVO * pCellVO = (CellVO*) (cellData + i * CELL_VO_SIZE);
  for (int j = 0; j < pCellVO->connectionCount; j++) {
    int toConnection = pCellVO->connections[j];
    CellVO * pToCellVO = &pCellVOs[toConnection];
    
    switch (pCellVO->signalResponses[j]) {
        
      case sigIncrement: {
        ++pToCellVO->counter;
        break; }
        
      case sigDecrement: {
        pToCellVO->counter = max(0, pToCellVO->counter - 1);
        break; }
        
      case sigToggle: {
        pToCellVO->counter = pToCellVO->counter ? 0 : 1;
        break; }
        
      case sigClear: {
        pToCellVO->counter = 0;
        break; }
        
      case sigRebroadcast: {
        //fireSignal(cellData, toConnection, rebroadcastCount + 1);
        break; }
    }
  }
}

/**
 * Apply cell actions
 **/
kernel void applyCellActions(
                             int turn,
                             __global float* cellData,
                             __global float* settingsData
                             ) {
  int i = get_global_id(0);
  
  SettingsVO * pSettings = (SettingsVO*) (settingsData);
  CellVO * pCellVOs = (CellVO*) cellData;
  CellVO * pCellVO = (CellVO*) (cellData + i * CELL_VO_SIZE);
  if (pCellVO->actionType < 0) return;
  
  
  if (pCellVO->counter > 2) {
    pCellVO->counter = 0;
    pCellVO->wake = 1;
  }
  
  pCellVO->actionX = pCellVO->actionY = -1;
  
  float x = pCellVO->x;
  float y = pCellVO->y;
  float xd = 0;
  float yd = 0;
  
  if (pCellVO->connectionCount <= 2 &&
      (pCellVO->actionType == actionMove || pCellVO->actionType == actionEye)) {
    int numConnections = pCellVO->connectionCount;
    for (int j = 0; j < numConnections; j++) {
      int k = pCellVO->connections[j];
      if (k == -1) {
        printf("WTF");
      }
      
      xd += x - pCellVOs[k].x;
      yd += y - pCellVOs[k].y;
    }
    
    xd *= pSettings->moveForce / float(numConnections);
    yd *= pSettings->moveForce / float(numConnections);
  }
  
  if (pCellVO->activationDuration > 0) {
    --pCellVO->activationDuration;
  }
  
  switch (pCellVO->actionType) {
      
    default: {
      if (pCellVO->wake) {
        pCellVO->wake = 0;
        fireSignal(cellData, i);
      }
      break; }
      
    case actionTimer: {
      int frequency = (pCellVO->actionData + 1) * 4;
      if ((turn % frequency) == 0) {
        fireSignal(cellData, i);
      }
      break; }
      
    case actionEye: {
      break;
    }
      
    case actionMove: {
      if (pCellVO->connectionCount <= 2) {
        if (! pCellVO->activationDuration && pCellVO->wake) {
          pCellVO->wake = 0;
          //if ((turn % (100 + i)) == 0) {
          pCellVO->activationDuration = int(pSettings->moveDuration);
        }
        
        if (pCellVO->activationDuration > 0) {
          xd *= pSettings->moveForce;
          yd *= pSettings->moveForce;
          
          pCellVO->actionX = xd;
          pCellVO->actionY = yd;
          
          pCellVO->velX -= xd;
          pCellVO->velY -= yd;
        }
      }
      break;
    }
  }
}
