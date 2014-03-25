#ifndef DPJ_WIG_DRAWABLES_HH_
#define DPJ_WIG_DRAWABLES_HH_

#include "GLDrawable.hh"
#include "GLProgram.hh"
#include "GLVertexArrayObject.hh"

struct wig_data : GLDrawable
{
  wig_data(std::string shader = "zoom_data") : GLDrawable(shader) { }
  
  void add(std::vector<bbi::wig::fixed_step_record> const& data,
           long start, int step = 1, int span = 1)
  {
    program->useProgram();
    auto vao = std::make_shared<GLVertexArrayObject>(program->programID());
    
    int num_els = 6 * (int)data.size();
    
    Eigen::MatrixXd verts(3, num_els);
    Eigen::MatrixXf norms(Eigen::MatrixXf::Ones(3, num_els));
    
    int idx = 0;
    for (auto const& d : data)
    {
      verts.col(idx++) = Eigen::Vector3d{start, 0, 0};
      verts.col(idx++) = Eigen::Vector3d{start, d.val, 0};
      verts.col(idx++) = Eigen::Vector3d{start + span, d.val, 0};
      
      verts.col(idx++) = Eigen::Vector3d{start + span, d.val, 0};
      verts.col(idx++) = Eigen::Vector3d{start + span, 0, 0};
      verts.col(idx++) = Eigen::Vector3d{start, 0, 0};
      
      start += step;
    }
    
    Eigen::Vector3f color(0.5, 0.1, 0.0);
    Eigen::MatrixXf cols(color.rowwise().replicate(num_els));
    
    verts.colwise() -= verts.rowwise().mean();
    
    double norm = verts.row(0).norm();
    verts.row(0) /= 0.001 * norm;
    
    Eigen::MatrixXf fverts = verts.cast<float>();
    
    vao->attributedBuffer(verts, "aVertex");
    vao->attributedBuffer(norms, "aNormal");
    vao->attributedBuffer(cols, "aColor");
    vao->primitiveType = GL_TRIANGLES;
    vaos.push_back(vao);
  }
  
  void add(std::vector<bbi::wig::var_step_record> const& data, int span = 1)
  {
    program->useProgram();
    auto vao = std::make_shared<GLVertexArrayObject>(program->programID());
    
    int num_els = 6 * (int)data.size();
    
    Eigen::MatrixXd verts(3, num_els);
    Eigen::MatrixXf norms(Eigen::MatrixXf::Ones(3, num_els));
    
    int idx = 0;
    for (auto const& d : data)
    {
      verts.col(idx++) = Eigen::Vector3d{d.chrom_start, 0, 0};
      verts.col(idx++) = Eigen::Vector3d{d.chrom_start, d.val, 0};
      verts.col(idx++) = Eigen::Vector3d{d.chrom_start + span, d.val, 0};
      
      verts.col(idx++) = Eigen::Vector3d{d.chrom_start + span, d.val, 0};
      verts.col(idx++) = Eigen::Vector3d{d.chrom_start + span, 0, 0};
      verts.col(idx++) = Eigen::Vector3d{d.chrom_start, 0, 0};
    }
    
    Eigen::Vector3f color(0.0, 0.5, 0.1);
    Eigen::MatrixXf cols(color.rowwise().replicate(num_els));
    
    verts.colwise() -= verts.rowwise().mean();
    
    double norm = verts.row(0).norm();
    verts.row(0) /= 0.001 * norm;
    
    Eigen::MatrixXf fverts = verts.cast<float>();
    
    vao->attributedBuffer(verts, "aVertex");
    vao->attributedBuffer(norms, "aNormal");
    vao->attributedBuffer(cols, "aColor");
    vao->primitiveType = GL_TRIANGLES;
    vaos.push_back(vao);
  }
  void add(std::vector<bbi::wig::bed_graph_record> const& data, float val = 100)
  {
    program->useProgram();
    auto vao = std::make_shared<GLVertexArrayObject>(program->programID());
    
    int num_els = 6 * (int)data.size();
    
    Eigen::MatrixXd verts(3, num_els);
    Eigen::MatrixXf norms(Eigen::MatrixXf::Ones(3, num_els));
    
    int idx = 0;
    for (auto const& d : data)
    {
      verts.col(idx++) = Eigen::Vector3d{d.chrom_start, 0, 0};
      verts.col(idx++) = Eigen::Vector3d{d.chrom_start, val, 0};
      verts.col(idx++) = Eigen::Vector3d{d.chrom_start + d.chrom_end, val, 0};
      
      verts.col(idx++) = Eigen::Vector3d{d.chrom_start + d.chrom_end, val, 0};
      verts.col(idx++) = Eigen::Vector3d{d.chrom_start + d.chrom_end, 0, 0};
      verts.col(idx++) = Eigen::Vector3d{d.chrom_start, 0, 0};
    }
    
    Eigen::Vector3f color(0.0, 0.1, 0.5);
    Eigen::MatrixXf cols(color.rowwise().replicate(num_els));
    
    verts.colwise() -= verts.rowwise().mean();
    
    double norm = verts.row(0).norm();
    verts.row(0) /= 0.001 * norm;
    
    Eigen::MatrixXf fverts = verts.cast<float>();
    
    vao->attributedBuffer(verts, "aVertex");
    vao->attributedBuffer(norms, "aNormal");
    vao->attributedBuffer(cols, "aColor");
    vao->primitiveType = GL_TRIANGLES;
    vaos.push_back(vao);
  }
};


#endif /* DPJ_WIG_DRAWABLES_HH_ */
