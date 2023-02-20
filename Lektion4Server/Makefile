Executable = server
targets = all clean help
CXX = gcc

# wildcard
SOURCES = server.c iknlib.c
OBJECTS = $(SOURCES:.c=.o)
DEPS = $(SOURCES:.c=.d)
CXXFLAGS = -I.


# how to create .o files from cpp
%.d:%.c
	$(CXX) -MT$@ -MM $(CXXFLAGS) $< > $@
	$(CXX) -MT$(@:.d=.o) -MM  $(CXXFLAGS) $< >> $@

#  target dependant on executable 
all: $(Executable)

# executable dependant on OBJECTS and DEPS
# compile an exectuable (main) with objects (main.o part1.o part2.o)
$(Executable): $(OBJECTS) $(DEPS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJECTS)

-include $(DEPS)

# remove executable
clean:
	rm $(Executable) *.o *.d

help:
	@echo "Available targets: $(targets)"
	@echo "Makefile compiles: $(SOURCE)"
	@echo "Executable file: $(Executable)"