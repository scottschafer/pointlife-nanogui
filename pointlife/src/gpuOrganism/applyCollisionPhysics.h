#include "core/_inc/VO_size.h"
#include "core/_inc/CellTypes.h"

#include "_prefix.h"
#include "core/applyCollisionPhysics_cl.h"
#include "core/eraseCollisionGrid_cl.h"
#include "core/prepareCollisionGrid_cl.h"
#include "_postfix.h"

class WorldData;

void prepareForCollision(WorldData &);
int getNearbyCells(WorldData &, float x, float y, float dist, int maxResults, int * pBuffer);
bool hasNearbyCells(WorldData &, float x, float y, float dist);

void applyCollisionPhysics(WorldData &);
