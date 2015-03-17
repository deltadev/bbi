#ifndef DPJ_WIG_RECORD_HH_
#define DPJ_WIG_RECORD_HH_

#include "record.hh"

#include <sstream>
#include <vector>

/////////////////////////////////////////////////////////////////////
// wig_record
//
/////////////////////////////////////////////////////////////////////

//  wig_record(uint32_t chrom_id = 0,
//                  uint32_t chrom_start = std::numeric_limits<uint32_t>::max(),
//                  uint32_t chrom_end = 0,
//                  uint32_t item_step = 0,
//                  uint32_t item_span = 0,
//                  uint8_t type = 1,  // 1 bed graph, 2 var step, 3 fixed step
//                  uint8_t reserved = 0,
//                  uint16_t item_count = 0);

namespace bbi {
namespace wig {
enum class record_type { bed_graph = 1, var_step = 2, fixed_step = 3 };

struct header : bbi::record {
    uint32_t item_step;
    uint32_t item_span;
    uint8_t type;
    uint8_t reserved;
    uint16_t item_count;

    header() {}

    header(std::streambuf* s) { unpack(*this, s); }

    record_type record_type() { return static_cast<enum record_type>(type); }

    friend void print(header const& h, std::ostream& os) {
        print(static_cast<bbi::record const&>(h), os);
        os << std::setw(25) << std::left << "item_step" << h.item_step << '\n';
        os << std::setw(25) << std::left << "item_span" << h.item_span << '\n';
        os << std::setw(25) << std::left << "type" << h.type << '\n';
        os << std::setw(25) << std::left << "reserved" << h.reserved << '\n';
        os << std::setw(25) << std::left << "item_count" << h.item_count << '\n';
    }

    // void pack(std::ostream&) const;

    friend void unpack(header& h, std::streambuf* s) {
        unpack(static_cast<bbi::record&>(h), s);
        s->sgetn((char*)&h.item_step, sizeof h.item_step);
        s->sgetn((char*)&h.item_span, sizeof h.item_span);
        s->sgetn((char*)&h.type, sizeof h.type);
        s->sgetn((char*)&h.reserved, sizeof h.reserved);
        s->sgetn((char*)&h.item_count, sizeof h.item_count);
    }
};

struct bed_graph_record {
    uint32_t chrom_start;
    uint32_t chrom_end;

    bed_graph_record(std::streambuf* s) { unpack(*this, s); }
    bed_graph_record() {}

    // void pack(std::ostream&) const;

    friend void println(bed_graph_record const& r, std::ostream& os) {
        os << r.chrom_start << " " << r.chrom_end << '\n';
    }

    friend void unpack(bed_graph_record& r, std::streambuf* s) {
        s->sgetn((char*)&r.chrom_start, sizeof r.chrom_start);
        s->sgetn((char*)&r.chrom_end, sizeof r.chrom_end);
    }
};

struct var_step_record {
    uint32_t chrom_start;
    float val;

    var_step_record(std::streambuf* s) { unpack(*this, s); }
    var_step_record() {}

    // void pack(std::ostream&) const;

    friend void println(var_step_record const& r, std::ostream& os) {
        os << r.chrom_start << ' ' << r.val << '\n';
    }

    friend void unpack(var_step_record& r, std::streambuf* s) {
        s->sgetn((char*)&r.chrom_start, sizeof r.chrom_start);
        s->sgetn((char*)&r.val, sizeof r.val);
    }
};

struct fixed_step_record {
    float val;

    fixed_step_record(std::streambuf* s) { unpack(*this, s); }
    fixed_step_record() {}

    // void pack(std::ostream&) const;

    friend void println(fixed_step_record const& r, std::ostream& os) { os << r.val << '\n'; }

    friend void unpack(fixed_step_record& r, std::streambuf* s) {
        s->sgetn((char*)&r.val, sizeof r.val);
    }
};

// TODO: overload this so that types are annotated using the header info, e.g.
// the span and step, (if used).
//
template <typename T> std::vector<T> extract(std::streambuf& s, unsigned count) {
    std::vector<T> rs(count);
    std::generate(begin(rs), end(rs), [&]() -> T { return &s; });
    return rs;
}
}
template <> struct bbi_type<wig::header> : std::true_type {};
template <> struct bbi_type<wig::bed_graph_record> : std::true_type {};
template <> struct bbi_type<wig::var_step_record> : std::true_type {};
template <> struct bbi_type<wig::fixed_step_record> : std::true_type {};
}

#endif /* DPJ_WIG_RECORD_HH_ */
