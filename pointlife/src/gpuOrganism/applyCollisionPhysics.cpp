#include <cstdlib>
#include <fstream>
#include <string>
#include <time.h>

#include "../../EasyCL/EasyCL.h"
#include "../../EasyCL/CLKernel.h"
#include "../../EasyCL/CLKernel_structs.h"

#include "../constants.h"

#include "applyCollisionPhysics.h"
#include "../WorldData.h"
#include "../KernelCaller.h"

#include "../getOpenCLCode.h"

using namespace std;
using namespace easycl;

extern EasyCL * cl;

static CLKernel * kernel_eraseCollisionGrid = NULL;
static CLKernel * kernel_prepareCollisionGrid = NULL;
static CLKernel * kernel_applyCollisionPhysics = NULL;

const std::string src_eraseCollisionGrid = getOpenCLCode("eraseCollisionGrid");
const std::string src_prepareCollisionGrid = getOpenCLCode("prepareCollisionGrid");
const std::string src_applyCollisionPhysics = getOpenCLCode("applyCollisionPhysics");

static CLKernel * getKernel_eraseCollisionGrid() {
  if (! kernel_eraseCollisionGrid) {
    kernel_eraseCollisionGrid = cl->buildKernelFromString(
                                                         getHeaders() + src_eraseCollisionGrid,
                                                         "eraseCollisionGrid", "");
  }
  return kernel_eraseCollisionGrid;
}


static CLKernel * getKernel_prepareCollisionGrid() {
  if (! kernel_prepareCollisionGrid) {
    kernel_prepareCollisionGrid = cl->buildKernelFromString(
                                                         getHeaders() + src_prepareCollisionGrid,
                                                         "prepareCollisionGrid", "");
  }
  return kernel_prepareCollisionGrid;
}


static CLKernel * getKernel_applyCollisionPhysics() {
  if (! kernel_applyCollisionPhysics) {
    kernel_applyCollisionPhysics = cl->buildKernelFromString(
                                                         getHeaders() + src_applyCollisionPhysics,
                                                         "applyCollisionPhysics", "");
  }
  return kernel_applyCollisionPhysics;
}


void applyCollisionPhysics(WorldData & data) {

  int numGridCells = (int) data.collisionGrid.size();
  int numCells = int(data.cellData.size()) / CELL_VO_SIZE;
  
  // prepare the collision grid and cells
  BEGIN_CALL_FUNCTION(eraseCollisionGrid, numGridCells)
    ADD_IN_PARAM(data.collisionGrid)
  END_CALL_FUNCTION(numGridCells, WORKGROUP_SIZE);

  BEGIN_CALL_FUNCTION(prepareCollisionGrid, numCells)
    ADD_IN_PARAM(data.collisionGrid) AND
    ADD_IN_PARAM(data.cellData)
  END_CALL_FUNCTION(numCells,  WORKGROUP_SIZE);

  // apply collisions
  BEGIN_CALL_FUNCTION(applyCollisionPhysics, numCells)
    ADD_IN_PARAM(data.settingsData) AND
    ADD_IN_PARAM(data.collisionGrid) AND
    ADD_IN_PARAM(data.cellData) AND
    ADD_IN_PARAM(data.organismData)
  END_CALL_FUNCTION(numCells,  WORKGROUP_SIZE);
}
