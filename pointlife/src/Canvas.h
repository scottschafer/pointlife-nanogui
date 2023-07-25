///*
//    src/example4.cpp -- C++ version of an example application that shows
//    how to use the OpenGL widget. For a Python implementation, see
//    '../python/example4.py'.
//
//    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
//    The widget drawing code is based on the NanoVG demo application
//    by Mikko Mononen.
//
//    All rights reserved. Use of this source code is governed by a
//    BSD-style license that can be found in the LICENSE.txt file.
//*/
//
//#include <nanogui/opengl.h>
////#include <nanogui/glutil.h>
//#include <nanogui/screen.h>
//#include <nanogui/window.h>
//#include <nanogui/layout.h>
//#include <nanogui/label.h>
//#include <nanogui/checkbox.h>
//#include <nanogui/button.h>
//#include <nanogui/toolbutton.h>
//#include <nanogui/popupbutton.h>
//#include <nanogui/combobox.h>
//#include <nanogui/progressbar.h>
////#include <nanogui/entypo.h>
//#include <nanogui/messagedialog.h>
//#include <nanogui/textbox.h>
//#include <nanogui/slider.h>
//#include <nanogui/imagepanel.h>
//#include <nanogui/imageview.h>
//#include <nanogui/vscrollpanel.h>
//#include <nanogui/colorwheel.h>
//#include <nanogui/graph.h>
//#include <nanogui/tabwidget.h>
////#include <nanogui/glcanvas.h>
//#include <iostream>
//#include <string>
//
//// Includes for the GLTexture class.
//#include <cstdint>
//#include <memory>
//#include <utility>
//
//#if defined(__GNUC__)
//#  pragma GCC diagnostic ignored "-Wmissing-field-initializers"
//#endif
//#if defined(_WIN32)
//#  pragma warning(push)
//#  pragma warning(disable: 4457 4456 4005 4312)
//#endif
//
///*
//#define STB_IMAGE_IMPLEMENTATION
//#include <stb_image.h>
//*/
//
//#if defined(_WIN32)
//#  pragma warning(pop)
//#endif
//#if defined(_WIN32)
//#  if defined(APIENTRY)
//#    undef APIENTRY
//#  endif
//#  include <windows.h>
//#endif
//
//using std::cout;
//using std::cerr;
//using std::endl;
//using std::string;
//using std::vector;
//using std::pair;
//using std::to_string;
//
//
//class Canvas : public nanogui::GLCanvas {
//public:
//    Canvas(Widget *parent);
//
//    ~Canvas();
//
//    void setRotation(nanogui::Vector3f vRotation);
//
//    virtual void drawGL();
//
//private:
//    Eigen::Vector3f mRotation;
//};
