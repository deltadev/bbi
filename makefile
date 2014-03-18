# learns make a new pattern.
%.o : %.cc %.h
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) $< -o $@


# good for OSX, BSD, maybe linux.
CXXFLAGS=-stdlib=libc++ -std=c++11

# for asio on this system
CPPFLAGS=-I$(HOME)/include

LDFLAGS=-lz -L$(HOME)/lib -ldpj

OBJS=main_header.o records.o bbi_file.o bp_tree.o chromosome_tree.o \
        r_tree.o total_summary_header.o zoom_header.o

NET_OBJS=http_streambuf.o http_session.o


all: remote_bbi bbi_test run_http_session_test run_http_streambuf_test


remote_bbi.o: $(NET_OBJS) $(OBJS)
remote_bbi: remote_bbi.o $(NET_OBJS) $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

# tests

bbi_test.o: $(OBJS)

bbi_test: bbi_test.o $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)


http_session_test.o: http_session.o
run_http_session_test: http_session_test.o http_session.o


http_streambuf_test.o: $(NET_OBJS)
run_http_streambuf_test: http_streambuf_test.o $(NET_OBJS)


clean:
	rm -rf *.o remote_bbi bbi_test run_http_session_test run_http_streambuf_test
