CC=g++
CFLAGS=-c -Wall -Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef -Wfloat-equal -Winline -Wunreachable-code -Wmissing-declarations -Wmissing-include-dirs -Wswitch-enum -Wswitch-default -Weffc++ -Wmain -Wextra -Wall -g -pipe -fexceptions -Wcast-qual -Wconversion -Wctor-dtor-privacy -Wempty-body -Wformat-security -Wformat=2 -Wignored-qualifiers -Wlogical-op -Wno-missing-field-initializers -Wnon-virtual-dtor -Woverloaded-virtual -Wpointer-arith -Wsign-promo -Wstack-usage=8192 -Wstrict-aliasing -Wstrict-null-sentinel -Wtype-limits -Wwrite-strings -Werror=vla -D_EJUDGE_CLIENT_SIDE -DDEBUG
LDFLAGS=
SOURCES=main.cpp trees.cpp tree_dump.cpp debug/debug.cpp TextParse/text_parse.cpp debug/color_print.cpp Stack/stack.cpp diff.cpp parse.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=Diff

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf *.o Diff
	rm -rf *.exe Diff
	rm -rf *.html Diff
	rm -rf *.bin Diff
	rm -rf *.svg Diff
	rm -rf *.dot Diff
	rm -rf *.png Diff
