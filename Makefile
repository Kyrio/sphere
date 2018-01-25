CXX=clang++-5.0
CXXFLAGS=-Wall -std=c++14
LDLIBS=-lGL -lEGL -lwayland-client -lwayland-egl

.SUFFIXES:

EXEC=sphere
SOURCES = $(wildcard *.cpp)
OBJECTS = $(SOURCES:.cpp=.o)

$(EXEC): $(OBJECTS)
	$(CXX) $(LDLIBS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

run: $(EXEC)
	./$(EXEC)

clean:
	rm -f $(OBJECTS) $(EXEC)
