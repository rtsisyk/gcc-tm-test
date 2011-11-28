#
# gcc >= 4.7.0 path  (apt-get install gcc-snapshot)
SYSROOT=/usr/lib/gcc-snapshot

# contrib directory
CONTRIB=./contrib

CXX=g++
CXXFLAGS=-std=c++0x -O2 -pedantic -Wall -fgnu-tm -Isrc -g
LDFLAGS=-Wl,-rpath,$(CONTRIB)/lib -Wl,-rpath,$(SYSROOT)/lib -litm -lpthread
TARGET=tester

####

PATH:=$(SYSROOT)/bin:${PATH}

all: $(TARGET)

$(TARGET): main.o
	$(CXX) $(LDFLAGS) main.o -o $@

main.o: src/main.cpp src/Tests/*.h src/Utils/*.h
	$(CXX) $(CXXFLAGS) -c src/main.cpp -o main.o

contrib:
	make -C $(CONTRIB)

clean:
	rm -rf *.o $(TARGET)

distclean: clean
	make -C $(CONTRIB) distclean

run: $(TARGET)
	./$(TARGET) < ./tests.cfg

.PHONY: contrib clean disclean run
