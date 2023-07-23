#define kernel inline
#define global
#define __global
#define get_global_id(X) globalId

extern int globalId;

#include "../constants.h"
#include <math.h>

#define sqrt sqrtf
#define max(x,y) std::max(x,y)
#define min(x,y) std::min(x,y)

#define CPU 1
float VALIDATE_COORD(float x);
float VALIDATE_VELOCITY(float x);

std::string getHeaders();
