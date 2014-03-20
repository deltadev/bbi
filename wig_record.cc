#include "wig_record.h"

#include <iosfwd>
#include <iomanip>

#include <string>
#include <sstream>

///////////////////////////////////////////////////////////////////////////
// wig_record
//
///////////////////////////////////////////////////////////////////////////
namespace bbi
{
  
  namespace wig
  {

    
    header::header() { }
    header::header(uint8_t* bytes)
    {
      std::stringstream iss{{bytes, bytes + sizeof *this}};
      this->unpack(iss);
    }
    header::header(std::istream& is)
    {
      this->unpack(is);
    }

    record_type header::record_type() { return static_cast<enum record_type>(type); }
    
    void header::print(std::ostream& os) const {
      bbi::record::print(os);
      os << std::setw(25) << std::left << "item_step" << item_step << '\n';
      os << std::setw(25) << std::left << "item_span" << item_span << '\n';
      os << std::setw(25) << std::left << "type" << type << '\n';
      os << std::setw(25) << std::left << "reserved" << reserved << '\n';
      os << std::setw(25) << std::left << "item_count" << item_count << '\n';
    }
    
    void header::pack(std::ostream& os) const {
      bbi::record::pack(os);
      os.write((char*)&item_step, sizeof item_step);
      os.write((char*)&item_span, sizeof item_span);
      os.write((char*)&type, sizeof type);
      os.write((char*)&reserved, sizeof reserved);
      os.write((char*)&item_count, sizeof item_count);
    }
    
    void header::unpack(std::istream& os)
    {
      bbi::record::unpack(os);
      os.read((char*)&item_step, sizeof item_step);
      os.read((char*)&item_span, sizeof item_span);
      os.read((char*)&type, sizeof type);
      os.read((char*)&reserved, sizeof reserved);
      os.read((char*)&item_count, sizeof item_count);
    }
    
    // Bed graph
    //
    bed_graph_record::bed_graph_record(std::istream& is) { unpack(is); }
    bed_graph_record::bed_graph_record() { }
    void bed_graph_record::pack(std::ostream& os) const
    {
      os.write((char*)&chrom_start, sizeof chrom_start);
      os.write((char*)&chrom_end, sizeof chrom_end);
    }
    void bed_graph_record::unpack(std::istream& is)
    {
      is.read((char*)&chrom_start, sizeof chrom_start);
      is.read((char*)&chrom_end, sizeof chrom_end);
    }
    void bed_graph_record::print(std::ostream& os) const
    { os << "(" << chrom_start << ", " << chrom_end << ")"; }
    
    // Var step
    //
    
    var_step_record::var_step_record(std::istream& is) { unpack(is); }
    var_step_record::var_step_record() { }
    void var_step_record::pack(std::ostream& os) const
    {
      os.write((char*)&chrom_start, sizeof chrom_start);
      os.write((char*)&val, sizeof val);
    }
    void var_step_record::unpack(std::istream& is)
    {
      is.read((char*)&chrom_start, sizeof chrom_start);
      is.read((char*)&val, sizeof val);
    }
    void var_step_record::print(std::ostream& os) const
    { os << chrom_start << " - " << val; }
    
    // Fixed step
    //
    fixed_step_record::fixed_step_record(std::istream& is) { unpack(is); }
    fixed_step_record::fixed_step_record() { }
    void fixed_step_record::pack(std::ostream& os) const 
    { os.write((char*)&val, sizeof val); }
    
    // Better interface?
    //
    // void fixed_step::unpack(std::streambuf& isb) { isb.xsgetn((char*)&val, sizeof val); }
    // void fixed_step::unpack(std::istream& is) { unpack(*is.rdbuf()); }
    
    void fixed_step_record::unpack(std::istream& is) 
    { is.read((char*)&val, sizeof val); }
    void fixed_step_record::print(std::ostream& os) const
    {
      os << val;
    }
    
  }
  
}
