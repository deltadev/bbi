CXXFLAGS = -stdlib=libc++ -std=c++11
LDFLAGS=-lz
OBJS=main_header.o data_records.o bbi_file.o bp_tree.o chromosome_tree.o \
        r_tree.o total_summary_header.o zoom_header.o

all: test

%.o : %.cc %.h
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) $< -o $@

test.o: $(OBJS)

test_bbi: test_bbi.o $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)


