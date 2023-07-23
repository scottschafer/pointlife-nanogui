#pragma once

#include "CellCTOR.h"
#include "ConnectionCTOR.h"
#include "BoneCTOR.h"
#include "OrganismCTOR.h"
#include <vector>
#include <map>

namespace cbtz {

  // a create-time organism that is translated to a runtime version
  class OrganismCTOR {

  public:
    std::vector<CellCTOR*> cells;
    std::vector<BoneCTOR> bones;
    std::vector<ConnectionCTOR> connections;


    ~OrganismCTOR() { free(); }
  
    void free() {
      for (int i = 0 ; i < cells.size(); i++) {
        delete cells[i];
      }
      cells.resize(0);
      bones.resize(0);
    }
  
  
    void addConnection(CellCTOR * pCell1, CellCTOR * pCell2, float distance, int options = 0) {
      
      if (! pCell1 || ! pCell2) {
        return;
      }
      Connection c;
      c.reset();
      c.cell1 = pCell1;
      c.cell2 = pCell2;
      
      if (! distance) {
        distance = pCell1->mPos.distance(pCell2->mPos);
      }
      
      c.length = c.minDist = distance;
      c.options = options;
      
      for (int i = 0; i < connections.size(); i++) {
        CellCTOR * a = connections[i].cell1;
        CellCTOR * b = connections[i].cell2;
        if ((a == pCell1 && b == pCell2) || (b == pCell1 && a == pCell2)) {
          continue;
        }
      }
      connections.push_back(c);
      
      if (pCell1->mNumConnections < MAX_CONNECTIONS) {
        pCell1->mConnectedCellCTORs[pCell1->mNumConnections++] = pCell2;
      }
      if (pCell2->mNumConnections < MAX_CONNECTIONS) {
        pCell2->mConnectedCellCTORs[pCell2->mNumConnections++] = pCell1;
      }
    }
  };
};