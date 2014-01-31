
#include <zlib.h>
#include <sstream>

template <typename T>
std::vector<T>
bbi_file::records_for_leaf(r_tree::leaf_node ln) {

  uint32_t uncomp_buf_sz = main_hdr.uncompress_buf_size;
  std::vector<T> bdrs;
    
  is_.seekg(ln.data_offset);
  if (uncomp_buf_sz == 0) {
      
    T bdr;
    while (is_.tellg() < ln.data_offset + ln.data_size) {
      bdr.unpack(is_);
      bdrs.push_back(bdr);
    }
      
  } else {
    bdrs = inflate_records<T>(is_, ln.data_size, uncomp_buf_sz);
  }
    
  return bdrs;
}

template <typename T> std::vector<T>
bbi_file::inflate_records(std::istream& is, uint64_t comp_sz, size_t decomp_sz) {
    
  int ret;
  unsigned have;
  z_stream strm;
    
  /* allocate inflate state */
  strm.zalloc = Z_NULL;
  strm.zfree = Z_NULL;
  strm.opaque = Z_NULL;
  strm.avail_in = 0;
  strm.next_in = Z_NULL;
  ret = inflateInit(&strm);
  if (ret != Z_OK) {
    std::cerr << "couldn't init inflate context.\n";
    exit(1);
  }
    
  // Buffers.
  //
  std::vector<unsigned char> out(decomp_sz);
  std::vector<unsigned char> in(comp_sz);
  is.read((char*)in.data(), comp_sz);
    
  strm.avail_in = (unsigned)in.size();
  strm.next_in = in.data();
    
  do {
    strm.avail_out = (unsigned)out.size();
    strm.next_out = out.data();
    ret = inflate(&strm, Z_NO_FLUSH);
    have = (unsigned)out.size() - strm.avail_out;
    if (strm.avail_in == 0)
      break;
      
  } while (strm.avail_out == 0);
    
  if (ret != Z_STREAM_END) {
    std::cerr << "failed to inflate correctly.\n";
    exit(1);
  }
  inflateEnd(&strm);
    
  std::string output(out.begin(), out.end());
  std::istringstream iss;
  iss.str(output);
    
  std::vector<T> bdrs;
  while (iss) {
    T bdr;
    bdr.unpack(iss);
    if (bdr.chrom_start != bdr.chrom_end)
      bdrs.push_back(bdr);
  }
    
  return bdrs;
}
