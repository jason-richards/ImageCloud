MAJOR_VERSION = 1
MINOR_VERSION = 0
NAME    := libmiso
SNAME   := $(NAME).a
DNAME   := $(NAME).so
SRC     := $(wildcard *.cpp)
OBJ     := $(SRC:.cpp=.o)
CXX = g++
CXXFLAGS = --std=c++2a -Wall -O3 -pedantic -Wall -W -Werror -fPIC
LDFLAGS := #-L.
LDLIBS  := #-l$(...)

.PHONY: all clean fclean re debug

debug: CXXFLAGS += -DDEBUG -g
debug: all

all: $(SNAME) $(DNAME)

$(SNAME): $(OBJ)
	$(AR) $(ARFLAGS) $@ $^

$(DNAME): LDFLAGS += -shared
$(DNAME): $(OBJ)
	$(CXX) $(LDFLAGS) $^ $(LDLIBS) -o $@

clean:
	$(RM) $(OBJ)

fclean: clean
	$(RM) $(SNAME) $(DNAME)

re: fclean all
