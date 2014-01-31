CXXFLAGS = -stdlib=libc++ -std=c++11

%.o : %.cc %.h
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) $< -o $@

test.o: main_header.o data_records.o

test: test.o main_header.o data_records.o
	$(CXX) $(CXXFLAGS) $^ -o $@
