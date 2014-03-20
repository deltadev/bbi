#ifndef _GLRENDERER_H_
#define _GLRENDERER_H_

#include <vector>
#include <memory>

class GLDrawable;

class GLRenderer
{
public:
    float near, far, fov;
    float angleX, angleY, angleZ;
    float offX, offY, offZ;

    void cullFace(bool b);
    void clearColor(float r, float g, float b, float a);
    
    std::vector<std::shared_ptr<GLDrawable>> drawables;
    
    GLRenderer();
    void initGL();
    void render();
    void resizeViewport(float width, float height);
private:
    // The renderer only needs these for the aspect ratio.
    float viewWidth_, viewHeight_;
    
};


#endif /* _GLRENDERER_H_ */
