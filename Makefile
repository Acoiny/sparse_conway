CXX := clang++
# CC := clang

BINARY := sparse_conway

# prefix with debug, to make running as easy as typing d-TAB
BINARY_DEBUG := debug_$(BINARY)

# flags only for debugging
DEBUG_FLAGS = -g3 -O0 -ggdb

# flags only for release
RELEASE_FLAGS := -O3

CFLAGS = -Wall -Wextra -Werror -std=gnu++23 -finline-functions -march=native -Wno-unused-parameter

HDR = hdr

SRCDIR := src

SOURCE_FILES = $(wildcard src/*.cpp)
SOURCE_FILES += $(wildcard src/*/*.cpp)

OBJECT_FILES = $(patsubst %.cpp,bin/release/%.o,$(shell basename -a $(SOURCE_FILES)))

DOBJECT_FILES = $(patsubst %.cpp,bin/debug/%.o,$(shell basename -a $(SOURCE_FILES)))

ASSEMBLY_FILES = $(DOBJECT_FILES:bin/debug/%.o=bin/assembly/%.s)


LDFLAGS=-lsfml-graphics -lsfml-window -lsfml-system

.PHONY := all debug release clean

all: debug

run: debug
	./$(BINARY_DEBUG)

release: $(BINARY)
release: CFLAGS += $(RELEASE_FLAGS)

debug: $(BINARY_DEBUG)
debug: CFLAGS += $(DEBUG_FLAGS)


$(BINARY): $(OBJECT_FILES)
	$(CXX) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(BINARY_DEBUG): $(DOBJECT_FILES)
	$(CXX) $(CFLAGS) $^ -o $@ $(LDFLAGS)

## Release objects
# rule for all object files directly in the src folder
bin/release/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CFLAGS) -I $(HDR) -c $< -o $@

# rule for all subfolders
bin/release/%.o: $(SRCDIR)/*/%.cpp
	$(CXX) $(CFLAGS) -I $(HDR) -c $< -o $@
## --

## Debug objects
bin/debug/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CFLAGS) -I $(HDR) -c $< -o $@

bin/debug/%.o: $(SRCDIR)/*/%.cpp
	$(CXX) $(CFLAGS) -I $(HDR) -c $< -o $@
## --

# rule to dump assembly
assembly: $(ASSEMBLY_FILES)
assembly: CFLAGS += $(DEBUG_FLAGS)

$(ASSEMBLY_FILES): $(DOBJECT_FILES)
	$(shell mkdir -p bin/assembly)
	objdump -d -S $< > $@

clean:
	rm -f $(OBJECT_FILES) $(DOBJECT_FILES) $(ASSEMBLY_FILES) $(BINARY) $(BINARY_DEBUG)

rebuild:
	$(DEL)
	make all

