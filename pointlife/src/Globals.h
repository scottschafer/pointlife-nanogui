//
//  Globals.h
//  ClusterBotzGPU
//
//  Created by Scott Schafer on 5/30/17.
//  Copyright © 2017 Scott Schafer. All rights reserved.
//

#ifndef Globals_h
#define Globals_h

class Globals {
public:
  static bool exit;

  static int focusedCritter;
  static float magnification;
  static float offsetX, offsetY;
  static float focusX, focusY;
  static long minMsPerTurn;
  
  static float lastCellsPerMs;
  static float numCellsProcessed;
  static float elapsedProcessingMs;
};

#endif /* Globals_h */
