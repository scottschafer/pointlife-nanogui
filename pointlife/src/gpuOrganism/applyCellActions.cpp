#if 0
#include <cstdlib>
#include <fstream>
#include <string>
#include <time.h>

#include "../../EasyCL/EasyCL.h"
#include "../../EasyCL/CLKernel.h"
#include "../../EasyCL/CLKernel_structs.h"
#include "../constants.h"

#include "applyCellActions.h"
#include "../WorldData.h"
#include "../KernelCaller.h"

using namespace std;
using namespace easycl;

extern EasyCL * cl;
static CLKernel * kernel_applyCellActions = NULL;

#define STRINGIFY(TOSTRINGIFY) #TOSTRINGIFY

const char * src_applyCellActions = ""; // TODO
//#include "applyCellActions.cl"


static CLKernel * getKernel_applyCellActions() {
  if (! kernel_applyCellActions) {
    kernel_applyCellActions = cl->buildKernelFromString(
                                                         getHeaders() + src_applyCellActions,
                                                         "applyCellActions", "");
  }
  return kernel_applyCellActions;
}


void applyCellActions(WorldData & data) {

  int numCells = int(data.cellData.size()) / CELL_VO_SIZE;

  BEGIN_CALL_FUNCTION(applyCellActions, numCells)
    ADD_IN_PARAM(data.turn) AND
    ADD_IN_PARAM(data.cellData) AND
    ADD_IN_PARAM(data.settingsData)
  END_CALL_FUNCTION(numCells,  WORKGROUP_SIZE);
}
#endif

