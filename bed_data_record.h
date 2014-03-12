#ifndef DPJ_BED_DATA_RECORD_H_
#define DPJ_BED_DATA_RECORD_H_

#include "data_record.h"

#include <string>
/////////////////////////////////////////////////////////////////////
// bed_data_record
// 
/////////////////////////////////////////////////////////////////////
struct bed_data_record : data_record
{

  std::string rest;

  void print(std::ostream& os) const;
  void pack(std::ostream& os) const;
  void unpack(std::istream& os);

};


#endif /* DPJ_BED_DATA_RECORD_H_ */
