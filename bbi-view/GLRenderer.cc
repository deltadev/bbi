#include <iostream>
#include <sstream>

#include "GLRenderer.hh"
#include "GLError.hh"
#include "GLProgram.hh"
#include "GLDrawable.hh"

#include "logger.hh"
#include <OpenGL/gl3.h>

#include <Eigen/Dense>
#include <Eigen/Geometry>

namespace E = Eigen;
typedef E::Matrix4d matd4;
typedef E::Vector3d vecd3;


const double PI = std::atan(1.0)*4;

//struct GLRendererParms
//{
//    float viewWidth, viewHeight;
//    float near, far, fov;
//    float offX, offY, offZ;
//    float angleX, angleY, angleZ;
//};


GLRenderer::GLRenderer()
    : viewWidth_(1), viewHeight_(1), // aspect ratio only
      near(0.1), far(2000), fov(45),
      offX(0), offY(0), offZ(-50),
      angleX(0), angleY(0), angleZ(0)
{ }
void GLRenderer::cullFace(bool b) { b ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE); }

void GLRenderer::initGL()
{
    LOG_INFO(std::string((char*)glGetString(GL_RENDERER)));
    LOG_INFO(std::string((char*)glGetString(GL_VERSION)));
    GLint sz = 0;
    glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &sz);

    sz = 0;
    glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &sz);
  
    glEnable(GL_PROGRAM_POINT_SIZE);
    
    // Depth test will always be enabled
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LEQUAL);
    glDepthRange(0.0f, 1.0f);
    
    // We will always cull back faces for better performance
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CW);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // default
    glClearColor(1.0,1.0,1.0,0);

    glClearDepth(1.0f);

    // Warm up.
    render();
}
void GLRenderer::clearColor(float r, float g, float b, float a) { glClearColor(r, g, b, a); }

void GLRenderer::render()
{

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //
    // View and projection matrix.
    //
    double f = 1.0 / tanf(fov * PI / 360.0);
    double aspect = viewWidth_ / viewHeight_;
    matd4 p;
    p <<
      f / aspect, 0, 0,                 0,
      0,          f, 0,                 0,
      0,          0, (far + near) / (near - far), 2 * far * near / (near - far),
      0,          0, -1,                0;
    E::Projective3d projection(p);
    
    //
    // Model matrix.
    //
    vecd3 offset(offX, offY, offZ);
    vecd3 e1(1, 0, 0);
    vecd3 e2(0, 1, 0);
    E::Projective3d view(// Eye translation
                         E::Translation3d(offset) *
                         // rotate-animate
                         E::AngleAxisd(PI * angleY / 360.0, e2) *
                         E::AngleAxisd(PI * angleX / 360.0, e1)
                         );

    for (auto& drawable : drawables)
    {
        drawable->program->useProgram();
        E::Affine3d model(drawable->transform);

        E::Matrix3f normal(model.matrix().inverse().topLeftCorner(3, 3).transpose().cast<float>());

        E::Projective3d modelView = view * model;
        E::Projective3d modelViewProjection = projection * modelView;
        
//        GLInfo::instance().activeUniformNames(drawable->program->programID());
//        GLInfo::instance().programInfo(drawable->program->programID());
        

        GLint loc = drawable->program->uniformLocation("uModelViewProjectionMatrix");

        glUniformMatrix4fv(loc, 1, GL_FALSE, modelViewProjection.cast<float>().data());
        
        loc = drawable->program->uniformLocation("uModelViewMatrix");
        glUniformMatrix4fv(loc, 1, GL_FALSE, view.cast<float>().data());
        
        loc = drawable->program->uniformLocation("uNormalMatrix");
        glUniformMatrix3fv(loc, 1, GL_FALSE, normal.data());
        
        if (drawable->show)
        {
            drawable->draw();
        }
    }
}

void GLRenderer::resizeViewport(float width, float height)
{
    viewWidth_ = width;
    viewHeight_ = height;
    glViewport(0, 0, (GLsizei)width, (GLsizei)height);
}
