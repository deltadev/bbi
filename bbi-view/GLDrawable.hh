#ifndef _GLDRAWABLE_H_
#define _GLDRAWABLE_H_

#include <memory>
#include <map>
#include <string>
#include <vector>

#include <Eigen/Geometry>

class GLProgram;
class GLVertexArrayObject;

class GLDrawable
{
    
public:
    typedef std::shared_ptr<GLProgram> ProgramPtr;
    typedef Eigen::Affine3d            Transform;
    typedef GLVertexArrayObject        VAO;
    
    GLDrawable(std::string shaderName = "default");
    GLDrawable(ProgramPtr program);
    virtual ~GLDrawable();

    //
    // c++11 member initialization...
    //
    bool show = true;
    std::string nameStr = "";
    Eigen::Affine3d transform = Eigen::Affine3d::Identity();
    

    ProgramPtr program;

    virtual void draw();
    
    //
    // This is for any uniforms that we want to dynamically control plots.
    //
    std::map<std::string, float> uniforms;
    std::vector<std::shared_ptr<VAO>> vaos;
    

};



#endif /* _GLDRAWABLE_H_ */
