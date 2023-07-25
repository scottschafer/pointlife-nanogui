//
//  MainScreen.cpp
//  ClusterBotz
//
//  Created by Scott Schafer on 10/3/17.
//
//

#include <stdio.h>
#include "SettingsWindow.h"


std::string to_string(const float v) {
  char buffer[100];
  sprintf(buffer, "%.3f", v);
  return buffer;
}

SettingsWindow :: SettingsWindow(Widget *parent, WorldData & data) : world(data), Window(parent, "Settings") {
  set_position(Vector2i(15, 80));
  set_layout(new GroupLayout());
  Widget *panel = new Widget(this);
  panel->set_layout(new BoxLayout(Orientation::Vertical, Alignment::Middle, 0, 20));
  mainPanel = panel;

  SettingsVO * pSettings = (SettingsVO *) world.settingsData.data();
  
  createParamUI("Max Velocity", CELL_SIZE/20.0f, CELL_SIZE, &pSettings->maxVelocity);
  createParamUI("Wall Damper", 0.0f, 1.0f, &pSettings->wallDamper);
  createParamUI("Velocity Damper", 0.5f, 1.0f, &pSettings->velocityDamper);
  createParamUI("Center Pressure", 0.0f, 1.0f, &pSettings->centerPressure);

  createParamUI("Move Duration", 1.0f, 100.0f, &pSettings->moveDuration);
  createParamUI("Move Force", 0.1f, 1.0f, &pSettings->moveForce);
  
  // Add a slider and set defaults
  
  set_visible(true);
  //performLayout();
  
}

void SettingsWindow :: createParamUI(std::string label, float minVal, float maxVal, float * val) {
  Widget *panel = new Widget(mainPanel);
  panel->set_layout(new BoxLayout(Orientation::Horizontal, Alignment::Middle, 0, 20));

  TextBox *labelTextBox = new TextBox(panel);
  labelTextBox->set_size(Vector2i(120, 25));
  labelTextBox->set_value(label);

  Slider *slider = new Slider(panel);
  slider->set_fixed_width(80);
  slider->set_range(pair<float, float>(minVal, maxVal));
  
  slider->set_value(*val);
  
  // Add a textbox and set defaults */
  TextBox *textBox = new TextBox(panel);
  textBox->set_fixed_size(Vector2i(60, 25));
  textBox->set_value(::to_string(*val));
  
  // Propagate slider changes to the text box */
  FloatArray * settingsArray = &world.settingsData;
  
  slider->set_callback([textBox, val, settingsArray](float value) {
    *val = value;
    float displayVal = float(int(value * 100))/100.0f;
    textBox->set_value(::to_string(value));
    settingsArray->copyToDevice();
  });

}

/*
void SettingsWindow :: createParamUI(std::string label, int minVal, int maxVal, float * val) {
}
*/

bool SettingsWindow :: keyboardEvent(int key, int scancode, int action, int modifiers) {
  if (key == 262) {
    Globals::magnification *= 1.1;
    return true;
  }
  if (key == 263) {
    Globals::magnification *= .9;
    return true;
  }
  return false;
}
