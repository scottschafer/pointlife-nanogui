//
//  main.cpp
//  ClusterBotzGPU
//
//  Created by Scott Schafer on 5/13/17.
//  Copyright © 2017 Scott Schafer. All rights reserved.
//

#include <iostream>
#include <vector>

#include <cstdlib>
#include <pthread.h>
#include <unistd.h>
#include <nanogui/nanogui.h>

#include "../EasyCL/EasyCL.h"

#include "constants.h"

// #include "gpuOrganism/applyPhysics.h"
#include "gpuOrganism/applyPointPhysics.h"
#include "gpuOrganism/applyConnectionPhysics.h"
#include "gpuOrganism/applyCollisionPhysics.h"
#include "gpuOrganism/applyCellActions.h"
#include "gpuOrganism/applyBones.h"

//#include "gpuOrganism/outputRenderData.h"
#include "WorldData.h"
#include "Point.h"
#include "utils.h"
#include "Renderer.h"
#include "Globals.h"
#include "MainScreen.h"
#include "SettingsWindow.h"

using namespace nanogui;

/* Force usage of discrete GPU on laptops */
NANOGUI_FORCE_DISCRETE_GPU();

const float FPS = 120;
const float MICROSECONDS_PER_FRAME = 1000000 / FPS;

//int nanomain();

using namespace std;
using namespace easycl;
//using namespace nanogui;

EasyCL * cl = NULL;

WorldData w;

volatile bool gNeedsNextFrame = false;
volatile int inApplyPhysics = 0;

SettingsWindow * settingsWindow = NULL;

long lastCopyTimeMs = 0;

void applyPhysics(bool render) {
  
  if (inApplyPhysics) {
    return;
  }
  ++inApplyPhysics;
  
  //printf("%lf\n", curSeconds());
  
  long b4 = curMicroseconds();
  int turnsPerCrank = 0;
  
  //w.cellDataToRender.copyFrom(w.cellData);
  //cl->flush();
  
  int count = 1;
  
  while (true) {

    if (cl) {
//      cl->setProfiling(true);
    }
    
    ++turnsPerCrank;
    long turnStartMs = curMicroseconds();
    
    for (int i = 0; i < count; i++) {
      ++w.turn;
      Globals::numCellsProcessed += w.totalCellCount;
      //applyBones(w);
      //applyCellActions(w);
      applyConnectionPhysics(w);
      //w.validate();
      applyPointPhysics(w);
      //applyConnectionPhysics(w);
      //w.validate();
      applyCollisionPhysics(w);
      //w.validate();
    }
    
    w.cellData.copyFromDevice();
    w.organismData.copyFromDevice();
    
    //w.boneData.copyFromDevice();
    
    long elapsedTurnMs = curMicroseconds() - turnStartMs;
    //printf("Turn took %ld ms\n", elapsedTurnMs);
    
    
    long elapsedTime = curMicroseconds() - b4;
    if (elapsedTime > (MICROSECONDS_PER_FRAME - lastCopyTimeMs)) {
      break;
    }
    if (Globals::minMsPerTurn != 0) {
      break;
    }
    break;
  }
  //printf("turnsPerCrank = %d\n", turnsPerCrank);
  
#if USE_GPU
/*
  b4 = curMicroseconds();
  //w.cellData.copyFromDevice();    //cl->finish();
  {
    // RenderLock rl;
    //w.cellDataToRender.copyFrom(w.cellData);
    
    //w.cellDataToRender.copyFromDeviceEx();
    //w.cellData.copyFromDeviceEx();
    
    //w.cellData.cop
    lastCopyTimeMs = curMicroseconds() - b4;
  }
  //printf("copy took %ld ms\n", lastCopyTimeMs);
  */
#endif
  
  w.inited = true;
  
  /*
   if (render || 1) {
   //    w.cellDataToRender.copyFrom(w.cellData);
   
   #if USE_GPU
   w.cellData.copyFromDevice();
   long elapsedTime = curMicroseconds() - b4;
   if (elapsedTime > 100) {
   //printf("elapsed time %ld\n", elapsedTime);
   }
   #endif
   w.inited = true;
   }
   */
  
  /*
   if (render) {
   render = false;
   
   //outputRenderData(w);
   #if USE_GPU
   w.cellData.copyFromDevice();
   //    w.renderData.copyFromDevice();
   #endif
   gNeedsNextFrame = false;
   //Globals::needsNextFrame = false;
   //std::cout << "Set Globals::needsNextFrame to false";
   }
   */
 if (cl) {
//    cl->dumpProfiling();
  }

  --inApplyPhysics;
}

static pthread_t worldThread;

long timeSinceLastRender = 0;
long accumulatedTime = 0;

static void * threadFunc(void * pData) {
  int i = 0;
  int numTurns = 100;
  
  int lastSec = 0;
  
  while (true) {
    
    if (Globals::exit) {
      exit(0);
      return 0;
    }
    
    long minMsPerTurn = Globals::minMsPerTurn;
    //minMsPerTurn *= 2;
    /*
    if (minMsPerTurn < 50) {
//      numTurns = 101 - minMsPerTurn * 2;
//      minMsPerTurn = 0;
      minMsPerTurn *= 5;// * (minMsPerTurn - 51);
    }
    else {
      numTurns = 1;
      minMsPerTurn = 10 * (minMsPerTurn - 51);
    }
    */
    
    if (0 == i % numTurns) {
      accumulatedTime = 0;
    }
    long elapsedTime = curMicroseconds();
    long startTurnTime = elapsedTime;
    
    int sec = int(elapsedTime / 1000000);
    if (sec != lastSec) {
      lastSec = sec;
      //cout << sec << endl;
    }
    
    bool render = (elapsedTime - timeSinceLastRender) > MICROSECONDS_PER_FRAME;
    
    if (render) {
      //w.cellData.copyFromDevice();
      //w.organismData.copyFromDevice();
      //      printf("MICROSECONDS_PER_FRAME = %lf, (elapsedTime - timeSinceLastRender) = %ld\n",
      //      double(MICROSECONDS_PER_FRAME), (elapsedTime - timeSinceLastRender));
      timeSinceLastRender = elapsedTime;
    }
    else {
      //printf("! render, (elapsedTime - timeSinceLastRender) = %ld\n",(elapsedTime - timeSinceLastRender));
    }
    
    accumulatedTime -= elapsedTime;
    applyPhysics(render);//copy /*(i++ % numTurns) == 0 */ || ! USE_GPU);
    accumulatedTime += curMicroseconds();
    
    // every 100 turns (?), handle birth and death
    /*
    if ((w.turn % 100) == 0) {
      ConnectionVO * pConnections = (ConnectionVO *) w.connectionData.data();
      CellVO * pCells = (CellVO *) w.cellData.data();
      OrganismVO * pOrganism = (OrganismVO *) w.organismData.data();
      
      while (true) {
        if (pOrganism->numConnections < -1) {
          break;
        }
        if (false && pOrganism->numConnections && pOrganism->energy <= 0) {
          // kill it!
          int i = pOrganism->iStartConnection;
          int n = pOrganism->numConnections;
          pOrganism->numConnections = 0;
          while (n--) {
            CellVO * pCellFrom = &pCells[pConnections[i].iCellFrom];
            CellVO * pCellTo = &pCells[pConnections[i].iCellTo];
            pCellFrom->connectionCount = pCellFrom->connectionCount = 0;
            pCellTo->connectionCount = pCellTo->actionType = 0;
            pCellFrom->actionType = pCellTo->actionType = -1;
            ++i;
          }
        }
        ++pOrganism;
      }
      w.copyToDevice();
    }
    */
    
    //::usleep(1000000 / 50);
    
    minMsPerTurn *= 1000L;
    elapsedTime = curMicroseconds() - elapsedTime;
    if (elapsedTime < minMsPerTurn) {
      ::usleep(minMsPerTurn - elapsedTime);
    }
    ++i;
    if (0 == i % numTurns) {
      //cout << "Elapsed time: " << accumulatedTime /* (curMicroseconds() - b4) */ << endl;
    }
    
    float elapsedTurnTimeMs = float(curMicroseconds() - startTurnTime) / 1000.0f;
    Globals::elapsedProcessingMs += elapsedTurnTimeMs;
    if (Globals::elapsedProcessingMs > 1000) {
      Globals::lastCellsPerMs = Globals::numCellsProcessed / Globals::elapsedProcessingMs;
      Globals::elapsedProcessingMs = 0;
      Globals::numCellsProcessed = 0;
    }
  }
  
  return 0;
}

void startSimulationThread() {
  if(pthread_create(&worldThread, NULL, (void *(*)(void *))&threadFunc,0)) {
    
    fprintf(stderr, "Error creating thread\n");
  }
}

int main(int argc, char** argv) {
  bool useGPU = USE_GPU;
  if (useGPU) {
    if (cl == NULL) {
      if(!EasyCL::isOpenCLAvailable()) {
        cout << "opencl library not found" << endl;
        exit(-1);
      }
      cout << "found opencl library" << endl;
      cl = EasyCL::createForFirstGpuOtherwiseCpu();
      cout << "getMaxWorkgroupSize() = " << cl->getMaxWorkgroupSize() << endl;
    }
  }
  
  w.init();
  w.copyToDevice();
  
  nanogui::init();
  
  /* scoped variables */ {
    bool use_gl_4_1 = false;// Set to true to create an OpenGL 4.1 context.
    Screen *screen = nullptr;
    
    if (use_gl_4_1) {
      // NanoGUI presents many options for you to utilize at your discretion.
      // See include/nanogui/screen.h for what all of these represent.
      screen = new MainScreen(w, Vector2i(1024, 1024), "ClusterBotz",
                              /*resizable*/true, /*fullscreen*/false);
//      , /*colorBits*/8,
//                              /*alphaBits*/8, /*depthBits*/24, /*stencilBits*/8,
//                              /*nSamples*/0, /*glMajor*/4, /*glMinor*/1);
    } else {
      screen = new MainScreen(w, Vector2i(1200, 1024), "ClusterBotz");
    }
        
    Renderer::init(screen /*window */, w, 1024);
    
    settingsWindow = new SettingsWindow(screen, w);
    

    screen->set_visible(true);
    screen->perform_layout();
    
    startSimulationThread();
  
    nanogui::mainloop();
  }
  
  nanogui::shutdown();
  return 0;
}




/*
 src/example2.cpp -- C++ version of an example application that shows
 how to use the form helper class. For a Python implementation, see
 '../python/example2.py'.
 
 NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
 The widget drawing code is based on the NanoVG demo application
 by Mikko Mononen.
 
 All rights reserved. Use of this source code is governed by a
 BSD-style license that can be found in the LICENSE.txt file.
 */

#if 0
#include <nanogui/nanogui.h>
#include <iostream>

using namespace nanogui;

enum test_enum {
  Item1 = 0,
  Item2,
  Item3
};

bool bvar = true;
int ivar = 12345678;
double dvar = 3.1415926;
float fvar = (float)dvar;
std::string strval = "A string";
test_enum enumval = Item2;
Color colval(0.5f, 0.5f, 0.7f, 1.f);

int nanomain() {
  nanogui::init();
  
  /* scoped variables */ {
    bool use_gl_4_1 = false;// Set to true to create an OpenGL 4.1 context.
    Screen *screen = nullptr;
    
    if (use_gl_4_1) {
      // NanoGUI presents many options for you to utilize at your discretion.
      // See include/nanogui/screen.h for what all of these represent.
      screen = new Screen(Vector2i(500, 700), "NanoGUI test [GL 4.1]",
                          /*resizable*/true, /*fullscreen*/false, /*colorBits*/8,
                          /*alphaBits*/8, /*depthBits*/24, /*stencilBits*/8,
                          /*nSamples*/0, /*glMajor*/4, /*glMinor*/1);
    } else {
      screen = new Screen(Vector2i(500, 700), "NanoGUI test");
    }
    
    bool enabled = true;
    FormHelper *gui = new FormHelper(screen);
    ref<Window> window = gui->addWindow(Eigen::Vector2i(10, 10), "Form helper example");
    gui->addGroup("Basic types");
    gui->addVariable("bool", bvar);
    gui->addVariable("string", strval);
    
    gui->addGroup("Validating fields");
    gui->addVariable("int", ivar)->setSpinnable(true);
    gui->addVariable("float", fvar);
    gui->addVariable("double", dvar)->setSpinnable(true);
    
    gui->addGroup("Complex types");
    gui->addVariable("Enumeration", enumval, enabled)
    ->setItems({"Item 1", "Item 2", "Item 3"});
    gui->addVariable("Color", colval);
    
    gui->addGroup("Other widgets");
    gui->addButton("A button", []() { std::cout << "Button pressed." << std::endl; });
    
    screen->setVisible(true);
    screen->performLayout();
    window->center();
    
    nanogui::mainloop();
  }
  
  nanogui::shutdown();
  return 0;
}
#endif
