#ifndef DPJ_POINT_CLOUD_HH_
#define DPJ_POINT_CLOUD_HH_

#include <vector>

#include "program.hh"
#include "buffer.hh"

using namespace dpj;
using std::vector;

namespace
{
  std::vector<float> data;

}

struct point_cloud
{

  struct point
  {
    float x, y, z;
    friend
    std::istream& operator>>(std::istream& is, point& p) { return is >> p.x >> p.y >> p.z; }
  };
  using point_data_t = std::vector<point>;
  
  gl::program_t prog;

  GLuint vao;
  GLint u_loc;
  point mins, maxs, sums;

  gl::transform t;
  
  point_cloud() { }
  point_cloud(gl::program_t prog)
  : prog{prog}
  , u_loc{unif_loc(prog, "u_transform")}
  , mins{FLT_MAX, FLT_MAX, FLT_MAX}
  , maxs{FLT_MIN, FLT_MIN, FLT_MIN}
  , sums{0}
  { glGenVertexArrays(1, &vao); }
  

  friend
  int num_elems(point_cloud const& pc) { return static_cast<int>(data.size()) / 3; }

  friend
  point_cloud add(point_cloud& pc, point_data_t const& d)
  {
    for (auto p : d)
    {
      data.push_back(p.x);
      data.push_back(p.y);
      data.push_back(p.z);
    }
    
    // Translates to center.
    //
    gl::transform trans;
    trans(0, 3) = -pc.sums.x / num_elems(pc);
    trans(1, 3) = -pc.sums.y / num_elems(pc);
    trans(2, 3) = -pc.sums.z / num_elems(pc);
    
    // Scales to unit box.
    //
    gl::transform scale;
    scale(0,0) = 2.0 / std::max<float>((pc.maxs.x - pc.mins.x), 1);
    scale(1,1) = 2.0 / std::max<float>((pc.maxs.y - pc.mins.y), 1);
    scale(2,2) = 2.0 / std::max<float>((pc.maxs.z - pc.mins.z), 1);


    pc.t = scale * trans;
    
    glBindVertexArray(pc.vao);
    
    load(buffer(gl::global, "a_pos"), GL_ARRAY_BUFFER, data);
    attr_ptr(pc.prog, "a_pos", 3);
    
    validate(pc.prog);
    
    return pc;
  }
  
  friend
  point_data_t parse_points(point_cloud& pc, std::istream& is)
  {
    point_data_t points;
    point p;
    while (is >> p)
    {
      if (p.x < pc.mins.x)
        pc.mins.x = p.x;
      else if (pc.maxs.x < p.x)
        pc.maxs.x = p.x;
      pc.sums.x += p.x;
      
      if (p.y < pc.mins.y)
        pc.mins.y = p.y;
      else if (pc.maxs.y < p.y)
        pc.maxs.y = p.y;
      pc.sums.y += p.y;
      
      if (p.z < pc.mins.z)
        pc.mins.z = p.z;
      else if (pc.maxs.z < p.z)
        pc.maxs.z = p.z;
      pc.sums.z += p.z;
      
      points.emplace_back(p);
    }
    return points;
  }
  
  
  
  friend
  void draw(point_cloud const& pc, gl::renderer const& r)
  {
    use(pc.prog);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBindVertexArray(pc.vao);
    
    // Uses the pc's transform to scale to unit box.
    //
    glUniformMatrix4fv(pc.u_loc, 1, GL_TRUE /* row major*/, pc.t.data());

    glDrawArrays(GL_POINTS, 0, (int)num_elems(pc));
  }
  
};

#endif /* DPJ_POINT_CLOUD_HH_ */
