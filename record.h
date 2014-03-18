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
  
  // Extracts records when we don't know how large the records are.
  //
  // - we rely on the InputIterator to tell us when the input ends.
  //
  template<typename T> std::vector<T> extract(std::istringstream& is)
  {
    std::vector<T> data;
    while (is)
    {
      T t{is};
      if (t.chrom_start == t.chrom_end)
        break;
      data.push_back(t);
    }

    return data;
  }
  
  template<typename T> std::vector<T> extract(std::istringstream& is, unsigned count)
  {
    std::vector<T> data(count);
    std::generate(begin(data), end(data), [&]() -> T { return is; });
    return data;
  }

  // Helper for function template specialization.
  //
  template <typename T> struct is_wig_type : std::false_type { };

}

#endif /* DPJ_RECORD_H_ */
