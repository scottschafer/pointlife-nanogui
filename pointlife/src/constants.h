//
//  constants.h
//  ClusterBotzGPU
//
//  Created by Scott Schafer on 5/13/17.
//  Copyright © 2017 Scott Schafer. All rights reserved.
//

#ifndef constants_h
#define constants_h

typedef unsigned char BYTE;

#define USE_GPU 0
//#define USE_CELL_STRUCT 1
//#define USE_CELL_CONNECTIONS 1
//#define MIN_CELLS_PER_ORGANISM 64
//#define APPLY_CONNECTIONS_IN_POINT_PHYSICS 1
#define INITIAL_ENERGY 100
//#define WORLD_COMPLEXITY 2

#define NUM_TRIBOTS_SQRT 4
#define NUM_TRIBOTS (NUM_TRIBOTS_SQRT*NUM_TRIBOTS_SQRT)
#define NUM_ORGANISMS NUM_TRIBOTS
#define NUM_CELLS (NUM_TRIBOTS*3)
#define WORLD_DIM (NUM_TRIBOTS_SQRT*8)
#define WORKGROUP_SIZE 512

/*
#if WORLD_COMPLEXITY == -1
#undef MIN_CELLS_PER_ORGANISM
#define MIN_CELLS_PER_ORGANISM 17

  #define NUM_ORGANISMS 4
  #define WORKGROUP_SIZE 32
  #define WORLD_DIM 32
  #define MAX_CELLS_PER_ORGANISM 24
#endif

#if WORLD_COMPLEXITY == 0
  #define NUM_ORGANISMS 1
  #define WORKGROUP_SIZE 1
  #define WORLD_DIM 128
  #define MAX_CELLS_PER_ORGANISM 128
#endif


#if WORLD_COMPLEXITY == 1
  #define NUM_ORGANISMS 32
  #define WORKGROUP_SIZE 256
  #define WORLD_DIM 128
  #define MAX_CELLS_PER_ORGANISM 100
#endif

#if WORLD_COMPLEXITY == 2
  #define NUM_ORGANISMS 64
  // this provides a massive speed increase on the GPU.
  #define WORKGROUP_SIZE 512
  #define WORLD_DIM WORKGROUP_SIZE
  #define MAX_CELLS_PER_ORGANISM 200
#endif

#if WORLD_COMPLEXITY == 3
  #define NUM_ORGANISMS 128
  // this provides a massive speed increase on the GPU.
  #define WORKGROUP_SIZE 128
  #define WORLD_DIM (WORKGROUP_SIZE*8)
  #define MAX_CELLS_PER_ORGANISM 128
#endif

#if WORLD_COMPLEXITY == 4
  #define NUM_ORGANISMS 512
  // this provides a massive speed increase on the GPU.
  #define WORKGROUP_SIZE 512
  #define WORLD_DIM 512
  #define MAX_CELLS_PER_ORGANISM 64
#endif

#if WORLD_COMPLEXITY == 5
  #define NUM_ORGANISMS 512
  // this provides a massive speed increase on the GPU.
  #define WORKGROUP_SIZE 512
  #define WORLD_DIM 1024
  #define MAX_CELLS_PER_ORGANISM 128
#endif
*/

// render data
//#define MAX_TOTAL_CELLS (NUM_ORGANISMS * MAX_CELLS_PER_ORGANISM)
//#define MAX_SAVED_RENDER_FRAMES 1
//#define MAX_RENDER_FRAME_SIZE (MAX_TOTAL_CELLS * 5)
//#define RENDER_BUFFER_SIZE (1 + MAX_SAVED_RENDER_FRAMES * (MAX_RENDER_FRAME_SIZE + 1))

#define GENOME_LENGTH 2048
#define CELL_SIZE 1.0f
#define ORGANISM_RADIUS (CELL_SIZE*10.0f)
#define MAX_VELOCITY (CELL_SIZE*.3f)
#define NUM_FRAMES_TO_TEST 100

#define WALL_DAMPER 1.0f

#define MAX_CONNECTIONS 2
#define MAX_BONES 6

// how many cells can a bone connect?
#define NUM_BONE_CONNECTIONS 25
#define MAX_BONE_CONNECTIONS 25


// rendering
#define RCMD_LENGTH 5
#define CELL_RENDER_SIZE 10

#define RCMD_SET_LINE_COLOR 1
#define RCMD_SET_FILL_COLOR 2
#define RCMD_SET_LINE_WIDTH 3
#define RCMD_CIRCLE 4
#define RCMD_LINE 5



#endif /* constants_h */
