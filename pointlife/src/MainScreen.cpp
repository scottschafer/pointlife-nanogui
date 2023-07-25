//
//  MainScreen.cpp
//  ClusterBotz
//
//  Created by Scott Schafer on 10/3/17.
//
//

#include <stdio.h>
#include "MainScreen.h"

static const int minSpeed = 50;

/// Default keyboard event handler
bool MainScreen :: keyboardEvent(int key, int scancode, int action, int modifiers) {

  if (action == GLFW_PRESS || action == GLFW_REPEAT) {
    
    switch (key) {
      case GLFW_KEY_ESCAPE:             // ESCAPE key
        Globals::exit = true;
        return true;
        
      case GLFW_KEY_G:
        //Globals::gravity = ! Globals::gravity;
        return true;
        
      case 'A': {
        CellVO * pCells = (CellVO *) w.cellData.data();
        pCells[0].localData[0] += .1f;
        break; }
      
      case 'Z': {
        CellVO * pCells = (CellVO *) w.cellData.data();
        pCells[0].localData[0] -= .1f;
        break; }
        
      case '\'': {
        CellVO * pCells = (CellVO *) w.cellData.data();
        pCells[0].localData[1] += .1f;
        break; }
      
      case '/': {
        CellVO * pCells = (CellVO *) w.cellData.data();
        pCells[0].localData[1] -= .1f;
        break; }

      case 'p':
        //Globals::pause = ! Globals::pause;
        return true;
        
      case 'f':
        //Globals::inFitnessTest = ! Globals::inFitnessTest;
        return true;
        
      case GLFW_KEY_DOWN:
        Globals::magnification *= 1.1;
        return true;
        
      case GLFW_KEY_UP:
        Globals::magnification *= .9;
        return true;

      case GLFW_KEY_RIGHT:
        Globals::focusedCritter = (Globals::focusedCritter + 1) % w.totalOrganismCount;
        return true;

      case GLFW_KEY_LEFT:
        Globals::focusedCritter = (Globals::focusedCritter + w.totalOrganismCount - 1) % w.totalOrganismCount;
        return true;
        
      case GLFW_KEY_LEFT_BRACKET:
        if (Globals::minMsPerTurn == 0) {
          Globals::minMsPerTurn = minSpeed;
        }
        else {
          Globals::minMsPerTurn += 1;
        }
        return true;
        
      case GLFW_KEY_RIGHT_BRACKET:
        Globals::minMsPerTurn = std::max(0L, Globals::minMsPerTurn - 1);
//        if (Globals::minMsPerTurn <= minSpeed) {
//          Globals::minMsPerTurn = 0;
//        }
        return true;
    }
  }
  
//  return Screen::keyboardEvent(key, scancode, action, modifiers);
}

bool MainScreen :: mouseButtonEvent(const Vector2i &p, int button, bool down, int modifiers) {
  
  if (! down) {
    return;
  }
  
  float scale = Globals::magnification;
  float offsetX = Globals::offsetX;
  float offsetY = Globals::offsetY;

  float x = p.x();
  float y = p.y();
  
  x /= scale;
  y /= scale;

  x -= offsetX;
  y -= offsetY;
  
  
  w.cellData.copyFromDevice();
  CellVO * pCell = (CellVO *) w.cellData.data();
  int maxCells = w.cellData.size() / sizeof(CellVO);
//  OrganismVO * pOrganisms = (OrganismVO *) worldData.organismData.data();

  
  float closestDist = WORLD_DIM * WORLD_DIM;
  CellVO * pClosest = NULL;;
  
  while (maxCells-- > 0) {
    if (pCell->connectionCount >= 0) {
      float dx = pCell->x - x;
      float dy = pCell->y - y;
      float dsquared = dx * dx + dy * dy;
      if (dsquared < closestDist) {
        closestDist = dsquared;
        pClosest = pCell;
      }
      if (dsquared < CELL_SIZE * CELL_SIZE * 4) {
        Globals::focusedCritter = pCell->organismIndex;
        break;
      }
      break;
    }
  
    ++pCell;
  }

  cout << "click @:" << int(x) << ", " << int(y) << std::endl;
  if (pClosest) {
    cout << "closest:" << pClosest->x << ", " << pClosest->y << std::endl;
  }
  
  return true;
}
