//
//  Globals.cpp
//  ClusterBotzGPU
//
//  Created by Scott Schafer on 5/30/17.
//  Copyright © 2017 Scott Schafer. All rights reserved.
//

#include "Globals.h"
#include "constants.h"
#include "gpuOrganism/core/_inc/VO.h"

bool Globals :: exit = false;

int Globals :: focusedCritter = 0;
float Globals :: magnification = 1;
float Globals :: offsetX = 1;
float Globals :: offsetY = 1;

float Globals :: focusX = 0;
float Globals :: focusY = 0;
long Globals :: minMsPerTurn = 60; //1000 / 20;

float Globals :: lastCellsPerMs = 0;
float Globals :: numCellsProcessed = 0;
float Globals :: elapsedProcessingMs = 0;


void setDefaults(SettingsVO * pSettings) {
  pSettings->maxVelocity = .6f;
  pSettings->wallDamper = WALL_DAMPER;
  pSettings->velocityDamper = .95f;
  pSettings->centerPressure = .00f;
  
  pSettings->moveDuration = 100;
  pSettings->moveForce = .4f;
}
