#include <cstdlib>
#include <fstream>
#include <string>
#include <time.h>

#include "../../EasyCL/EasyCL.h"
#include "../../EasyCL/CLKernel.h"
#include "../../EasyCL/CLKernel_structs.h"

#include "../constants.h"
#include "../KernelCaller.h"

#include "applyConnectionPhysics.h"
#include "../WorldData.h"

using namespace std;
using namespace easycl;

extern EasyCL * cl;
static CLKernel * kernel_applyConnectionPhysics = NULL;


#define STRINGIFY(TOSTRINGIFY) #TOSTRINGIFY

const char * src_applyConnectionPhysics = "";
//#include "applyConnectionPhysics.cl"

static CLKernel * getKernel_applyConnectionPhysics() {
  if (! kernel_applyConnectionPhysics) {
    kernel_applyConnectionPhysics = cl->buildKernelFromString(
                                                         getHeaders() + src_applyConnectionPhysics,
                                                         "applyConnectionPhysics", "");
  }
  return kernel_applyConnectionPhysics;
}

void applyConnectionPhysics(WorldData & data) {
  int numCells = int(data.cellData.size()) / CELL_VO_SIZE;
  BEGIN_CALL_FUNCTION(applyConnectionPhysics, numCells)
    ADD_INOUT_PARAM(data.cellData) AND
    ADD_IN_PARAM(data.settingsData)
  END_CALL_FUNCTION(numCells,  WORKGROUP_SIZE);
}
