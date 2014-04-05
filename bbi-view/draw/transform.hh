#ifndef DPJ_TRANSFORM_HH_
#define DPJ_TRANSFORM_HH_

#include <array>

namespace dpj
{
  namespace gl
  {
    struct transform
    {
      std::array<float, 16> m
      {{
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
      }};
      
      float& operator()(int i, int j) { return m[4 * i + j]; }
      float const* data() const { return m.data(); }
      
      friend
      transform operator*(transform a, transform b)
      {
        transform r;
        r(0,0)=a(0,0)*b(0,0)+a(0,1)*b(1,0)+a(0,2)*b(2,0)+a(0,3)*b(3,0);
        r(0,1)=a(0,0)*b(0,1)+a(0,1)*b(1,1)+a(0,2)*b(2,1)+a(0,3)*b(3,1);
        r(0,2)=a(0,0)*b(0,2)+a(0,1)*b(1,2)+a(0,2)*b(2,2)+a(0,3)*b(3,2);
        r(0,3)=a(0,0)*b(0,3)+a(0,1)*b(1,3)+a(0,2)*b(2,3)+a(0,3)*b(3,3);
        r(1,0)=a(1,0)*b(0,0)+a(1,1)*b(1,0)+a(1,2)*b(2,0)+a(1,3)*b(3,0);
        r(1,1)=a(1,0)*b(0,1)+a(1,1)*b(1,1)+a(1,2)*b(2,1)+a(1,3)*b(3,1);
        r(1,2)=a(1,0)*b(0,2)+a(1,1)*b(1,2)+a(1,2)*b(2,2)+a(1,3)*b(3,2);
        r(1,3)=a(1,0)*b(0,3)+a(1,1)*b(1,3)+a(1,2)*b(2,3)+a(1,3)*b(3,3);
        r(2,0)=a(2,0)*b(0,0)+a(2,1)*b(1,0)+a(2,2)*b(2,0)+a(2,3)*b(3,0);
        r(2,1)=a(2,0)*b(0,1)+a(2,1)*b(1,1)+a(2,2)*b(2,1)+a(2,3)*b(3,1);
        r(2,2)=a(2,0)*b(0,2)+a(2,1)*b(1,2)+a(2,2)*b(2,2)+a(2,3)*b(3,2);
        r(2,3)=a(2,0)*b(0,3)+a(2,1)*b(1,3)+a(2,2)*b(2,3)+a(2,3)*b(3,3);
        r(3,0)=a(3,0)*b(0,0)+a(3,1)*b(1,0)+a(3,2)*b(2,0)+a(3,3)*b(3,0);
        r(3,1)=a(3,0)*b(0,1)+a(3,1)*b(1,1)+a(3,2)*b(2,1)+a(3,3)*b(3,1);
        r(3,2)=a(3,0)*b(0,2)+a(3,1)*b(1,2)+a(3,2)*b(2,2)+a(3,3)*b(3,2);
        r(3,3)=a(3,0)*b(0,3)+a(3,1)*b(1,3)+a(3,2)*b(2,3)+a(3,3)*b(3,3);
        
        return r;
      }
    };
  }
}
#endif /* DPJ_TRANSFORM_HH_ */
