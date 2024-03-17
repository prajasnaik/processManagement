# Process Execution

#### Contributors: Prajas Naik

In this program, I have chosen to make the default behavior as the process running in the foreground, but this can be changed optionally.

If the number of arguments passed to the process is insufficient, it will terminate with error code -1. This was also a design decision to allow the process to indicate that the issue was with the arguments and not any other specific error.

Another design decision I made was to report only one error of write system call in standard output when printing which error code our C program is reporting. If the write system call fails another time, the value of errno is returned and no error code is written to standard output. 