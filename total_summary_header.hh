#ifndef DPJ_TOTAL_SUMMARY_HEADER_H_
#define DPJ_TOTAL_SUMMARY_HEADER_H_

#include <cstdint>
#include <iostream>

struct total_summary_header
{
  uint64_t bases_covered;
  double min_val;
  double max_val;
  double sum_data;
  double sum_squares;

  void print(std::ostream& os) const;
  void pack(std::ostream& os) const;
  void unpack(std::istream& os);
};


#endif /* DPJ_TOTAL_SUMMARY_HEADER_H_ */
