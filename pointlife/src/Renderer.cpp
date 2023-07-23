//
//  main.cpp
//  ClusterBotzGPU
//
//  Created by Scott Schafer on 5/13/17.
//  Copyright © 2017 Scott Schafer. All rights reserved.
//

#include <iostream>
#include <vector>
#include <GLUT/glut.h>
#include <cstdlib>
#include <pthread.h>
#include <unistd.h>
#include <vector>

#include "constants.h"
#include "Renderer.h"
#include "Globals.h"
#include "WorldData.h"
#include "utils.h"
#include "gpuOrganism/core/_inc/CellTypes.h"
#include "../EasyCL/EasyCL.h"

using namespace nanogui;
using namespace std;

nanogui::GLShader mShader;
Eigen::Vector3f mRotation(nanogui::Vector3f(0.25, 0.5, 0.33));

// static variables
Renderer * Renderer::_instance = 0;
static float cameraX = 0, cameraY = 0;

void reshape(int, int);
void keyboardFunc(unsigned char key, int x, int y);

static float windowWidth, windowHeight;

static pthread_mutex_t rmutex = PTHREAD_MUTEX_INITIALIZER;

RenderLock::RenderLock() {
  pthread_mutex_lock( &rmutex );
}

RenderLock::~RenderLock() {
  pthread_mutex_unlock( &rmutex );
}


Renderer :: Renderer(nanogui::Widget *parent, WorldData & worldData) : nanogui::GLCanvas(parent), worldData(worldData) {
  _instance = this;
  glfwSwapInterval(1);
}


void Renderer :: init(nanogui::Widget * widget, WorldData & worldData, int windowSize) {
  
  if (_instance) {
    return;
  }
  windowWidth = windowHeight = windowSize;
  Globals::magnification = float(windowSize) / float(WORLD_DIM);
  
  Renderer * pRenderer = new Renderer(widget, worldData);
  pRenderer->setBackgroundColor(nanogui::Color(255,0,0,1));
  pRenderer->setDrawBorder(true);
  
  pRenderer->setSize(Vector2i(windowSize, windowSize));
}

void Renderer :: setMagnification(float magnification) {
  Globals::magnification = magnification;
}

void Renderer :: setCameraOffset(float worldX, float worldY) {
  cameraX = worldX;
  cameraY = worldY;
}


float lastFocusX = WORLD_DIM / 2;
float lastFocusY = WORLD_DIM / 2;

static float fromIntCoordinate(int v) {
  return float(v) / float(INT_MAX) * float(WORLD_DIM);
}

extern volatile bool gNeedsNextFrame;

bool gDoneInit = false;

static FloatArray localCellDataCopy;

static vector<NVGcolor> organismColors;

void Renderer::draw(NVGcontext *ctx) {
    
  if (organismColors.size() == 0) {
    for (int i = 0; i < NUM_ORGANISMS; i++) {
      int r = randRange(64, 255);
      int g = randRange(64, 255);
      int b = randRange(64, 255);
      organismColors.push_back(nvgRGBA(r,g,b,128));
    }
  }
  
  extern long curMicroseconds();
  if (! worldData.inited) {
    return;
  }
  
  if (! worldData.cellData.size()) {
    return;
  }
  
  FloatArray & srcCellData = worldData.cellData;//ToRender;
  //srcCellData.copyFromDevice();
  
  vector<float> cellData;
  
  cellData.resize(srcCellData.size());
  {
    //RenderLock rl;
    std::memcpy(cellData.data(), srcCellData.data(), srcCellData.size() * sizeof(float));
  }
  
  //if (! localCellDataCopy.size()) {
  //localCellDataCopy.copyFrom(cellData);
  //}
  
  CellVO * pCell = (CellVO *) cellData.data();
  OrganismVO * pOrganisms = (OrganismVO *) worldData.organismData.data();
  
  CellVO * pAllCells = pCell;
  
  float focusX = 0, focusY = 0, focusCount = 0;
  for (int i = 0; i < worldData.totalCellCount; i ++) {
    if (pCell->organismIndex == Globals::focusedCritter) {
      focusX += pCell->x;
      focusY += pCell->y;
      ++focusCount;
    }
    ++pCell;
  }
  if (focusCount) {
    focusX /= focusCount;
    focusY /= focusCount;
  }
  pCell = (CellVO *) cellData.data();
  
  Widget::draw(ctx);
  nvgEndFrame(ctx);
  
  float scale = Globals::magnification;
  
  float offsetX = (windowWidth / scale) / 2;
  float offsetY = (windowHeight / scale) / 2;
  
  
  float cellsPerMS = 100;
  char msg[200];
  sprintf(msg, "Cells per ms: %f", Globals::lastCellsPerMs);
  nvgFontSize(ctx, 48);
  nvgText(ctx, 30, 30, msg, msg + std::strlen(msg));
  
  int numOrganisms = int(worldData.organismData.size() / ORGANISM_VO_SIZE);
  sprintf(msg, "# organisms: %d, # cells: %d", worldData.totalOrganismCount, worldData.totalCellCount);
  nvgFontSize(ctx, 36);
  nvgText(ctx, 30, 70, msg, msg + std::strlen(msg));
  
  
  offsetX -= focusX;
  offsetY -= focusY;
  
  Globals::offsetX = offsetX;
  Globals::offsetY = offsetY;
  
  nvgScale(ctx, scale, scale);
  nvgTranslate(ctx, offsetX, offsetY);
  
  nvgBeginPath(ctx);
  nvgStrokeWidth(ctx, 1.0f);
  nvgRect(ctx, 0, 0, WORLD_DIM, WORLD_DIM);
  nvgStrokeColor(ctx, nanogui::Color(255,255,0,255));
  nvgStroke(ctx);
  
  
  nvgStrokeColor(ctx, nanogui::Color(255,255,0,255));
  
  int step = 1;
  float size = CELL_SIZE;// * 3;
  
  //nvgFillColor(ctx, nanogui::Color(255,255,255,128));
  
  // draw cells
  for (int i = 0; i < worldData.totalCellCount; i += step) {
    
//    if (pCell->actionType < 0) {
//      ++pCell;
//      continue;
//    }
    nvgFillColor(ctx, organismColors[pCell->organismIndex]);
    
    NVGcolor color = organismColors[pCell->organismIndex];
//    switch(pCell->actionType) {
//      case actionBite:
//        color = nanogui::Color(pCell->activationDuration ? 255 : 128,0,0,255);
//        break;
//      case actionMove:
//        color = nanogui::Color(64,64,190,255);
//        break;
//      case actionTimer:
//        color = nanogui::Color(128,128,32,255);
//        break;
//    }
    float a = pOrganisms[pCell->organismIndex].energy / INITIAL_ENERGY;
    a = a * .75f + .25f;
    if (a < 0) {
      a = 0;
    }
    else if (a > 1.0f) {
      a = 1.0f;
    }
    if (pOrganisms[pCell->organismIndex].numConnections == 0) {
      a = 1.0f;
    }
    a = 1.0f;
    color.rgba[3] = a;
    nvgFillColor(ctx, color);
    
    /*
     if (pCell->actionType == actionBite) {
     nvgFillColor(ctx, nanogui::Color(255,0,0,255));
     //}
     //if (pCell->organismIndex == Globals::focusedCritter) {
     //  nvgFillColor(ctx, nanogui::Color(255,255,255,192));
     }
     else {
     nvgFillColor(ctx, organismColors[pCell->organismIndex]);
     }
     */
    
    // draw cell. Use rectangle instead of circle for speed.
    nvgBeginPath(ctx);
    if (true || Globals::magnification > 10 && pCell->organismIndex == Globals::focusedCritter) {
//      nvgCircle(ctx, pCell->x, pCell->y, size * .5f * (i == 0 ? 4.0f : 1.0f));
      nvgCircle(ctx, pCell->x, pCell->y, size * .5f);
    }
    else {
      nvgRect(ctx, pCell->x, pCell->y, size, size);
    }
    nvgFill(ctx);
    
    /*
    if (true || pCell->organismIndex == Globals::focusedCritter) {
      // draw cell connections
      nvgStrokeWidth(ctx, CELL_SIZE/10.0f);
      nvgStrokeColor(ctx, nvgRGBA(255,255,255,255));
      for (int j = 0; j < pCell->connectionCount; j++) {
        nvgBeginPath(ctx);
        CellVO * pToCell = &pAllCells[pCell->connections[j]];
        nvgMoveTo(ctx, pCell->x, pCell->y);
        nvgLineTo(ctx, pToCell->x, pToCell->y);
        nvgStroke(ctx);
      }
    }
    
    if (pCell->actionType == actionMove && pCell->activationDuration > 0) {
      
      ////
      nvgStrokeWidth(ctx, CELL_SIZE/1.5f);
      
      nvgBeginPath(ctx);
      nvgMoveTo(ctx, pCell->x, pCell->y);
      nvgLineTo(ctx, pCell->x + pCell->actionX * 20, pCell->y + pCell->actionY * 20);
      nvgStrokeColor(ctx, nvgRGBA(0,0,255,192));
      nvgStroke(ctx);
      
      ////
    }
    */
    ++pCell;
  }
  
  
  // draw connections
  pCell = (CellVO *) cellData.data();
  for (int i = 0; i < worldData.totalCellCount; i += step) {
      // draw cell connections
      nvgStrokeWidth(ctx, CELL_SIZE/20.0f);
      nvgStrokeColor(ctx, nvgRGBA(255,255,255,255));
      for (int j = 0; j < pCell->connectionCount; j++) {
        nvgBeginPath(ctx);
        CellVO * pToCell = &pAllCells[pCell->connections[j]];
        nvgMoveTo(ctx, pCell->x, pCell->y);
        nvgLineTo(ctx, pToCell->x, pToCell->y);
        nvgStroke(ctx);
    }
    
    
    if (pCell->vertexId == 0) {
      CellVO * pCellR = (CellVO*) (cellData.data() + pCell->connections[0] * CELL_VO_SIZE);
      CellVO * pCellL = (CellVO*) (cellData.data() + pCellR->connections[0] * CELL_VO_SIZE);

      float xT = pCell->x;
      float yT = pCell->y;

      float xR = pCellR->x;
      float yR = pCellR->y;

      float xL = pCellL->x;
      float yL = pCellL->y;

      float scale = 1.0f;
      float scaleL = pCell->localData[WHEEL_LEFT] * scale;
      float scaleR = pCell->localData[WHEEL_RIGHT] * scale;
      
      nvgStrokeWidth(ctx, CELL_SIZE / 5.0f);
      nvgStrokeColor(ctx, nvgRGBA(255,0,255,255));

      nvgBeginPath(ctx);
      nvgMoveTo(ctx, xR, yR);
      nvgLineTo(ctx,
        xR + scaleR * (xR - xT),
        yR + scaleR * (yR - yT));
      nvgStroke(ctx);

      nvgBeginPath(ctx);
      nvgMoveTo(ctx, xL, yL);
      nvgLineTo(ctx,
        xL + scaleL * (xL - xT),
        yL + scaleL * (yL - yT));
      nvgStroke(ctx);

    }
    
    
    ++pCell;
  }

  // draw bones
  pCell = (CellVO *) cellData.data();
  BoneVO * pBone = (BoneVO*) worldData.boneData.data();
  nvgStrokeWidth(ctx, CELL_SIZE/4.0f);
  
  for (int i = 0; i < worldData.boneData.size() / sizeof(BoneVO); i++) {
    if (pBone->numConnections == -1) {
      break;
    }
    
    nvgBeginPath(ctx);
    /*
     nvgMoveTo(ctx, pCell[pBone->connectedCells[0]].x,  pCell[pBone->connectedCells[0]].y);
     for (int j = 1; j < pBone->numConnections; j++) {
     nvgLineTo(ctx, pCell[pBone->connectedCells[j]].x,  pCell[pBone->connectedCells[j]].y);
     }
     */
    nvgMoveTo(ctx, pBone->renderX[0], pBone->renderY[0]);
    
    for (int j = 1; j < pBone->numConnections; j++) {
      nvgLineTo(ctx, pBone->renderX[j], pBone->renderY[j]);
    }
    
    nvgStrokeColor(ctx, nvgRGBA(255,0,0,255));
    nvgStroke(ctx);
    ++pBone;
  }
  
  
  
  //////////////////////////////
  
#if 0
  
#if USE_GPU
  //renderData.copyFromDevice();
#endif
  
  //Globals::needsNextFrame.store(true);
  gNeedsNextFrame = true;
  //std::cout << "Set Globals::needsNextFrame to true";
  
  int * pRender = renderData;
  float focusX = fromIntCoordinate(*pRender++);
  float focusY = fromIntCoordinate(*pRender++);
  int numCommands = *pRender++;
  
  Widget::draw(ctx);
  nvgEndFrame(ctx);
  
  float scale = Globals::magnification;
  
  float offsetX = (windowWidth / scale) / 2; //focusX * Globals::magnification - windowWidth / 2;
  float offsetY = (windowHeight / scale) / 2; //focusY * Globals::magnification - windowHeight / 2;
  
  offsetX -= focusX;// / scale;
  offsetY -= focusY;// / scale;
  //scale *= .5f;
  //focusX = focusY = 100.0f;//WORLD_DIM / 2.0f;
  nvgScale(ctx, scale, scale);
  
  nvgTranslate(ctx, offsetX, offsetY);
  //(windowWidth / scale  - WORLD_DIM) / 2 - focusX / scale,
  //(windowHeight / scale  - WORLD_DIM) / 2 - focusY / scale);
  
  //  nvgTranslate(ctx, (WORLD_DIM / 2 - focusX) / scale, (WORLD_DIM / 2 - focusY) / scale);
  //nvgTranslate(ctx, scale * WORLD_DIM / 2, scale * WORLD_DIM / 2);
  nvgBeginPath(ctx);
  nvgStrokeWidth(ctx, 1.0f);
  nvgRect(ctx, 0, 0, WORLD_DIM, WORLD_DIM);
  nvgStrokeColor(ctx, nanogui::Color(255,255,0,255));
  nvgStroke(ctx);
  
  
  nvgStrokeColor(ctx, nanogui::Color(255,255,0,255));
  
  while (numCommands--) {
    int command = *pRender++;
    switch (command) {
        
      case RCMD_SET_FILL_COLOR: {
        unsigned int rgba = *(unsigned int *)pRender++;
        int r = (rgba>>24) & 255;
        int g = (rgba>>16) & 255;
        int b = (rgba>>8) & 255;
        int a = (rgba) & 255;
        nvgFillColor(ctx, nvgRGBA(r, g, b, a));
        break;}
        
        
      case RCMD_CIRCLE: {
        float x = fromIntCoordinate(*pRender++);
        float y = fromIntCoordinate(*pRender++);
        float r = fromIntCoordinate(*pRender++);
        
        nvgBeginPath(ctx);
        nvgCircle(ctx, x, y, r);
        //nvgFillColor(ctx, nvgRGBA(110, 201, 235, 180));
        nvgFill(ctx);
        
        //        nvgCircle(ctx, x, y, r);
        break;}
    }
  }
#endif
}


/*
 if (mDrawBorder) {
 nvgBeginPath(ctx);
 nvgStrokeWidth(ctx, 10.0f);
 nvgRoundedRect(ctx, mPos.x() - 0.5f, mPos.y() - 0.5f,
 mSize.x() + 1, mSize.y() + 1, 20);
 nvgStrokeColor(ctx, nanogui::Color(255,255,0,255));
 nvgRoundedRect(ctx, mPos.x() - 1.0f, mPos.y() - 1.0f,
 mSize.x() + 2, mSize.y() + 2, 20);
 nvgStrokeColor(ctx, nanogui::Color(255,255,0,255));
 nvgStroke(ctx);
 }
 
 const Screen* screen = dynamic_cast<const Screen*>(this->window()->parent());
 assert(screen);
 
 float pixelRatio = screen->pixelRatio();
 Vector2f screenSize = screen->size().cast<float>();
 Vector2i positionInScreen = absolutePosition();
 
 Vector2i size = (mSize.cast<float>() * pixelRatio).cast<int>(),
 imagePosition = (Vector2f(positionInScreen[0],
 screenSize[1] - positionInScreen[1] -
 (float) mSize[1]) * pixelRatio).cast<int>();
 
 GLint storedViewport[4];
 glGetIntegerv(GL_VIEWPORT, storedViewport);
 
 glViewport(imagePosition[0], imagePosition[1], size[0] , size[1]);
 
 glEnable(GL_SCISSOR_TEST);
 glScissor(imagePosition[0], imagePosition[1], size[0], size[1]);
 glClearColor(mBackgroundColor[0], mBackgroundColor[1],
 mBackgroundColor[2], mBackgroundColor[3]);
 glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
 
 this->drawGL();
 
 glDisable(GL_SCISSOR_TEST);
 glViewport(storedViewport[0], storedViewport[1],
 storedViewport[2], storedViewport[3]);
 }
 */

void DrawCircle(float cx, float cy, float r, int num_segments) {
  glBegin(GL_LINE_LOOP);
  for (int ii = 0; ii < num_segments; ii++)   {
    float theta = 2.0f * 3.1415926f * float(ii) / float(num_segments);//get the current angle
    float x = r * cos(theta);//calculate the x component
    float y = r * sin(theta);//calculate the y component
    glVertex2f(x + cx, y + cy);//output vertex
  }
  glEnd();
}

void Renderer::determineOffset() {
  //  Globals::offsetX = (WORLD_DIM * Globals::magnification - WORLD_DIM) / 2;
  //  Globals::offsetY = Globals::offsetX;
  
  /*
   critterX = critterY = 0;
   
   //return;
   // TODO - handle focus on organism
   int focusIndex = Globals :: mFocusCritter;
   if (focusIndex != -1) {
   WorldThreadMutex wtm;
   
   World & world = WorldThreadRunner::getWorld();
   Cell * cells = world.mCells;
   
   
   if (cells[focusIndex].mNumConnections == 0 ||
   cells[focusIndex].mEntityIndex != lastEntityIndex ||
   cells[focusIndex].mEntityCount != lastEntityLength ) {
   focusIndex = Globals :: mFocusCritter = -1;
   }
   if (focusIndex != -1) {
   
   int entityIndex = cells[focusIndex].mEntityIndex;
   
   //        generation = world.getGeneration() || ! Globals::inFitnessTest;
   int testIndex = world.getTestIndex();
   
   #if 0
   
   double length = 0;
   for (int i = 0; i < world.mNumCells; i++) {
   if (cells[i].mOnBoard && cells[i].mEntityIndex == entityIndex) {
   critterX += cells[i].mPos.x;// - WORLD_DIM/2;
   critterY += cells[i].mPos.y;// - WORLD_DIM/2;
   ++length;
   }
   }
   
   if (length > 1) {
   critterX /= length;
   critterY /= length;
   //offsetX += (critterX / length) * Globals::magnification;
   //offsetY += (critterY / length) * Globals::magnification;
   
   offsetX = critterX * Globals::magnification - WORLD_DIM / 2;
   offsetY = critterY * Globals::magnification - WORLD_DIM / 2;
   
   }
   else {
   focusNextCritter();
   }
   #endif
   }
   //int firstEntityIndex = cells[0].mEntityIndex;
   }
   */
}


void setLineWidth( float w) {
  if (w == 0) {
    w = .000000000000000000001;
  }
  glLineWidth( w * Globals::magnification);
  return;
  
  GLfloat lineWidth[2];
  glGetFloatv(GL_ALIASED_LINE_WIDTH_RANGE, lineWidth);
  GLfloat minW = lineWidth[1];
  GLfloat maxW = lineWidth[0] * 100;
  
  w *= minW;
  float v = w * Globals::magnification;
  
  
  v = std::min(std::max(v, minW), maxW);
  
  glLineWidth(v);
}





void Renderer::drawGL() {
  
  
  if (0) {
    
    mShader.bind();
    
    Matrix4f mvp;
    mvp.setIdentity();
    float fTime = (float)glfwGetTime();
    mvp.topLeftCorner<3,3>() = Eigen::Matrix3f(Eigen::AngleAxisf(mRotation[0]*fTime, Vector3f::UnitX()) *
                                               Eigen::AngleAxisf(mRotation[1]*fTime,  Vector3f::UnitY()) *
                                               Eigen::AngleAxisf(mRotation[2]*fTime, Vector3f::UnitZ())) * 0.25f;
    
    mShader.setUniform("modelViewProj", mvp);
    
    glEnable(GL_DEPTH_TEST);
    /* Draw 12 triangles starting at index 0 */
    mShader.drawIndexed(GL_TRIANGLES, 0, 12);
    
    glDisable(GL_DEPTH_TEST);
  }
  
  // Draw a Red 1x1 Square centered at origin
  glBegin(GL_QUADS);              // Each set of 4 vertices form a quad
  glColor3f(1.0f, 0.0f, 0.0f); // Red
  glVertex2f(-100000.0f, -100000.0f);    // x, y
  glVertex2f( 100000.0f, -100000.0f);
  glVertex2f( 100000.0f,  100000.0f);
  glVertex2f(-100000.0f,  100000.0f);
  glEnd();
  //glFlush();
  return;
  
  
  float smooth = 0;//.3f;
  
  std::vector<RenderObject> & listObjects = listObjects;
  
  /*
   glEnable( GL_POINT_SMOOTH );
   glEnable( GL_BLEND );
   glEnable(GL_SCISSOR_TEST);
   glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
   
   //glScissor(0, 0, WINDOW_SIZE, WINDOW_SIZE);
   
   glClearColor( 0, 0, 0, 1.0f );
   glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
   //glWindowPos2i(-offsetX,-offsetY);
   //glPixelZoom(Globals::magnification, Globals::magnification);
   //  glRasterPos2d(0,0);
   //  glPixelZoom(1,1);
   
   // save transformation
   glPushMatrix();
   
   // position rendering
   */
  
  {
    RenderLock rl;
    
    
    float offsetX = Globals::focusX * Globals::magnification - windowWidth / 2;
    float offsetY = Globals::focusY * Globals::magnification - windowHeight / 2;
    
    lastFocusX = offsetX = lastFocusX * smooth + (1.0f - smooth) * offsetX;
    lastFocusY = offsetY = lastFocusY * smooth + (1.0f - smooth) * offsetY;
    
    glTranslatef(offsetX, offsetY, 0);
    glScalef(Globals::magnification, Globals::magnification, 1);
    
    
    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_LINES);
    setLineWidth(1);
    
    glVertex2f(0, 0);
    glVertex2f(WORLD_DIM, 0);
    
    glVertex2f(WORLD_DIM, 0);
    glVertex2f(WORLD_DIM, WORLD_DIM);
    
    glVertex2f(0, WORLD_DIM);
    glVertex2f(WORLD_DIM, WORLD_DIM);
    
    glVertex2f(0, 0);
    glVertex2f(0, WORLD_DIM);
    glEnd();
    
    std::vector<RenderObject> listObjects;
    RenderObject obj;
    obj.createCircle(obj,  100, 100, 20, ::Color(255,255,255,1), 4, ::Color(255,255,255,1));
    listObjects.push_back(obj);
    
    for (int i = 0; i < listObjects.size(); i++) {
      RenderObject & obj = listObjects[i];
      glColor4f(obj.frameColor.r, obj.frameColor.g, obj.frameColor.b, obj.frameColor.a);
      
      switch (obj.type) {
        case RTYPE_CIRCLE:
          DrawCircle(
                     obj.circleX,
                     obj.circleY,
                     obj.radius,
                     6);
          break;
          
        case RTYPE_LINE:
          glBegin(GL_LINES);
          glVertex2f(obj.points[0].x, obj.points[0].y);
          glVertex2f(obj.points[1].x, obj.points[1].y);
          glEnd();
          break;
      }
    }
  }
  
  /*
   std::vector<Point> connectorLines;
   
   for (int i = 0; i < w._cellPosX->size(); i++) {
   float x = w._cellPosX->get(i);
   float y = w._cellPosY->get(i);
   gRenderPoints.push_back(Point(x, y));
   }
   
   
   for (int i = 0; i < w._connectionToIndices->size(); i++) {
   int from = w.connectionFromIndices[i];
   int to = w.connectionToIndices[i];
   float x1 = w._cellPosX->get(from);
   float y1 = w._cellPosY->get(from);
   float x2 = w._cellPosX->get(to);
   float y2 = w._cellPosY->get(to);
   
   connectorLines.push_back(Point(x1, y1));
   connectorLines.push_back(Point(x2, y2));
   }
   
   
   glColor4f(1.0, 1.0, 1.0, .25);
   glBegin(GL_LINES);
   //setLineWidth(0);// * (pass ? magnification : 1));
   for (int i = 0; i < connectorLines.size(); i += 2) {
   
   /*
   if (connectorOutside[i/2]) {
   glColor4f(1, 0, 0, .85);
   }
   //        setLineWidth(40);//connectorLineWidth[i/2]);
   
   double x1 = connectorLines[i].x * WINDOW_SIZE / WORLD_DIM;;
   double y1 = connectorLines[i].y * WINDOW_SIZE / WORLD_DIM;;
   double x2 = connectorLines[i+1].x * WINDOW_SIZE / WORLD_DIM;;
   double y2 = connectorLines[i+1].y * WINDOW_SIZE / WORLD_DIM;;
   
   glVertex2f(x1, y1);
   glVertex2f(x2, y2);
   }
   */
  
  // Draw blue text at screen coordinates (100, 120), where (0, 0) is the top-left of the
  // screen in an 18-point Helvetica font
  glRasterPos2i(100, 120);
  glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
  //glutBitmapString(GLUT_BITMAP_HELVETICA_18, "text to render");
  
  
  /*
   glEnd();
   glPopMatrix();
   
   glFinish();
   */
  //glutSwapBuffers();
  
  //glutPostRedisplay();
}








