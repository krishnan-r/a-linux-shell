
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <unistd.h>
#include <limits.h>
#include <time.h>

#define LINE_SIZE 1024
#define ARG_LEN 50
#define HISTORY_SIZE 100
#define DEBUG 0

#define RED "\x1B[31m"
#define GREEN "\x1B[32m"
#define YELLOW "\x1B[33m"
#define BLUE "\x1B[34m"
#define MAGENTA "\x1B[35m"
#define CYAN "\x1B[36m"
#define WHITE "\x1B[37m"
#define RESET "\x1B[0m"

#include "builtins.c"

char *prompt()
{
    time_t timer;
    char timebuf[26];
    struct tm *tm_info;

    time(&timer);
    tm_info = localtime(&timer);

    strftime(timebuf, 26, "[%H:%M:%S]", tm_info);

    char hostname[HOST_NAME_MAX];
    char username[LOGIN_NAME_MAX];
    gethostname(hostname, HOST_NAME_MAX);
    getlogin_r(username, LOGIN_NAME_MAX);

    char dir[LINE_SIZE];
    char *str = (char *)malloc(LINE_SIZE);
    getcwd(dir, LINE_SIZE);
    snprintf(str, LINE_SIZE, YELLOW "%s " GREEN "%s@%s" WHITE ":" BLUE "%s" WHITE "$ " RESET, timebuf, username, hostname, dir);
    return str;
}

char *read_command_line()
{
    char *p = prompt();
    char *line = readline(p);
    free(p);
    if (line != NULL)
    {
        char *l = (char *)malloc(LINE_SIZE);
        strcpy(l, line);
        free(line);
        return l;
    }
    else
        return NULL;
}

void print_splash()
{

    printf("\033[2J");
    printf(WHITE "   ___  ____    _        _    ____    ____  _          _ _ \n" RESET);
    printf(WHITE "  / _ \\/ ___|  | |      / \\  | __ )  / ___|| |__   ___| | |\n" RESET);
    printf(WHITE " | | | \\___ \\  | |     / _ \\ |  _ \\  \\___ \\| '_ \\ / _ \\ | |\n" RESET);
    printf(WHITE " | |_| |___) | | |___ / ___ \\| |_) |  ___) | | | |  __/ | |\n" RESET);
    printf(WHITE "  \\___/|____/  |_____/_/   \\_\\____/  |____/|_| |_|\\___|_|_|" RESET);
    printf(WHITE "  Version 1.0 Written by: Krishnan R\n\n" RESET);
}
int parse_command(char *cmd, char *argv[])
{
    int argc = 0;
    char str[LINE_SIZE], *pch;
    strcpy(str, cmd);
    if (DEBUG)
        printf("\nSplitting command \"%s\" into tokens: [", str);
    pch = strtok(str, " ");
    while (pch != NULL)
    {
        if (DEBUG)
            printf(" %s, ", pch);
        argv[argc] = (char *)malloc(strlen(pch));
        strcpy(argv[argc], pch);
        argc++;
        pch = strtok(NULL, " ");
    }
    if (DEBUG)
        printf("] with %d arguments\n", argc);
    return argc;
}
void quit()
{
    exit(0);
}

int main(int argc, char **argv)
{
    print_splash();
    pid_t pid = -1;
    while (1)
    {
        int i;
        char *cmdline, *cmdlist[ARG_LEN] = {NULL};
        char background_job = 0;

        cmdline = read_command_line(); //To read the command line given by the user.

        if (cmdline == NULL)
        {
            quit();
            continue;
        }

        if (*cmdline)
        {
            char cmdline2[LINE_SIZE];
            strcpy(cmdline2, cmdline);
            if(history_expand(cmdline2, &cmdline)==0)add_history(cmdline);
        }
        //printf("%s", cmdline);

        int cmdlen = parse_command(cmdline, cmdlist);
        if (cmdlen == 0)
            continue;

        if (strcmp(cmdlist[cmdlen - 1], "&") == 0)
        {
            background_job = 1;
            cmdlen--;
            free(cmdlist[cmdlen - 1]);
            cmdlist[cmdlen - 1] = NULL;
        }
        int builtin = check_builtin(cmdlist[0]);
        if (builtin >= 0)
        {
            run_builtin(builtin, cmdlist, cmdlen);
        }
        else
        {
            pid = fork();
            if (pid == 0)
            { //child
                char result[LINE_SIZE];
                snprintf(result, LINE_SIZE, "Exec result: '%s'", cmdlist[0]);
                execvp(cmdlist[0], cmdlist);
                perror(result);
                return 1;
            }
            else
            { // parent
                int status;
                wait(&status);
            }
        };

        for (i = 0; i < cmdlen; i++)
        {
            free(cmdlist[i]);
        }
    }
    return 0;
}
