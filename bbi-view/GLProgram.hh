#ifndef _GLPROGRAM_H_
#define _GLPROGRAM_H_

//#import <OpenGL/OpenGL.h>
#import <OpenGL/gl3.h>

#include <string>
#include <vector>
#include <map>
#include <regex>


//
// Class to create a program with attached shaders.
//
// Sets up attribLocations.
// Retrieves uniformLocation.
// Handles error reporting.
//
class GLProgram
{
public:
    
    GLProgram(std::string shaderName);
    GLProgram(std::vector<std::string> shaders);
    ~GLProgram();
    
    //
    // These are bound with glAttribLocation.
    //
    enum {
        VERTEX_ATTRIB_IDX,     // "aVertex"
        COLOR_ATTRIB_IDX,   // "aColor"
        NORMAL_ATTRIB_IDX,  // "aNormal"
        TEXCOORD_ATTRIB_IDX // "aTexCoord"
    };

    //
    // This is obtained from the shader side and so must be called 
    // after compileAndLink().
    //
    GLuint uniformLocation(std::string uniform);
    
    //
    // Validation should only take place after a VAO has been bound.
    //
    void validateProgram();
    void useProgram();
    
    GLuint programID();
    
    std::map<std::string, GLuint> shaderIDs;
    std::string const& shaderName();
private:
    void linkProgram();

    void compileShader(GLuint shader);
    void compileShaders(std::vector<std::string> shaders);
    void checkProgram(GLenum statusCheck);
    
    GLuint programID_;
    std::string shaderName_;
    std::regex vshRE_, fshRE_, gshRE_;
};

#endif /* _GLPROGRAM_H_ */
