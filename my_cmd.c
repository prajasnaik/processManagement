#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>


#define     E_GENERAL   -1
#define     E_OK        0
#define     ENABLE      1
#define     DISABLE     0


int ec = E_OK;

int fPathName = DISABLE;
int fOptions  = DISABLE;
int fBackground = ENABLE;

char *executablePath;
char *optionsBuffer;

int     Help                    ();
int     ProcessCommandLine      (char **, int);
int     PerformOperations       ();


// Main function
int 
main(int argc, char *argv[])
{
    if (argc == 1)
            ec = Help();
    else
    {
        ec = ProcessCommandLine(argv, argc);
        ec = PerformOperations();
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
            fPathName = ENABLE;
            if(argno + 1 == argCount)
            {
                return E_GENERAL;
            }
            executablePath = commandLineArguments[argno + 1];
            argno += 2;
            break;
        case 'o':
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
    
}



char ** 
VectorizeString(char *string, char *path)
{
    char *testString = (char *) calloc(strlen(string), 1);
    for (int i = 0; i < strlen(string); i ++)
    {
        testString[i] = string[i];
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
   // loop through the string to extract all other tokens
    while( token != NULL ) {
        options[i++] = token;
        token = strtok(NULL, " ");
    }
    options[i] = NULL;
    return options;
}