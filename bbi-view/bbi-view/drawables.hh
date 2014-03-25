#ifndef DPJ_DRAWABLES_HH_
#define DPJ_DRAWABLES_HH_

#include "GLDrawable.hh"
#include "GLProgram.hh"
#include "GLVertexArrayObject.hh"
#include "wig_drawables.hh"

struct zoom_data : GLDrawable
{
  zoom_data(std::string shader = "zoom_data") : GLDrawable(shader) { }
  
  void add(std::vector<bbi::zoom::record> const& data, int block)
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
    
    Eigen::Vector3f color(0.9, 0.9, 0.5);
    Eigen::MatrixXf cols(color.rowwise().replicate(num_els));
    
    // Centres the data.
    //
    Eigen::Vector3d mins = verts.rowwise().minCoeff();
    Eigen::Vector3d maxs = verts.rowwise().maxCoeff();
    Eigen::Vector3d means = verts.rowwise().mean();
    Eigen::Vector3d ranges = maxs - mins;
    

    verts.row(0).array() -= mins[0];
    verts.row(0).array() /= ranges[0];
    verts.row(0).array() += block;
    verts.row(0).array() *= 100;

    verts.row(1).array() -= mins[1];
    verts.row(1).array() /= 0.1 * means[1];
    
    Eigen::MatrixXf fverts = verts.cast<float>();
    
    vao->attributedBuffer(fverts, "aVertex");
    vao->attributedBuffer(norms, "aNormal");
    vao->attributedBuffer(cols, "aColor");
    vao->primitiveType = GL_TRIANGLES;
    vaos.push_back(vao);
  }
};


#endif /* DPJ_DRAWABLES_HH_ */
