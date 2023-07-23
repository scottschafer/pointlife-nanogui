//
//  mCellCTOR.h
//  ClusterBotz
//
//  Created by Scott Schafer on 4/20/17.
//  Copyright © 2017 Scott SSE. All rights reserved.
//

#pragma once

#include "Point.h"
#include "Color.h"
#include "constants.h"

#ifndef VO
#include "vo.h"
#endif

namespace cbtz {

  struct OrganismCTOR;

  typedef struct CellCTOR {

    OrganismCTOR * mOrganism;
    Point mPos;
    
    int mNumConnections;
    CellCTOR* mConnectedCellCTORs[MAX_CONNECTIONS];
    
    CellVO mVO;
    /*
    int mAction;
    int mParam;
    int mActionFrequency;
    int mActionPhase;
    int mInactiveCount;
    
    //int mDefaultPhase;
    
    int mLastCollision;
    int mLastFlagellum;
    int mLastImpulse;
    int mLastAte;
    int mLastSentSignal;
    int mContractTurnCount;
    
    Color color;
    */
    
  } CellCTOR, * CellCTORPtr;


  inline void reset(CellCTOR * pCellCTOR) {
  }
  
  inline bool isConnected(CellCTOR * pCellCTOR1, CellCTOR * pCellCTOR2) {
    for (int i = 0; i < pCellCTOR1->mNumConnections; i++) {
      if (pCellCTOR1->mConnectedCellCTORs[i] == pCellCTOR2) {
        return true;
      }
    }
    return false;
  };
};
