#ifndef DPJ_DRAWABLES_HH_
#define DPJ_DRAWABLES_HH_

#include "GLDrawable.hh"

struct zoom_data : GLDrawable
{
  zoom_data(std::string shader = "zoom_data") : GLDrawable(shader) { }
  
  void set_data(std::vector<bbi::zoom::record> const& data)
  {
    program->useProgram();
    auto vao = std::make_shared<GLVertexArrayObject>(program->programID());
    
    int num_els = 6 * (int)data.size();
    
    Eigen::MatrixXf verts(3, num_els);
    Eigen::MatrixXf norms(Eigen::MatrixXf::Ones(3, num_els));
    
    int idx = 0;
    for (auto const& d : data)
    {
      verts.col(idx++) = Eigen::Vector3f{d.chrom_start, d.min_val, 0};
      verts.col(idx++) = Eigen::Vector3f{d.chrom_start, d.max_val, 0};
      verts.col(idx++) = Eigen::Vector3f{d.chrom_end, d.max_val, 0};
      
      verts.col(idx++) = Eigen::Vector3f{d.chrom_end, d.max_val, 0};
      verts.col(idx++) = Eigen::Vector3f{d.chrom_end, d.min_val, 0};
      verts.col(idx++) = Eigen::Vector3f{d.chrom_start, d.min_val, 0};
    }
    
    Eigen::Vector3f color(0.0, 0.5, 0.1);
    Eigen::MatrixXf cols(color.rowwise().replicate(num_els));
    
    verts.colwise() -= verts.rowwise().mean();
    
    vao->attributedBuffer(verts, "aVertex");
    vao->attributedBuffer(norms, "aNormal");
    vao->attributedBuffer(cols, "aColor");
    vao->primitiveType = GL_TRIANGLES;
    vaos.push_back(vao);
  }
};
struct wig_data : GLDrawable
{
  wig_data(std::string shader = "zoom_data") : GLDrawable(shader) { }
  
  void set_data(std::vector<bbi::wig::var_step_record> const& data)
  {
    program->useProgram();
    auto vao = std::make_shared<GLVertexArrayObject>(program->programID());
    
    int num_els = 6 * (int)data.size();
    
    Eigen::MatrixXf verts(3, num_els);
    Eigen::MatrixXf norms(Eigen::MatrixXf::Ones(3, num_els));
    
    int idx = 0;
    for (auto const& d : data)
    {
      verts.col(idx++) = Eigen::Vector3f{d.chrom_start, 0, 0};
      verts.col(idx++) = Eigen::Vector3f{d.chrom_start, d.val, 0};
      verts.col(idx++) = Eigen::Vector3f{d.chrom_start + 1, d.val, 0};
      
      verts.col(idx++) = Eigen::Vector3f{d.chrom_start + 1, d.val, 0};
      verts.col(idx++) = Eigen::Vector3f{d.chrom_start + 1, 0, 0};
      verts.col(idx++) = Eigen::Vector3f{d.chrom_start, 0, 0};
    }
    
    Eigen::Vector3f color(0.0, 0.5, 0.1);
    Eigen::MatrixXf cols(color.rowwise().replicate(num_els));
    
    verts.colwise() -= verts.rowwise().mean();
    
    vao->attributedBuffer(verts, "aVertex");
    vao->attributedBuffer(norms, "aNormal");
    vao->attributedBuffer(cols, "aColor");
    vao->primitiveType = GL_TRIANGLES;
    vaos.push_back(vao);
  }
};


#endif /* DPJ_DRAWABLES_HH_ */
