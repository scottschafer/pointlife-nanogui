//
//  MainScreen.h
//  ClusterBotz
//
//  Created by Scott Schafer on 10/3/17.
//
//

#ifndef SettingsWindow_h
#define SettingsWindow_h

#include <cstdlib>
#include <pthread.h>
#include <unistd.h>
#include <nanogui/nanogui.h>
#include "Globals.h"
#include "WorldData.h"

using namespace nanogui;

class SettingsWindow : public Window {
public:
  SettingsWindow(Widget *parent, WorldData & );
  virtual bool keyboardEvent(int key, int scancode, int action, int modifiers);

private:
  void createParamUI(std::string label, float minVal, float maxVal, float * val);
  //void createParamUI(std::string label, int minVal, int maxVal, int * val);
  
private:
  WorldData & world;
  Widget * mainPanel;
};

#endif /* SettingsWindow_h */
