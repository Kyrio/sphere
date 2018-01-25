CXX=clang++-5.0
CXXFLAGS=-Wall -std=c++14
LDFLAGS=-Linclude
LDLIBS=-lGL -lEGL -lwayland-client -lwayland-egl

.SUFFIXES:

EXEC=bin/sphere
SOURCES = $(wildcard *.cpp)
OBJECTS = $(addprefix obj/, $(SOURCES:.cpp=.o))

$(EXEC): $(OBJECTS)
	$(CXX) $(LDFLAGS) $(LDLIBS) -o $@ $^

obj/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

run: $(EXEC)
	./$(EXEC)

clean:
	$(RM) $(OBJECTS) $(EXEC)
