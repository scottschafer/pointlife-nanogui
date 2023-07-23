#if 0
#include <cstdlib>
#include <fstream>
#include <string>
#include <time.h>

#include "../../EasyCL/EasyCL.h"
#include "../../EasyCL/CLKernel.h"
#include "../../EasyCL/CLKernel_structs.h"
#include "../constants.h"

#include "applyBones.h"
#include "../WorldData.h"
#include "../KernelCaller.h"

using namespace std;
using namespace easycl;

extern EasyCL * cl;
static CLKernel * kernel_applyBones = NULL;

#define STRINGIFY(TOSTRINGIFY) #TOSTRINGIFY

const char * src_applyBones = ""; // TODO


static CLKernel * getKernel_applyBones() {
  if (! kernel_applyBones) {
    kernel_applyBones = cl->buildKernelFromString(
                                                         getHeaders() + src_applyBones,
                                                         "applyBones", "");
  }
  return kernel_applyBones;
}


void applyBones(WorldData & data) {

//return; /// FOR NOW

  int numBones = int(data.boneData.size()) / BONE_VO_SIZE;
  int numCells = int(data.cellData.size()) / CELL_VO_SIZE;

  applyBones((float*)1, (float*)2, (float*) 3);
  
  BEGIN_CALL_FUNCTION(applyBones, numBones)
    ADD_IN_PARAM(data.boneData) AND
    ADD_IN_PARAM(data.cellData) AND
    ADD_IN_PARAM(data.settingsData)
  
  END_CALL_FUNCTION(numBones,  WORKGROUP_SIZE);
}
#endif
