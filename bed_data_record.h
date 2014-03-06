#ifndef _DPJ_BED_DATA_RECORD_H_
#define _DPJ_BED_DATA_RECORD_H_

#include "data_record.h"

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


#endif /* _DPJ_BED_DATA_RECORD_H_ */
