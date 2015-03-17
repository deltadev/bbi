#ifndef DPJ_TOTAL_SUMMARY_HEADER_HH_
#define DPJ_TOTAL_SUMMARY_HEADER_HH_

#include <cstdint>
#include <ostream>

struct total_summary_header {
    uint64_t bases_covered;
    double min_val;
    double max_val;
    double sum_data;
    double sum_squares;

    total_summary_header() {}
    total_summary_header(std::streambuf* s) { unpack(*this, s); }

    // void pack(std::ostream& os) const;
    friend void print(total_summary_header const& h, std::ostream& os) {
        os << std::setw(25) << std::left << "bases_covered" << h.bases_covered << '\n';
        os << std::setw(25) << std::left << "min_val" << h.min_val << '\n';
        os << std::setw(25) << std::left << "max_val" << h.max_val << '\n';
        os << std::setw(25) << std::left << "sum_data" << h.sum_data << '\n';
        os << std::setw(25) << std::left << "sum_squares" << h.sum_squares << '\n';
    }

    friend void unpack(total_summary_header& r, std::streambuf* s) {
        s->sgetn((char*)&r.bases_covered, sizeof r.bases_covered);
        s->sgetn((char*)&r.min_val, sizeof r.min_val);
        s->sgetn((char*)&r.max_val, sizeof r.max_val);
        s->sgetn((char*)&r.sum_data, sizeof r.sum_data);
        s->sgetn((char*)&r.sum_squares, sizeof r.sum_squares);
    }
};

#endif /* DPJ_TOTAL_SUMMARY_HEADER_HH_ */
