TARGET = kernel

# Add additional PC files
PKG_CONFIG_PATH = $(shell pkg-config --variable pc_path pkg-config)
PKG_CONFIG_LIBS = yaml-cpp uuid libzmq libzen libmediainfo libexif openssl opencv4 lapack libjpeg dlib miso

CXX = g++
LIB = $(shell PKG_CONFIG_PATH=$(PKG_CONFIG_PATH):./pkgconfig pkg-config --libs $(PKG_CONFIG_LIBS))
INC = $(shell PKG_CONFIG_PATH=$(PKG_CONFIG_PATH):./pkgconfig pkg-config --cflags $(PKG_CONFIG_LIBS))

CXXFLAGS = --std=c++2a -Wall -O3 -DDLIB_JPEG_SUPPORT $(INC)

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
	$(CXX) $(OBJECTS) -Wall $(LIB) -o $@

clean:
	-rm -f *.o
	-rm -f $(TARGET)
