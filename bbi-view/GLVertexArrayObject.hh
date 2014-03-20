#ifndef _GLVERTEXARRAYOBJECT_H_
#define _GLVERTEXARRAYOBJECT_H_

//#import <OpenGL/OpenGL.h>
#import <OpenGL/gl3.h>
#include <iostream>

#include "GLError.hh"

#include <vector>
#include <map>
#include <string>

class GLVertexArrayObject
{
public:
    //
    // The VAO has a program from which it obtains attribute locations.
    //
    GLVertexArrayObject(GLuint program);
    ~GLVertexArrayObject();

    //
    // Knows how to draw itself.
    //
    void draw();
    //
    // Using this callback if given.
    //
    std::function<void()> drawCallback;
    //
    // Generates a single buffer for GL_ARRAY_BUFFER binding.
    // Copies data to this buffer.
    // The buffer object is owned by this VAO object in some respect.
    //
    // Does EnableVertexAttribArray for the attribute using hardcoded values!
    //
    // Type T requires the following attributes available:
    //     data.rows()
    //     data.cols()
    //     data.data()
    //     data.size()
    //
    // The only use case at the moment is an Eigen::Matrix<float, Dyn, Dyn>
    // but anything else satisfying the conditions should do.
    //
    template <typename T>
    void attributedBuffer(const T &data, std::string attribute);
    template <typename T>
    void texture1D(T const& image);
    template <typename T>
    void texture2D(T const& image);

    void elementBuffer(std::vector<GLuint> const& indices);
    
    GLenum elementIndexType;
    
    GLuint vaoID();
    GLenum primitiveType;

    //
    // Stores the buffer IDs for the attributes.
    //
    // This is used in the destructor to delete the bufferes.
    GLuint operator[](const std::string& attribute) const;
    GLsizei numElements;
    GLsizei numIndices;
    
private:
    GLuint vaoID_, programID_;
    std::map<std::string, GLuint> bufferNames_;
};


#include "GLVertexArrayObject.cc"
#endif /* _GLVERTEXARRAYOBJECT_H_ */
