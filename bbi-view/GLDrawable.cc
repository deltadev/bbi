#include "GLDrawable.hh"
#include "GLProgram.hh"
#include "GLVertexArrayObject.hh"

GLDrawable::GLDrawable(std::string shaderName)
    : program(std::make_shared<GLProgram>(shaderName)) { }

GLDrawable::GLDrawable(ProgramPtr program) : program(program) { }

GLDrawable::~GLDrawable() { }

void GLDrawable::draw()
{
    program->useProgram();
    for (auto& vao : vaos)
    {
        vao->draw();
    }
}