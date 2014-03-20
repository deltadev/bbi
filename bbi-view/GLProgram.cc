#include <OpenGL/gl3.h>
#include <iostream>
#include <fstream>
#include <iterator>
#include <regex>
#include "GLProgram.hh"
#include "GLError.hh"

#include "Logger.hh"

namespace
{
    typedef std::istreambuf_iterator<char> I;
    std::string readFile(std::string fileName)
    {
        std::ifstream inFile(fileName.c_str());
        if (!inFile.good())
        {
            LOG_ERROR(fileName);
            std::exit(EXIT_FAILURE);
        }
        return std::string(I(inFile), I());
    }
}

GLProgram::GLProgram(std::string shaderName)
    : shaderName_(shaderName),
      programID_(glCreateProgram()),
      vshRE_("\\.vsh|\\.vert"),
      fshRE_("\\.fsh|\\.frag"),
      gshRE_("\\.gsh|\\.geom")
{
    if (programID_ == 0)
    {
        LOG_ERROR("failed glCreateProgram() call.");
        std::exit(EXIT_FAILURE);
    }
    
    glBindAttribLocation(programID_, VERTEX_ATTRIB_IDX, "aVertex");
    glBindAttribLocation(programID_, COLOR_ATTRIB_IDX, "aColor");
    glBindAttribLocation(programID_, NORMAL_ATTRIB_IDX, "aNormal");
    glBindAttribLocation(programID_, TEXCOORD_ATTRIB_IDX, "aTexCoord");

    std::vector<std::string> shaders{shaderName + ".vsh", shaderName + ".fsh"};
    compileShaders(shaders);
    linkProgram();

    LOG_INFO(shaderName);
}

GLProgram::GLProgram(std::vector<std::string> shaders)
    : programID_(glCreateProgram()),
      vshRE_("\\.vsh|\\.vert"),
      fshRE_("\\.fsh|\\.frag"),
      gshRE_("\\.gsh|\\.geom")
{
    if (programID_ == 0)
    {
        LOG_ERROR("failed to create gl program!");
        std::exit(EXIT_FAILURE);

    }
    
    glBindAttribLocation(programID_, VERTEX_ATTRIB_IDX, "aVertex");
    glBindAttribLocation(programID_, COLOR_ATTRIB_IDX, "aColor");
    glBindAttribLocation(programID_, NORMAL_ATTRIB_IDX, "aNormal");
    glBindAttribLocation(programID_, TEXCOORD_ATTRIB_IDX, "aTexCoord");

    compileShaders(shaders);
    linkProgram();
    LOG_INFO(shaderName_);
}

GLProgram::~GLProgram()
{
    if (programID_ != 0)
    {
        GLsizei shaderCount;
        
        glGetProgramiv(programID_, GL_ATTACHED_SHADERS, &shaderCount);
        
        std::vector<GLuint> shaders(shaderCount);
        glGetAttachedShaders(programID_, shaderCount, &shaderCount, shaders.data());
        shaders.resize(shaderCount);
        for (GLuint shader : shaders)
        {
            glDeleteShader(shader);
        }
        glDeleteProgram(programID_);
        glUseProgram(0);
    }
}

void GLProgram::compileShaders(std::vector<std::string> shaders)
{
    for (auto const& shader : shaders)
    {
        std::string source = readFile(shader);
        GLuint shaderID = 0;
        if (std::regex_search(std::begin(shader), std::end(shader), vshRE_))
        {
            shaderID = glCreateShader(GL_VERTEX_SHADER);
        }
        else if (std::regex_search(std::begin(shader), std::end(shader), fshRE_))
        {
            shaderID = glCreateShader(GL_FRAGMENT_SHADER);
        }
        else if (std::regex_search(std::begin(shader), std::end(shader), gshRE_))
        {
            shaderID = glCreateShader(GL_GEOMETRY_SHADER);
        }
        shaderIDs[shader] = shaderID;
        
        char const* sourcePtr(source.data());
        GLint sourceLength((GLint)source.size());
        glShaderSource(shaderID, 1, &sourcePtr, &sourceLength);
        compileShader(shaderID);
        glAttachShader(programID_, shaderID);
    }
}



void GLProgram::linkProgram()
{
    glLinkProgram(programID_);
    checkProgram(GL_LINK_STATUS);
    LOG_INFO(std::to_string(programID_) + " is programID_.");
}
void GLProgram::validateProgram()
{
    glValidateProgram(programID_);
    checkProgram(GL_VALIDATE_STATUS);
    LOG_INFO(std::to_string(programID_) + " is programID_.");
}

GLuint GLProgram::uniformLocation(std::string uniform)
{
    GLint loc = glGetUniformLocation(programID_, uniform.c_str());
    if(loc < 0)
    {
        LOG_INFO(uniform + " missing for shader: " + shaderName_);
    }
    return loc;
}

//
// Accessors.
//
std::string const& GLProgram::shaderName() { return shaderName_; }
void GLProgram::useProgram() { glUseProgram(programID_); }
GLuint GLProgram::programID() { return programID_; }

//
// Private
//
void GLProgram::checkProgram(GLenum statusCheck)
{
    GLint logLength, status;
    glGetProgramiv(programID_, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0) 
    {
        std::vector<GLchar> log(logLength);
        glGetProgramInfoLog(programID_, logLength, &logLength, log.data());
        LOG_INFO(std::string(log.data(), (size_t)logLength - 1));
    }
    
    glGetProgramiv(programID_, statusCheck, &status);
    if (status == 0)
    {
        LOG_ERROR("Failed check gl program.");
        exit(EXIT_FAILURE);
    }
    
}

void GLProgram::compileShader(GLuint shader)
{
    GLint logLength, status;
    glCompileShader(shader);
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0) 
    {
        std::vector<GLchar> log(logLength);
        glGetShaderInfoLog(shader, logLength, &logLength, log.data());
        LOG_INFO(std::string(log.data(), (size_t)logLength));;
    }
    
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == 0)
    {
        LOG_ERROR("Failed to compile shader.");;
        exit(EXIT_FAILURE);
    }
}





























