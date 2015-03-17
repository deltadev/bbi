#ifndef DPJ_BBI_STREAM_HH_
#define DPJ_BBI_STREAM_HH_

#include <type_traits>

#include <vector>
#include <fstream>

#include <zlib.h>

#include "dpj/util/block_decompressor.hh"
#include "dpj/http/http_streambuf.hh"

#include "main_header.hh"
#include "total_summary_header.hh"
#include "zoom_header.hh"

#include "r_tree.hh"
#include "bbi_index.hh"
#include "contig_index.hh"

namespace bbi {

class stream : public std::streambuf {
  public:
    enum type { wig = 0x888FFC26, bed = 0x8789F2EB } type;

    stream() {}
    stream(std::string resource) {
        if (resource.substr(0, 7) == "http://") {
            resource = resource.substr(7); // inefficient
            auto pos = resource.find_first_of('/');
            auto host = resource.substr(0, pos);
            resource = resource.substr(pos);
            input_stream_.reset(
                new dpj::http::streambuf(host, "http", resource, false /* debug */));
        } else {
            std::unique_ptr<std::filebuf> fb{new std::filebuf{}};
            fb->open(resource, std::ios_base::in);
            if (!fb->is_open())
                throw std::runtime_error("remote_bbi: could not open file: " + resource);
            input_stream_ = std::move(fb);
        }

        // Unpacks main header and determines type.
        //
        unpack(main_header, input_stream_.get());
        bool is_bed = main_header.magic == type::bed; // TODO: what integer conversion happens here?
        type = is_bed ? type::bed : type::wig;

        // Unpacks total summary header
        //
        input_stream()->pubseekpos(main_header.total_summary_offset);
        unpack(total_summary_header, input_stream_.get());

        // Unpacks the number of records.
        //
        input_stream()->pubseekpos(main_header.full_data_offset);
        input_stream()->sgetn((char*)&num_records, 8);

        // Unpacks the zoom headers.
        //
        input_stream()->pubseekpos(main_header::byte_size);
        for (int i = 0; i < main_header.zoom_levels; ++i) {
            zoom_header z_h{0};
            unpack(z_h, input_stream_.get());
            zoom_headers.push_back(z_h);
        }
    }

    std::streambuf* input_stream() { return input_stream_.get(); }

    friend stream& seek(stream& s, r_tree::leaf_node ln) {
        s.input_buf.resize(ln.data_size);

        s.input_stream()->pubseekpos(ln.data_offset);
        std::streamsize n = s.input_stream()->sgetn((char*)s.input_buf.data(), s.input_buf.size());

        if (n != ln.data_size)
            throw std::runtime_error("file::inflate_records failed to read comp_sz bytes");

        if (s.main_header.uncompress_buf_size == 0) {
            s.setg((char*)s.input_buf.data(), (char*)s.input_buf.data(),
                   (char*)s.input_buf.data() + s.input_buf.size());
        } else {
            auto p = s.decompressor.decompress(s.input_buf.data(),
                                               s.input_buf.data() + s.input_buf.size());
            s.setg((char*)p.first, (char*)p.first, (char*)p.second);
        }

        return s;
    }

    friend index index(stream& s, int level) {
        if (level > s.zoom_headers.size()) {
            level = static_cast<int>(s.zoom_headers.size());
            std::cerr << "Warning: zoom level requested was greater than available.\n";
        }

        if (level == 0)
            s.input_stream()->pubseekpos(s.main_header.full_index_offset);
        else
            s.input_stream()->pubseekpos(s.zoom_headers[level - 1].index_offset);

        return s.input_stream();
    }

    friend contig_index ctig_index(stream& s) {
        s.input_stream()->pubseekpos(s.main_header.bp_tree_offset);
        return s.input_stream();
    }

    friend void print_header(stream& s, std::ostream& os) {
        os << "\n**** main_header ****\n\n";
        print(s.main_header, os);
    }

    friend void print_summary(stream& s, std::ostream& os) {
        os << "\n**** total_summary_header ****\n\n";
        print(s.total_summary_header, os);
    }

    friend void print_num_records(stream& s, std::ostream& os) {
        os << "num records: " << s.num_records << '\n';
    }

    std::vector<zoom_header> zoom_headers;

  private:
    std::unique_ptr<std::streambuf> input_stream_;

    main_header main_header;
    total_summary_header total_summary_header;
    std::size_t num_records;

    std::vector<uint8_t> input_buf;
    block_decompressor decompressor;
};
}

#endif /* DPJ_BBI_STREAM_HH_ */
