#include <cstdlib>
#include <fstream>
#include <string>
#include <time.h>

#include "../../EasyCL/EasyCL.h"
#include "../../EasyCL/CLKernel.h"
#include "../../EasyCL/CLKernel_structs.h"

#include "../constants.h"

#include "applyPointPhysics.h"
#include "../WorldData.h"
// #include "../VO_size.h"
#include "../KernelCaller.h"
#include "../getOpenCLCode.h"

using namespace std;
using namespace easycl;

extern EasyCL * cl;
static CLKernel * kernel_applyPointPhysics = NULL;

const std::string src_applyPointPhysics = getOpenCLCode("applyPointPhysics");

static CLKernel * getKernel_applyPointPhysics() {
  if (! kernel_applyPointPhysics) {
    kernel_applyPointPhysics = cl->buildKernelFromString(
                                                         getHeaders() + src_applyPointPhysics,
                                                         "applyPointPhysics", "");
  }
  return kernel_applyPointPhysics;
}

void applyPointPhysics(WorldData & data) {

  //data.cellDataToRender.resize(1024);
  int numCells = int(data.cellData.size()) / CELL_VO_SIZE;

  CellVO * pCell = (CellVO *) data.cellData.data();
  BEGIN_CALL_FUNCTION(applyPointPhysics, numCells)
    ADD_IN_PARAM(data.cellData) AND
    ADD_IN_PARAM(data.settingsData)
  END_CALL_FUNCTION(numCells,  WORKGROUP_SIZE);
}
