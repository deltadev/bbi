#ifndef DPJ_RECORD_H_
#define DPJ_RECORD_H_

#include <cstdint>
#include <iosfwd>

#include <sstream>

#include <vector>

#include <type_traits>

////////////////////////////////////////////////////////////////////////////
// record
//
//     This is simply for code re-use. Only intended for static dispatch so
//     no virtual methods are needed.
//
////////////////////////////////////////////////////////////////////////////


namespace bbi
{
  
  struct record
  {
    uint32_t chrom_id;
    uint32_t chrom_start;
    uint32_t chrom_end;

    void print(std::ostream& os) const;
    void pack(std::ostream& os) const;
    void unpack(std::istream& is);
    
  };
  

    // Helpers for function template specialization.
  //
  template <typename T> struct is_wig_type : std::false_type { };

  template <typename T>
  using wig_type = std::enable_if<bbi::is_wig_type<T>::value, T>;
  
  template <typename T>
  using other_type = std::enable_if<!bbi::is_wig_type<T>::value, T>;

}

#endif /* DPJ_RECORD_H_ */
