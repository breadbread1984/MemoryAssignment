CXXFLAGS=-I. -O2 -DNDEBUG
LIBS=
OBJS=$(patsubst %.cpp,%.o,$(wildcard *.cpp))

all: main

main: main.o memory.o
	$(CXX) $^ $(LIBS) -o ${@}

clean:
	$(RM) $(OBJS) *.o
