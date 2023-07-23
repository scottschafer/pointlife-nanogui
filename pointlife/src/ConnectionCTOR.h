//
//  mConnection.h
//  ClusterBotz
//
//  Created by Scott Schafer on 4/20/17.
//  Copyright © 2017 Scott SSE. All rights reserved.
//

#pragma once
#include "CellCTOR.h"

namespace cbtz {
  
  
  typedef struct ConnectionCTOR {
    
    CellCTOR *cell1, * cell2;
    
    float minDist;
    float maxDist;
    int options;
    float expandTurnCount;
    float length;
    
    void reset() {
      options = 0;
      expandTurnCount = 0;
      cell1 = cell1 = NULL;
      minDist = maxDist = CELL_SIZE;
    }
    
    bool isConnected(CellCTOR * pCell) {
      return cell1 == pCell || cell2 == pCell;
    }
    
    CellCTOR * getToCell(CellCTOR * pFromCell) {
      return (pFromCell == cell1) ? cell2 : cell1;
    }
    
  } Connection;
  
  
  /*
  inline void connect(Connection * pConnection, Cell * pCell1, Cell * pCell2, float distance, int options = 0) {
    pConnection->reset();
    pConnection->cell1 = pCell1;
    pConnection->cell2 = pCell2;
    pConnection->length = pConnection->minDist = distance;
    pConnection->options = options;
  }
   */
};