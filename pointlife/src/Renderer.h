//
//  Renderer.h
//  ClusterBotzGPU
//
//  Created by Scott Schafer on 5/22/17.
//  Copyright © 2017 Scott Schafer. All rights reserved.
//

#ifndef Renderer_h
#define Renderer_h


#include <nanogui/opengl.h>
//#include <nanogui/glutil.h>
#include <nanogui/screen.h>
#include <nanogui/window.h>
#include <nanogui/layout.h>
#include <nanogui/label.h>
#include <nanogui/checkbox.h>
#include <nanogui/button.h>
#include <nanogui/toolbutton.h>
#include <nanogui/popupbutton.h>
#include <nanogui/combobox.h>
#include <nanogui/progressbar.h>
//#include <nanogui/entypo.h>
#include <nanogui/messagedialog.h>
#include <nanogui/textbox.h>
#include <nanogui/slider.h>
#include <nanogui/imagepanel.h>
#include <nanogui/imageview.h>
#include <nanogui/vscrollpanel.h>
#include <nanogui/colorwheel.h>
#include <nanogui/graph.h>
#include <nanogui/tabwidget.h>
//#include <nanogui/canvas.h>
#include <vector>
#include "Point.h"
#include "Color.h"
#include "FlexArrays.h"

enum RenderType {
  RTYPE_CIRCLE,
  RTYPE_LINE,
  RTYPE_POLYGON,
  RTYPE_TEXT
};

class RenderLock {
public:
  RenderLock();
  ~RenderLock();
};

class WorldData;

class RenderObject {
public:

  RenderObject() {}
  
  static void createCircle(RenderObject & obj, float cx, float cy, float r, Color frameColor, float frameWidth, Color fillColor) {
    obj.type = RTYPE_CIRCLE;
    obj.circleX = cx;
    obj.circleY = cy;
    obj.radius = r;
    obj.frameColor = frameColor;
    obj.frameWidth = frameWidth;
    obj.fillColor = fillColor;
  }
  
  static void createLine(RenderObject & obj, float x1, float y1, float x2, float y2, Color frameColor, float frameWidth) {
    obj.type = RTYPE_LINE;
    obj.points.push_back(Point(x1, y1));
    obj.points.push_back(Point(x2, y2));
    obj.frameColor = frameColor;
    obj.frameWidth = frameWidth;
  }
  
  static void createPolygon(RenderObject & obj, std::vector<Point> & points, Color frameColor, float frameWidth, Color fillColor) {
    obj.type = RTYPE_POLYGON;
    obj.points = points;
    obj.frameColor = frameColor;
    obj.frameWidth = frameWidth;
    obj.fillColor = fillColor;
  }
  
public:
  RenderType type;
  Color frameColor;
  float frameWidth;
  Color fillColor;
  float circleX, circleY, radius;
  std::vector<Point> points;

};

class Renderer : public nanogui::Canvas{
private:
  static Renderer * _instance;
  WorldData & worldData;
  
  Renderer(nanogui::Widget *parent, WorldData & worldData);
  
public:
  static bool isInited() { return !!_instance; }
  static Renderer & instance() { return *_instance; }

  static void init(nanogui::Widget *, WorldData & worldData, int windowSize = 800);
  
  void setMagnification(float magnification);
  void setCameraOffset(float worldX, float worldY);

  void clearRenderList() { listObjects.resize(0); }
  
  void addCircle(float cx, float cy, float r, Color frameColor, float frameWidth, Color fillColor) {
    RenderObject obj;
    RenderObject::createCircle(obj, cx, cy, r, frameColor, frameWidth, fillColor);
    listObjects.push_back(obj);
  }
  
  void addLine(float x1, float y1, float x2, float y2, Color frameColor, float frameWidth) {
    RenderObject obj;
    RenderObject::createLine(obj, x1, y1, x2, y2, frameColor, frameWidth);
    listObjects.push_back(obj);
  }
  
  void addPolygon(std::vector<Point> & points, Color frameColor, float frameWidth, Color fillColor) {
    RenderObject obj;
    RenderObject::createPolygon(obj, points, frameColor, frameWidth, fillColor);
    listObjects.push_back(obj);
  }

  virtual void draw(NVGcontext *ctx);
//  virtual void drawGL();
  
private:
  //static void display(void);
  static void determineOffset();

private:
  std::vector<RenderObject> listObjects;
  
  
};

#endif /* Renderer_h */
