#ifndef DPJ_DRAWABLES_HH_
#define DPJ_DRAWABLES_HH_

#include "GLDrawable.hh"
#include "GLProgram.hh"
#include "GLVertexArrayObject.hh"

struct zoom_data : GLDrawable
{
  zoom_data(std::string shader = "zoom_data") : GLDrawable(shader) { }
  
  void set_data(std::vector<bbi::zoom::record> const& data)
  {
    program->useProgram();
    auto vao = std::make_shared<GLVertexArrayObject>(program->programID());
    
    int num_els = 6 * (int)data.size();
    
    Eigen::MatrixXd verts(3, num_els);
    Eigen::MatrixXf norms(Eigen::MatrixXf::Ones(3, num_els));
    
    int idx = 0;
    for (auto const& d : data)
    {

      
      verts.col(idx++) = Eigen::Vector3d{d.chrom_start, d.min_val, 0};
      verts.col(idx++) = Eigen::Vector3d{d.chrom_start, d.max_val, 0};
      verts.col(idx++) = Eigen::Vector3d{d.chrom_end, d.max_val, 0};
      
      verts.col(idx++) = Eigen::Vector3d{d.chrom_end, d.max_val, 0};
      verts.col(idx++) = Eigen::Vector3d{d.chrom_end, d.min_val, 0};
      verts.col(idx++) = Eigen::Vector3d{d.chrom_start, d.min_val, 0};
    }
    
    Eigen::Vector3f color(0.9, 0.1, 0.9);
    Eigen::MatrixXf cols(color.rowwise().replicate(num_els));
    
    // Centres the data.
    //
    verts.colwise() -= verts.rowwise().mean();

    
    double norm = verts.row(0).norm();
    verts.row(0) /= 0.001 * norm;


    
    Eigen::MatrixXf fverts = verts.cast<float>();
    
    vao->attributedBuffer(fverts, "aVertex");
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
    
    Eigen::MatrixXd verts(3, num_els);
    Eigen::MatrixXf norms(Eigen::MatrixXf::Ones(3, num_els));
    
    int idx = 0;
    for (auto const& d : data)
    {
      verts.col(idx++) = Eigen::Vector3d{d.chrom_start, 0, 0};
      verts.col(idx++) = Eigen::Vector3d{d.chrom_start, d.val, 0};
      verts.col(idx++) = Eigen::Vector3d{d.chrom_start + 1, d.val, 0};
      
      verts.col(idx++) = Eigen::Vector3d{d.chrom_start + 1, d.val, 0};
      verts.col(idx++) = Eigen::Vector3d{d.chrom_start + 1, 0, 0};
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
};


#endif /* DPJ_DRAWABLES_HH_ */
