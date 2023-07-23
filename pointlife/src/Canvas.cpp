#include "Canvas.h"
#include "Globals.h"

using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::vector;
using std::pair;
using std::to_string;

using namespace nanogui;


RenderLock::RenderLock() {
  pthread_mutex_lock( &mutex );
}

RenderLock::~RenderLock() {
  pthread_mutex_unlock( &mutex );
}


Canvas::Canvas(Widget *parent) : nanogui::GLCanvas(parent), mRotation(nanogui::Vector3f(0.25, 0.5, 0.33))
{
}

~Canvas::Canvas()
{
  mShader.free();
}

void Canvas::setRotation(nanogui::Vector3f vRotation)
{
  mRotation = vRotation;
}

virtual void Canvas::drawGL() override
{
  this->

  /*
        mShader.bind();

        Matrix4f mvp;
        mvp.setIdentity();
        float fTime = (float)glfwGetTime();
        mvp.topLeftCorner<3,3>() = Eigen::Matrix3f(Eigen::AngleAxisf(mRotation[0]*fTime, Vector3f::UnitX()) *
                                                   Eigen::AngleAxisf(mRotation[1]*fTime,  Vector3f::UnitY()) *
                                                   Eigen::AngleAxisf(mRotation[2]*fTime, Vector3f::UnitZ())) * 0.25f;

        mShader.setUniform("modelViewProj", mvp);

        glEnable(GL_DEPTH_TEST);
        // Draw 12 triangles starting at index 0 
        mShader.drawIndexed(GL_TRIANGLES, 0, 12);
        glDisable(GL_DEPTH_TEST);
        */
}
