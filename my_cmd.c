// @date: 18 March 2024
// @author: Prajas Naik
// Simple process management program

// Include Statements
#include    <unistd.h>
#include    <sys/types.h>
#include    <sys/wait.h>
#include    <errno.h>
#include    <string.h>
#include    <stdlib.h>
#include    <stdio.h>

//Define Statements (Macros)
#define     E_GENERAL   -1
#define     E_OK        0
#define     ENABLE      1
#define     DISABLE     0
#define     MAX_SIZE    1024

// Global Variable for Error Code
int         ec          =   E_OK;

// Global Variable Flags for deciding which operation to perform
int         fOptions            =   DISABLE;
int         fBackground         =   DISABLE;
int         fExecutablePath     =   DISABLE;

// Buffer pointers for storing paths for each function
char *      executablePath;
char *      optionsBuffer;

// Function Declarations
int         Help                    ();
int         ProcessCommandLine      (char **, int);
int         PerformOperations       ();
int         ExecuteInForeground     (char *);
int         ExecuteInBackground     (char *);
char **     VectorizeString         (char *, char *);
int         PrintError              (int );


// Main function
int 
main(int argc, char *argv[])
{
    for (int i = 0; i < argc; i ++) //used to print all the command line arguments received by the process separated by a space
    {
        ec = write(STDOUT_FILENO, argv[i], strlen(argv[i]));
        if (ec == E_GENERAL)
        {
            ec = PrintError(errno);
            return ec;
        }   
        ec = write(STDOUT_FILENO, " ", 1);
        if (ec == E_GENERAL)
        {
            ec = PrintError(errno);
            return ec;
        }
    }
    if (argc == 1)
    {
        ec = Help(); //Help is called if no options are given
        if (ec != E_OK)
        {
            ec = PrintError(ec);
            return ec;
        }
    }
    else
    {        
        ec = ProcessCommandLine(argv, argc);
        if (ec != E_OK)
        {
            ec = PrintError(ec);
            return ec;
        }
        ec = PerformOperations();
        if (ec != E_OK)
        {
            ec = PrintError(ec);
            return ec;
        }
    }
    
    return ec;       
}

// function: ProcessCommandLine
//      This function takes the command line arguments and appropriately sets 
//      flags for which operations need to be performed. It also extracts 
//      appropiate pointers
//  @param: commandLineArguments - Pointer to array containing command line 
//          arguments
//  @param: argCount - Integer count of total  number of command line arguments
//  @return: Integer error code
int 
ProcessCommandLine(char *commandLineArguments[], int argCount)
{
    int argno = 1;
    while(argno < argCount)
    {
        switch (commandLineArguments[argno][1])
        {
        case 'e':
            if(argno + 1 == argCount)
            {
                return E_GENERAL;
            }
            fExecutablePath = ENABLE;
            executablePath = commandLineArguments[argno + 1];
            argno += 2;
            break;
        case 'o':
            if(argno + 1 == argCount)
            {
                return E_GENERAL;
            }
            fOptions = ENABLE;
            optionsBuffer = commandLineArguments[argno + 1];
            argno += 2;
            break;
        case 'b':
            fBackground = ENABLE;
            argno += 1;
            break;
        case 'f':
            fBackground = DISABLE;
            argno += 1;
            break;
        default:
            return E_GENERAL;
            break;
        }
    } 
    if(!fExecutablePath)
    {
        char *buf = "\nNo executable path specified, please use -e flag to provide path.";
        ec = write(STDOUT_FILENO, buf, strlen(buf));
        if (ec == E_GENERAL)
        {
            ec = PrintError(errno);
            return ec;
        }
        return E_GENERAL;
    }
    return E_OK; 
}


//  function: PerformOperations
//      This function calls and executes appropriate functions based on
//      which flags were set after processing command line.
//  @param: None
//  @return: Integer Error Code
int
PerformOperations()
{
    if(fBackground)
    {
        if (fOptions)
        {
            ec = ExecuteInBackground(optionsBuffer);
        }
        else
        {
            ec = ExecuteInBackground("none");
        }
    }
    else
    {
        if(fOptions)
            ec = ExecuteInForeground(optionsBuffer);
        else
        {
            ec = ExecuteInForeground("none");
        }
    }
    return ec;
}


//  function: ExecuteInForeground
//      This function executes a given binary specified by a path in the foreground
//      The options for this binary are optional and must be given as "none" if no
//      options are to be applied.
//  @param: pointer to a character array containing options string
//  @return: integer error code
int
ExecuteInForeground(char *options)
{
    int wstatus, retVal;
    char **arguments = VectorizeString(options, executablePath); 
    int pid = fork();
    if (pid == E_GENERAL)
        return errno;
    else if (pid == 0)
    {
        int ec = execv(executablePath, arguments);
        return errno; //only returns if error occurred in execv() system call
    }
    else
    {
        int w = waitpid(pid, &wstatus, WUNTRACED);
        if (w == E_GENERAL)
            return errno;
        if (&wstatus != NULL)
        {
            if (WIFEXITED(wstatus))
            {
                retVal = WEXITSTATUS(wstatus); //finds exit status of child if not 0
            }
        }
        
    }

    char buffer[MAX_SIZE];
    snprintf(buffer, MAX_SIZE, "\n%s - %d", "Process ID of Child", pid);
    ec = write(STDOUT_FILENO, buffer, strlen(buffer));
    if (ec == E_GENERAL)
        return errno;
    if (retVal != E_OK)
    {
        snprintf(buffer, MAX_SIZE, "\n%s - %d", "Exit Code of Child: ", retVal);
        ec = write(0, buffer, strlen(buffer));
        if (ec == -1)
            return errno;
    }
    ec = E_OK;
    return ec;
}

//  function: ExecuteInForeground
//      This function executes a given binary specified by a path in the background
//      The options for this binary are optional and must be given as "none" if no
//      options are to be applied.
//  @param: pointer to a character array containing options string
//  @return: integer error code
int
ExecuteInBackground(char * options)
{
    char **arguments = VectorizeString(options, executablePath);
    int pid = fork();
    if (pid == E_GENERAL)
        return errno; 
    else if (pid == 0)
    {
        int ec = execv(executablePath, arguments);
        return errno; //only returns if error occurred in execv() system call
    }
    else
    {
        char buffer[MAX_SIZE];
        snprintf(buffer, MAX_SIZE, "\n%s - %d", "Process ID of Child", pid);
        ec = write(0, buffer, strlen(buffer));
        if (ec == E_GENERAL)
            return errno;
        return E_OK;
    }
}


//  function: VectorizeString
//      This function takes a string of character options as well as executable path and
//      constructs a null terminated array of strings that can be passed to the execv() 
//      system call. The format of the array is similar to the command line options received
//      by processes as command line arguments. 
//  @param: pointer to a character array containing options string
//  @param: pointer to a character array containing an executable path
//  @return: pointer to the vectorized array of strings
char ** 
VectorizeString(char *optionsString, char *path)
{
    if (optionsString == "none")
    {
        char **options = (char **) calloc(2, sizeof(char *)); //gets a pointer to an array of pointers
        options[0] = path;
        options[1] = NULL;
        return options;
    }
    char *testString = (char *) calloc(strlen(optionsString), 1); //gets a new character array
    for (int i = 0; i < strlen(optionsString); i ++)
    {
        testString[i] = optionsString[i]; //sets the new array equal to the old one
    }
    int i = 0;
    int elementCount = 1; //Set to one because an array without spaces still has one element
    int length = strlen(testString + 1); // 
    for (i = 0; i < length; i ++)
    {
        if(testString[i] == ' ')
            elementCount++; //checks for number of spaces in the array
    }

    char **options = (char **) calloc(elementCount + 2, sizeof(char *)); // Is + 2 because of the addition of the path at the start and the null character at the end
    options[0] = path;
    char * token = strtok(testString, " "); //splits the string on spaces and appends to argument array
    i = 1;
    while( token != NULL ) {
        options[i++] = token; 
        token = strtok(NULL, " "); 
    }
    options[i] = NULL; //terminates array with null character
    return options;
}

// function: Help
//      Provides a basic message on how to use the program.
//  @param: None
//  @return: Integer Error Code
int 
Help()
{
    char *helpMessage = "\n\tUsage:\n\t./my_cmd -e <ExecutablePath> -o <OptionsString> [-f|-b] \n\tFor more info, please refer to README file\n";
    int length = strlen(helpMessage);
    int error = write(STDOUT_FILENO, helpMessage, length);
    if (error == E_GENERAL)
        return errno;
    return E_OK;
}

//  function: PrintError
//      This function takes an error code that occurred during a program's
//      execution and prints a message onto standard output
//  @param: integer error code that needs to be printed
//  @return: integer error code 
int
PrintError(int errorCode)
{
    char buffer[MAX_SIZE];
    snprintf(buffer, MAX_SIZE, "\nError Code of C program: %d", errorCode);
    ec = write(STDOUT_FILENO, buffer, strlen(buffer));
    if (ec == E_GENERAL)
        return errno;
    return E_OK;
}