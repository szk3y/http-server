SRCS = main.cc network.cc util.cc http.cc
OBJS = $(SRCS:.cc=.o)
DEPS = $(SRCS:.cc=.d)
TARGET = http-server

CXX = clang++
CXXFLAGS = -g -O0 -Wall -Wextra -std=c++14
LIBS =

.PHONY: all
all: $(OBJS)
	ctags -R
	$(CXX) -o $(TARGET) $(CXXFLAGS) $(OBJS) $(LIBS)

%.o: %.cc
	$(CXX) -MMD -MP -c -o $@ $(CXXFLAGS) $<

.PHONY: clean
clean:
	$(RM) $(TARGET) $(OBJS) $(DEPS) tags

-include *.d
