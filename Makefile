CXX=clang++
CXXFLAGS=-Wall -std=c++14 -Iinclude
LDLIBS=-lGL -lEGL -lwayland-client -lwayland-egl

.SUFFIXES:

EXEC=sphere
SOURCES = $(wildcard src/*.cpp)
OBJECTS = $(SOURCES:src/%.cpp=obj/%.o)

$(EXEC): $(OBJECTS)
	$(CXX) $(LDLIBS) -o $@ $^

obj/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

run: $(EXEC)
	./$(EXEC)

clean:
	$(RM) $(OBJECTS) $(EXEC)
