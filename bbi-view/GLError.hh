#ifndef _GLERROR_H_
#define _GLERROR_H_

#include <OpenGL/gl3.h>

class GLError
{
public:
    static GLError& instance();
    static std::string errorString();
    static void printError();


private:

    GLError();
    ~GLError();
    GLError(const GLError& e);
    GLError& operator=(const GLError& e);
    
    static std::string errorString(GLenum error);
};


#endif /* _GLERROR_H_ */
