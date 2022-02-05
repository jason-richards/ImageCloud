TARGET = kernel
LIBS = $(shell pkg-config --libs yaml-cpp uuid libzmq libzen libmediainfo libexif openssl opencv4)

CXX = g++
CXXFLAGS = --std=c++2a -Wall -O3 $(shell pkg-config --cflags opencv4)

.PHONY: default debug all clean

default: $(TARGET)
all: default

debug: CXXFLAGS += -DDEBUG -g
debug: all

OBJECTS = $(patsubst %.cpp, %.o, $(wildcard *.cpp))
HEADERS = $(wildcard *.hpp)

%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PRECIOUS: $(TARGET) $(OBJECTS)

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -Wall $(LIBS) -o $@

clean:
	-rm -f *.o
	-rm -f $(TARGET)
