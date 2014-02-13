#include <iostream>
#include <fstream>
#include <sstream>
#include <iterator>
#include <iomanip>

#include <string>

#include "http_streambuf.h"
#include "prettyprint.hpp"
#include "http_session.h"

#include "dpj_utils.h"

using namespace dpj;


using std::vector;
using std::string;
using std::begin;


using std::streambuf;
using std::istream;
using std::left;
using std::setw;
using std::cerr;
using std::cout;
using std::boolalpha;


using std::bind;
using std::function;
using namespace std::placeholders;


/////////////////////////////////////////////////////////////////////////////
//
// Tests for the http_streambuf.
//
//   - These tests are `tests by comparison' to std::ifstream and
//     std::filebuf whose interfaces we are trying to emulate.
//
/////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////
// Tests via the std::istream interface
//////////////////////////////////////////
bool test_read_across_buffer(std::istream& is1, std::istream& is2, size_t buf_sz, bool verbose)
{
  cout << setw(50) << left << "doing test_read_across_buffer()... ";

  buf_sz = std::max<size_t>(2, buf_sz);
  std::vector<char> data1(4 * buf_sz);
  is1.seekg(2 * buf_sz - 1);
  
  is1.read(data1.data(), data1.size());
  
  std::vector<char> data2(4 * buf_sz);
  is2.seekg(2 * buf_sz - 1);
  is2.read(data2.data(), data2.size());
  
  return data1 == data2;
}

bool test_rdstate(std::istream& is1, std::istream& is2, bool verbose)
{
  cout << setw(50) << left << "doing test_rdstate()... ";
  auto rd1 = is1.rdstate();
  auto rd2 = is2.rdstate();
  bool result = rd1 == rd2;

  // Re-set here so other tests aren't effected.
  is1.setstate(std::ios_base::goodbit);
  is2.setstate(std::ios_base::goodbit);
  return result;
}
bool test_get(std::istream& is1, std::istream& is2, bool verbose)
{
  
  cout << setw(50) << left << "doing test_getline()... ";
  
  char c1 = is1.get();
  char c2 = is2.get();
  
  if (verbose) {
    cout << "got char1:\n" << c1 << ' ';
    cout << "got char2:\n" << c2 << ' ';
  }
  
  return c1 == c2;
}

bool test_getline(std::istream& is1, std::istream& is2, bool verbose)
{
  
  cout << setw(50) << left << "doing test_getline()... ";
  
  
  std::string line1, line2;
  
  std::getline(is1, line1);
  std::getline(is2, line2);
  
  if (verbose) {
    cout << "got line1:\n" << line1 << ' ';
    cout << "got line2:\n" << line2 << ' ';
  }
  
  return line1 == line2;
}

bool test_read(istream& is1, istream& is2, std::streamsize sz, bool verbose)
{
  
  cout << setw(50) << left << "doing test_read()... ";
  
  vector<char> line1(sz), line2(sz);
  
  is1.read(line1.data(), sz);
  auto count1 = is1.gcount();
  is2.read(line2.data(), sz);


  if (verbose) {
    cout << "read line1:\n" << string(begin(line1), end(line1)) << ' ';
    cout << "read line2:\n" << string(begin(line2), end(line2)) << ' ';
  }
  return line1 == line2 && count1 == sz;
}

bool test_seekg_tellg(istream& is1, istream& is2, std::streampos sz, bool verbose)
{
  
  cout << setw(50) << left << "doing test_seekg_tellg()... ";
  
  std::streampos pos1 = is1.seekg(sz).tellg();
  std::streampos pos2 = is2.seekg(sz).tellg();
  
  if (verbose)
    cout << "pos1: " << pos1 << ", pos2: " << pos2 << ' ';
  
  return pos1 == pos2;
}
bool test_gcount(std::istream& is1, std::istream& is2, bool verbose)
{
  cout << setw(50) << left << "doing test_gcount()... ";
  size_t count1 = is1.gcount();
  size_t count2 = is2.gcount();
  if (verbose)
    cout << "count1: " << count1 << ", count2: " << count2 << ' ';
  
  return count1 == count2;
}


//////////////////////////////////////////////////
// Tests via the std::streambuf public interface
//////////////////////////////////////////////////

bool test_pubimbue(std::streambuf* sb1, std::streambuf* sb2, bool verbose)
{
  cout << setw(50) << left << "doing test_pubimbue()... NOT IMPLEMENTED...";
  return true;
}

bool test_getloc(std::streambuf* sb1, std::streambuf* sb2, bool verbose)
{
  cout << setw(50) << left << "doing test_getloc()... NOT IMPLEMENTED...";
  return true;
}

bool test_pubsetbuf(std::streambuf* sb1, std::streambuf* sb2, bool verbose)
{
  cout << setw(50) << left << "doing test_pubsetbuf()... NOT IMPLEMENTED... ";
  // TODO: possible impl would be to setbuf to common buf for both streambufs.
  //
  return true;
}

bool test_pubseekoff(streambuf* sb1, streambuf* sb2, std::streamoff off,
                     std::ios_base::seekdir way, bool verbose)
{
  cout << setw(50) << left << "doing test_pubseekoff()... ";
  auto pos1 = sb1->pubseekoff(off, way);
  auto pos2 = sb2->pubseekoff(off, way);
  if (verbose)
    cout << "pos1: " << pos1 << ", pos2: " << pos2 << ' ';
  return pos1 == pos2;
}

bool test_pubseekpos(streambuf* sb1, streambuf* sb2, std::streampos pos, bool verbose)
{
  cout << setw(50) << left << "doing test_pubseekpos()... ";
  return sb1->pubseekpos(pos) == sb2->pubseekpos(pos);
}

bool test_pubsync(std::streambuf* sb1, std::streambuf* sb2, bool verbose)
{
  cout << setw(50) << left << "doing test_pubsync()... NOT IMPLEMENTED...";
  return true;
}

bool test_in_avail(std::streambuf* sb1, std::streambuf* sb2, bool verbose)
{
  cout << setw(50) << left << "doing test_in_avail()... NOT IMPLEMENTED...";
  return true;
}

bool test_snextc(std::streambuf* sb1, std::streambuf* sb2, bool verbose)
{
  cout << setw(50) << left << "doing test_snextc()... ";
  char c1 = sb1->snextc();
  char c2 = sb2->snextc();
  if (verbose)
    cout << "c1: " << c1 << ", c2: " << c2 << ' ';
  return c1 == c2;
}

bool test_sbumpc(std::streambuf* sb1, std::streambuf* sb2, bool verbose)
{
  cout << setw(50) << left << "doing test_sbumpc()... ";
  return sb1->sbumpc() == sb2->sbumpc();
}

bool test_sgetc(std::streambuf* sb1, std::streambuf* sb2, bool verbose)
{
  cout << setw(50) << left << "doing test_sgetc()... ";
  return sb1->sgetc() == sb2->sgetc();
}

bool test_sgetn(std::streambuf* sb1, std::streambuf* sb2, size_t n, bool verbose)
{
  cout << setw(50) << left << "doing test_sgetn()... ";
  std::vector<char> buf1(n), buf2(n);
  sb1->sgetn(buf1.data(), n);
  sb2->sgetn(buf2.data(), n);
  
  if (verbose)
    cout << "buf1: " << buf1 << ", buf2: " << buf2 << ' ';
  
  return buf1 == buf2;
}

bool test_sputc(std::streambuf* sb1, std::streambuf* sb2, char c, bool verbose)
{
  cout << setw(50) << left << "doing test_sputc()... NOT IMPLEMENTED...";
  return true;
}

bool test_sputn(std::streambuf* sb1, std::streambuf* sb2, size_t n, bool verbose)
{
  cout << setw(50) << left << "doing test_sputn()... NOT IMPLEMENTED...";
  return true;
}

bool test_sputbackc(std::streambuf* sb1, std::streambuf* sb2, char c, bool verbose)
{
  cout << setw(50) << left << "doing test_sputbackc()... NOT IMPLEMENTED...";
  return true;
}

bool test_sungetc(std::streambuf* sb1, std::streambuf* sb2, bool verbose)
{
  cout << setw(50) << left << "doing test_sungetc()... NOT IMPLEMENTED...";
  return true;
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

bool test_streambuf(streambuf* sb1, streambuf* sb2, bool verbose)
{

  
  bool all_pass = true;
  bool result;
  
  ////////////
  // Exercises the std::streambuf interface
  //
  //
  vector<function<bool(streambuf*,streambuf*, bool)>> sb_tfs
  {
    
    test_pubimbue,
    test_getloc,
    test_pubsetbuf,
    test_pubsync,
    test_in_avail,
    
    test_snextc,
    bind(test_pubseekoff, _1, _2, 0, std::ios_base::cur, _3),
    test_snextc,
    bind(test_pubseekoff, _1, _2, 0, std::ios_base::cur, _3),
    
    test_sbumpc,
    bind(test_pubseekoff, _1, _2, 0, std::ios_base::cur, _3),
    
    
    test_sgetc,
    bind(test_pubseekoff, _1, _2, 0, std::ios_base::cur, _3),
    
    
    test_sungetc,
    
    // These aren't really supporte since it's istream only at present.
    //
    bind(test_sputc, _1, _2, 'x', _3),
    bind(test_sputn, _1, _2, 10, _3),
    bind(test_sputbackc, _1, _2, 'x', _3)
    
  };
  
  // Adds tests for each of the buff_sizes/offsets in bszs.
  //
  
  size_t offs[]{0, 1, 32, 64, 128, 512, 1024, 4096, 4 * 4096};
  for (auto sz : offs)
  {
    sb_tfs.insert(sb_tfs.end(), {
      
      bind(test_pubseekpos, _1, _2, sz, _3),
      
      bind(test_sgetn, _1, _2, sz, _3),
      bind(test_pubseekoff, _1, _2, 0, std::ios_base::cur, _3),
      
      bind(test_pubseekoff, _1, _2, sz, std::ios_base::cur, _3),
      bind(test_pubseekoff, _1, _2, 0, std::ios_base::cur, _3),
      
      bind(test_pubseekoff, _1, _2, sz, std::ios_base::beg, _3),
      bind(test_pubseekoff, _1, _2, 0, std::ios_base::cur, _3),
      
      
    });
  }
  
  for (auto& tf : sb_tfs)
  {
    result = tf(sb1, sb2, verbose);
    cout << "TEST_PASS: "<< boolalpha << result << '\n';
    all_pass = all_pass && result;
  }
  
  return all_pass;
  
}


// TODO: Could do with making these tests more systematic.
//
bool test_istream(istream& is1, istream& is2, bool verbose)
{
  bool all_pass = true;
  bool result;
  
  ////////////
  // Exercises the istream interface.
  //
  // If all the above tests are equiv, between std::ifstream and std::istream(&http_streambuf)
  // then these test should be superfluous. They might be useful later for profiling if nothing
  // else...
  //
  vector<function<bool(std::istream&,std::istream&, bool)>> is_tfs
  {
    
    test_getline,
    test_gcount,
    test_get,
    test_gcount,
    
  };
  
  // Adds tests for each of the offsets in szs.
  //
  
  size_t offs[]{0, 1, 32, 64, 128, 512, 1024, 4096, 4 * 4096};
  for (auto sz : offs)
  {
    is_tfs.insert(is_tfs.end(), {
      
      bind(test_read, _1, _2, sz, _3),
      test_rdstate,
      test_gcount,
      
      bind(test_seekg_tellg, _1, _2, sz, _3),
      test_rdstate,
      test_get,
      test_gcount,
      test_rdstate,
      
      bind(test_read_across_buffer, _1, _2, sz, _3),
      test_gcount,
      test_rdstate,
    });
  }
  
  for (auto& tf : is_tfs) {
    result = tf(is1, is2, verbose);
    cout << "TEST_PASS: "<< boolalpha << result << '\n';
    all_pass = all_pass && result;
  }
  
  
  return all_pass;
};
































