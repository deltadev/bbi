#ifndef DPJ_PROGRAM_HH_
#define DPJ_PROGRAM_HH_

#include <OpenGL/gl3.h>

#include <vector>
#include <string>

class program_t
{
public:
  program_t(std::vector<std::string> sources)
  : sources(std::move(sources)), id{glCreateProgram()} { }

  ~program_t()
  {
    if (id == 0)

GLuint id;
  std::vector<std::string> sources;

  
};


#endif /* DPJ_PROGRAM_HH_ */
