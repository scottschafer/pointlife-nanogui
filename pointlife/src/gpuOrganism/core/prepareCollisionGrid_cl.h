/**
 * Apply connections
 **/

kernel void prepareCollisionGrid(
                                 __global int * collisionGrid,
                                 __global float* cellData
                                 ) {
  
  int i = get_global_id(0);
    
  CellVO * pCellVO = (CellVO*) (cellData + i * CELL_VO_SIZE);
  if (pCellVO->connectionCount < 0) return;
  
  int x = (int) pCellVO->x;
  int y = (int) pCellVO->y;
  
  VALIDATE_COORD(x);
  VALIDATE_COORD(y);
  
  int iGrid = y * WORLD_DIM + x;
  
  pCellVO->nextInGrid = collisionGrid[iGrid];
  collisionGrid[iGrid] = i;
}
