#ifndef DPJ_BED_RECORD_HH_
#define DPJ_BED_RECORD_HH_

#include <sstream>
#include <string>
#include <array>

#include "record.hh"
/////////////////////////////////////////////////////////////////////
// bed_record
//
/////////////////////////////////////////////////////////////////////
namespace bbi {
namespace bed {
struct record : bbi::record {
    std::string rest;

    record() {}
    record(std::streambuf* s) { unpack(*this, s); }

    // void pack(std::ostream& os) const;

    friend void print(record const& r, std::ostream& os) {
        print(static_cast<bbi::record const&>(r), os);
        os << std::setw(25) << std::left << "rest" << r.rest << '\n';
    }

    friend void unpack(record& r, std::streambuf* s) {
        unpack(static_cast<bbi::record&>(r), s);

        // FIXME: What is a reasonable upper bound for the size of the string rest?
        //        Have chosen 4 * 1024 here.
        //
        std::array<char, 4 * 1024> buff;
        std::istream is{s};
        is.getline(buff.data(), 4 * 1024, '\0');
        std::streamsize count = is.gcount();
        if (count > 0)
            r.rest.assign(buff.data(), count - 1);
    }
};
}

template <> struct bbi_type<bed::record> : std::true_type {};
}

#endif /* DPJ_BED_RECORD_HH_ */
