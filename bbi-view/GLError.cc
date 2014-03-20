#include <iostream>
#include <sstream>
#include <string>

#include "GLError.hh"


GLError& GLError::instance()
{
    static GLError e;
    return e;
}
std::string GLError::errorString()
{
    std::ostringstream oss;
    GLenum err = glGetError();
    while (err != GL_NO_ERROR)
    {
        oss << errorString(err);
        err = glGetError();
    }
    return oss.str();
}

GLError::GLError() { };
GLError::~GLError() { };


std::string GLError::errorString(GLenum error) 
{
    std::string str;
    switch( error )
    {
        case GL_NO_ERROR:
            str = "GL_NO_ERROR";
            break;
        case GL_INVALID_ENUM:
            str = "GL_INVALID_ENUM";
            break;
        case GL_INVALID_VALUE:
            str = "GL_INVALID_VALUE";
            break;
        case GL_INVALID_OPERATION:
            str = "GL_INVALID_OPERATION";
            break;		
        case GL_OUT_OF_MEMORY:
            str = "GL_OUT_OF_MEMORY";
            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            str = "GL_INVALID_FRAMEBUFFER_OPERATION";
            break;
        default:
            str = "(ERROR: Unknown Error Enum)";
            break;
    }
    return str;
}

