# Process Execution

## How to use the program

#### Compiling and Execution:

```Bash
        make
        ./my_cmd -e <ExecutablePath> -o <Options> [-b | -f]
``` 
Here, the -b flag is used to indicate that the child process will run in the background, and the -f flag indicates that it will run in the foreground. The default behavior is foreground execution. 

## Design Decisions

In this program, we have chosen to make the default behavior as the process running in the foreground, but this can be changed optionally. This behavior was chosen to ensure that the process executes either in foreground or background but not both.

If the number of arguments passed to the process is insufficient, it will terminate with error code -1. This was also a design decision to allow the process to indicate that the issue was with the arguments and not any other specific error. Similarly, any other error related to arguments will return -1

Another design decision we made was to report only one error of write system call in standard output when printing which error code our C program is reporting. If the write system call fails another time, the value of errno is returned and no error code is written to standard output. 

In the program, we have chosen to print exit code of child only if the value is not 0, otherwise no error code is printed. This is to remove redundancy.
Similarly, no error code is printed for the C program if no error has arisen during the process execution. If any error occurs, it will be detected and printed accordingly.

In the program if no arguments are given, the help message will be displayed with information of how to use the program. If options are given but no executable path is specified, the program will print message specifying that executable path must be specified. This decision was made to meet assignment requirements while retaining the default help functionality.

Finally, the program only prints the exit code of the child. It does not check whether the core was dumped in the child or whether it incountered any signals. This behavior was chosen as the assignment only specifed that the exit code has to be printed, and no mention was made of the signals received by the child. 
