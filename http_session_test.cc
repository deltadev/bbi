#include <iostream>
#include <fstream>
#include <sstream>
#include <iterator>
#include <iomanip>
#include <string>

#include "dpj_utils.h"
using namespace dpj;


#include "prettyprint.hpp"

#include "http_session.h"


using std::cerr;
using std::cout;
using std::boolalpha;
using std::vector;
using std::string;
using std::begin;
using std::streambuf;
using std::istream;
using std::left;
using std::setw;
using std::bind;
using std::function;
using namespace std::placeholders;


///
/////////////////////////////////////////////////////////////////////////////
//
// Tests for http_session
//
/////////////////////////////////////////////////////////////////////////////

bool test_get(http::session& conn)
{

  cout << setw(50) << left << "doing test_getline()... NOT IMPLEMENTED... ";

  return true;
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
