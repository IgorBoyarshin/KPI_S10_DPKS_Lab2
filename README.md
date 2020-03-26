This project contains a command-line arguments parser.

It supports the following arguments:

--help (-h) with a single number to get help on the specified function.

--version (-v) to print the version of the program.

--args (-a) with multiple numbers separated by commas to specify arguments for the subtask.

--repeat (-r) with a single number to repeat the execution specified number of times.


If any of the arguments are repeated -- they are ignored altogether.

If an unknown argument is passed -- a corresponding error message is printed.

Otherwise the passed arguments are printed to the console with their respective passed numbers (if any).

In case multiple arguments number are supplied to a command that takes only a single argument, all numbers starting from the second one are ignored.

In case multiple short arguments are joined together (as in -abc), then the following numbers (if any) are understood to be passed to the last argument.
