//
//  MainScreen.h
//  ClusterBotz
//
//  Created by Scott Schafer on 10/3/17.
//
//

#ifndef MainScreen_h
#define MainScreen_h

#include <cstdlib>
#include <pthread.h>
#include <unistd.h>
#include <nanogui/nanogui.h>
#include "Globals.h"
#include "WorldData.h"


using namespace nanogui;

class MainScreen : public Screen {
  private:
    WorldData & w;
  
  public:
    MainScreen(WorldData & w, const Vector2i &size, const std::string &caption,
           bool resizable = true, bool fullscreen = false, int colorBits = 8,
           int alphaBits = 8, int depthBits = 24, int stencilBits = 8,
           int nSamples = 0,
           unsigned int glMajor = 3, unsigned int glMinor = 3)
  
           : Screen(size, caption, resizable, fullscreen, colorBits, alphaBits,
           depthBits, stencilBits, nSamples, glMajor, glMinor), w(w) {
            setBackground(nanogui::Color(0,0,0,255));
           }
  
  virtual bool keyboardEvent(int key, int scancode, int action, int modifiers);  
  virtual bool mouseButtonEvent(const Vector2i &p, int button, bool down, int modifiers);
};


#endif /* MainScreen_h */
