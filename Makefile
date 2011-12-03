#

# contrib directory
CONTRIB=./contrib/target

CXX=g++
CXXFLAGS=-std=c++0x -pedantic -Wall -fgnu-tm -Isrc -O2
LDFLAGS=-Wl,-rpath,$(CONTRIB)/lib -Wl,-rpath,$(CONTRIB)/lib32 -Wl,-rpath,$(CONTRIB)/lib64 -Wl,-rpath,/usr/lib/gcc-snapshot/lib -litm -lpthread
TARGET=tester

####

PATH:=$(CONTRIB)/bin:/usr/lib/gcc-snapshot/bin:${PATH}

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

.PHONY: contrib clean distclean run
