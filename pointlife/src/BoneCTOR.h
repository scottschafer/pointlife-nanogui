//
//  BoneCTOR.h
//  ClusterBotzGPU
//
//  Created by Scott Schafer on 5/22/17.
//  Copyright © 2017 Scott Schafer. All rights reserved.
//

#ifndef BoneCTOR_h
#define BoneCTOR_h

#include "CellCTOR.h"
#include <vector>

namespace cbtz {

  class BoneCTOR {
  public:
    std::vector<CellCTOR*> cells;
  };
};

#endif /* BoneCTOR_h */
