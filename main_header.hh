#ifndef DPJ_MAIN_HEADER_HH_
#define DPJ_MAIN_HEADER_HH_

#include <cstdint>
#include <iostream>

struct main_header {
    static const uint16_t byte_size = 64;

    uint32_t magic;
    uint16_t version;
    uint16_t zoom_levels;
    uint64_t bp_tree_offset;
    uint64_t full_data_offset;
    uint64_t full_index_offset;
    uint16_t field_count;
    uint16_t defined_field_count;
    uint64_t auto_sql_offset;
    uint64_t total_summary_offset;
    uint32_t uncompress_buf_size;
    uint64_t extension_offset;

    main_header() {}
    main_header(std::streambuf* s) { unpack(*this, s); }
    // void pack(std::ostream& os) const;

    friend void print(main_header const& h, std::ostream& os) {
        using std::setw;
        using std::left;
        os << setw(25) << left << "magic" << h.magic << '\n';
        os << setw(25) << left << "version" << h.version << '\n';
        os << setw(25) << left << "zoom_levels" << h.zoom_levels << '\n';
        os << setw(25) << left << "bp_tree_offset" << h.bp_tree_offset << '\n';
        os << setw(25) << left << "full_data_offset" << h.full_data_offset << '\n';
        os << setw(25) << left << "full_index_offset" << h.full_index_offset << '\n';
        os << setw(25) << left << "field_count" << h.field_count << '\n';
        os << setw(25) << left << "defined_field_count" << h.defined_field_count << '\n';
        os << setw(25) << left << "auto_sql_offset" << h.auto_sql_offset << '\n';
        os << setw(25) << left << "total_summary_offset" << h.total_summary_offset << '\n';
        os << setw(25) << left << "uncompress_buf_size" << h.uncompress_buf_size << '\n';
        os << setw(25) << left << "extension_offset" << h.extension_offset << '\n';
    }

    friend void unpack(main_header& h, std::streambuf* s) {
        s->sgetn((char*)&h.magic, sizeof h.magic);
        s->sgetn((char*)&h.version, sizeof h.version);
        s->sgetn((char*)&h.zoom_levels, sizeof h.zoom_levels);
        s->sgetn((char*)&h.bp_tree_offset, sizeof h.bp_tree_offset);
        s->sgetn((char*)&h.full_data_offset, sizeof h.full_data_offset);
        s->sgetn((char*)&h.full_index_offset, sizeof h.full_index_offset);
        s->sgetn((char*)&h.field_count, sizeof h.field_count);
        s->sgetn((char*)&h.defined_field_count, sizeof h.defined_field_count);
        s->sgetn((char*)&h.auto_sql_offset, sizeof h.auto_sql_offset);
        s->sgetn((char*)&h.total_summary_offset, sizeof h.total_summary_offset);
        s->sgetn((char*)&h.uncompress_buf_size, sizeof h.uncompress_buf_size);
        s->sgetn((char*)&h.extension_offset, sizeof h.extension_offset);
    }
};

#endif /* DPJ_MAIN_HEADER_HH_ */
