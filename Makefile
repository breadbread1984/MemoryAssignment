CXXFLAGS=-I. -g2 `pkg-config --cflags opencv4`
LIBS=`pkg-config --libs opencv4` -lboost_random
OBJS=$(patsubst %.cpp,%.o,$(wildcard *.cpp))

all: main

main: main.o allocator.o
	$(CXX) $^ $(LIBS) -o ${@}

clean:
	$(RM) $(OBJS) *.o
