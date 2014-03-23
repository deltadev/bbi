#ifndef DPJ_RECORD_HH_
#define DPJ_RECORD_HH_

#include <cstdint>
#include <iosfwd>
#include <iomanip>
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
    
    
    //void pack(std::ostream& os) const;
    
    
    friend void print(record const& r, std::ostream& os)
    {
      os << std::setw(25) << std::left << "chrom_id" << r.chrom_id << '\n';
      os << std::setw(25) << std::left << "chrom_start" << r.chrom_start << '\n';
      os << std::setw(25) << std::left << "chrom_end" << r.chrom_end << '\n';
    }
    
    
    friend void unpack(record& r, std::streambuf* s)
    {
      s->sgetn((char*)&r.chrom_id, sizeof r.chrom_id);
      s->sgetn((char*)&r.chrom_start, sizeof r.chrom_start);
      s->sgetn((char*)&r.chrom_end, sizeof r.chrom_end);
    }
  };
  
  // Helpers for function template specialization.
  //
  template <typename T> struct bbi_type : std::false_type { };
  
}



template <typename T>
typename std::enable_if<bbi::bbi_type<T>::value, std::ostream&>::type
operator<<(std::ostream& os, T const& r) { print(r, os); return os; }

// TODO: this doesn't set the fail bit correctly on streambuf eof.
//
//template <typename T>
//typename std::enable_if<bbi::bbi_type<T>::value, std::istream&>::type
//operator>>(std::istream& is, T& r) { unpack(r, is.rdbuf()); return is; }


template <typename T>
typename std::vector<T> extract(bbi::record const& r, std::streambuf& s)
{
  std::vector<T> rs;
  while (s.in_avail() > 0)
  {
    // TODO: we could break out of this early assuming the records are ordered.
    //
    T t{&s};
    if (r.chrom_id == t.chrom_id && t.chrom_start < r.chrom_end && r.chrom_start < t.chrom_end)
      rs.push_back(t);
  }
  return rs;
}
#endif /* DPJ_RECORD_HH_ */
