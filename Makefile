#

# contrib directory
CONTRIB=./contrib/target

CXX=g++
CXXFLAGS=-std=c++0x -pedantic -Wall -Isrc -O2 -march=native
LDFLAGS=-Wl,-rpath,$(CONTRIB)/lib -Wl,-rpath,$(CONTRIB)/lib32 -Wl,-rpath,$(CONTRIB)/lib64 -Wl,-rpath,/usr/lib/gcc-snapshot/lib -lpthread
TARGET=tester

####

PATH:=$(CONTRIB)/bin:/usr/lib/gcc-snapshot/bin:${PATH}

all: $(TARGET)-onethread $(TARGET)-tm-gnu $(TARGET)-tm-tiny $(TARGET)-mutex

$(TARGET)-onethread: src/main.cpp src/Tests/*.h src/Utils/*.h
	$(CXX) $(CXXFLAGS) -DLOCKTYPE_NONE $(LDFLAGS) src/main.cpp -o $@

$(TARGET)-tm-gnu: src/main.cpp src/Tests/*.h src/Utils/*.h
	$(CXX) $(CXXFLAGS) -fgnu-tm -DLOCKTYPE_TM $(LDFLAGS) -litm src/main.cpp -o $@

$(TARGET)-tm-tiny: src/main.cpp src/Tests/*.h src/Utils/*.h
	$(CXX) $(CXXFLAGS) -fgnu-tm -DLOCKTYPE_TM $(LDFLAGS) -litmtiny src/main.cpp -o $@

$(TARGET)-mutex: src/main.cpp src/Tests/*.h src/Utils/*.h
	$(CXX) $(CXXFLAGS) -DLOCKTYPE_MUTEX $(LDFLAGS) src/main.cpp -o $@

contrib:
	make -C $(CONTRIB)

clean:
	rm -rf *.o $(TARGET)-onethread $(TARGET)-tm-gnu $(TARGET)-tm-tiny $(TARGET)-mutex

distclean: clean
	make -C $(CONTRIB) distclean

run: $(TARGET)-tm-tiny
	./$(TARGET)-tm-tiny < ./tests.cfg

.PHONY: contrib clean distclean run
