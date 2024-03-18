#include    <unistd.h>
#include    <sys/types.h>
#include    <sys/wait.h>
#include    <errno.h>
#include    <string.h>
#include    <stdlib.h>
#include    <stdio.h>

#define     E_GENERAL   -1
#define     E_OK        0
#define     ENABLE      1
#define     DISABLE     0
#define     MAX_SIZE    1024


int         ec          =   E_OK;

int         fOptions    =   DISABLE;
int         fBackground =   DISABLE;

char *      executablePath;
char *      optionsBuffer;

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
    for (int i = 0; i < argc; i ++)
    {
        ec = write(0, argv[i], strlen(argv[i]));
        if (ec == -1)
        {
            ec = PrintError(errno);
            return ec;
        }   
        ec = write(0, " ", 1);
        if (ec == -1)
        {
            ec = PrintError(errno);
            return ec;
        }
    }
    if (argc == 1)
            ec = Help();
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
            break;
        default:
            return E_OK;
            break;
        }
    } 
    return E_OK; 
}

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



int
ExecuteInForeground(char *options)
{
    int wstatus, retVal;
    char **arguments = VectorizeString(options, executablePath);
    int pid = fork();
    if (pid == -1)
        return errno;
    else if (pid == 0)
    {
        int ec = execv(executablePath, arguments);
        return errno;
    }
    else
    {
        int w = waitpid(pid, &wstatus, WUNTRACED);
        if (w == -1)
            return errno;
        if (&wstatus != NULL)
        {
            if (WIFEXITED(wstatus))
            {
                retVal = WEXITSTATUS(wstatus);
            }
        }
        
    }

    char buffer[MAX_SIZE];
    snprintf(buffer, MAX_SIZE, "\n%s - %d", "Process ID of Child", pid);
    ec = write(0, buffer, strlen(buffer));
    if (ec == -1)
        return errno;
    if (retVal != 0)
    {
        snprintf(buffer, MAX_SIZE, "\n%s - %d", "Exit Code of Child: ", retVal);
        ec = write(0, buffer, strlen(buffer));
        if (ec == -1)
            return errno;
    }
    ec = E_OK;
    return ec;
}

int
ExecuteInBackground(char * options)
{
    char **arguments = VectorizeString(options, executablePath);
    int pid = fork();
    if (pid == -1)
        return errno;
    else if (pid == 0)
    {
        int ec = execv(executablePath, arguments);
        return errno;
    }
    else
    {
        char buffer[MAX_SIZE];
        snprintf(buffer, MAX_SIZE, "\n%s - %d", "Process ID of Child", pid);
        ec = write(0, buffer, strlen(buffer));
        if (ec == -1)
            return errno;
        return E_OK;
    }
}

char ** 
VectorizeString(char *optionsString, char *path)
{
    if (optionsString == "none")
    {
        char **options = (char **) calloc(2, sizeof(char *));
        options[0] = path;
        options[1] = NULL;
        return options;
    }
    char *testString = (char *) calloc(strlen(optionsString), 1);
    for (int i = 0; i < strlen(optionsString); i ++)
    {
        testString[i] = optionsString[i];
    }
    int i = 0;
    int elementCount = 1;
    int length = strlen(testString + 1);
    for (i = 0; i < length; i ++)
    {
        if(testString[i] == ' ')
            elementCount++;
    }

    char **options = (char **) calloc(elementCount + 2, sizeof(char *));
    options[0] = path;
    char * token = strtok(testString, " ");
    i = 1;
    while( token != NULL ) {
        options[i++] = token;
        token = strtok(NULL, " ");
    }
    options[i] = NULL;
    return options;
}

int Help()
{
    return 0;
}

int
PrintError(int errorCode)
{
    char buffer[MAX_SIZE];
    snprintf(buffer, MAX_SIZE, "\nError Code of C program: %d", errorCode);
    ec = write(0, buffer, strlen(buffer));
    if (ec == -1)
        return errno;
    return E_OK;
}