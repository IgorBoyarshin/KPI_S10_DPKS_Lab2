# The name of the main file and executable
mainFileName = main
# Compilation flags
OPTIMIZATION_FLAG = -O0
LANGUAGE_LEVEL = -std=c++17
COMPILER_FLAGS = -Wall -Wextra -Wno-unused-parameter


# Auxiliary
filesObj = $(addsuffix .o, $(mainFileName))


all: cleanExe $(mainFileName)


# Compiler
%.o: %.cpp
	g++ $(COMPILER_FLAGS) $(OPTIMIZATION_FLAG) $(LANGUAGE_LEVEL) -c $<

# Linker
$(mainFileName): $(filesObj)
	g++ $(COMPILER_FLAGS) $(OPTIMIZATION_FLAG) $(LANGUAGE_LEVEL) $^ -o $@


# Utils
run:
	./main --version -l 123 --repeat --args 19,14,8 -rh 1

clean:
	rm -f a.out *.o *.gch .*.gch $(mainFileName)

cleanExe:
	rm -f $(mainFileName)
