kernel void eraseCollisionGrid(__global int * collisionGrid) {
  collisionGrid[get_global_id(0)] = -1;
}
