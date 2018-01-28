
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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define LINE_SIZE 1024
#define ARG_LEN 50
#define HISTORY_SIZE 100
int DEBUG = 0;
#define MAX_JOBS 100

#define RED "\x1B[31m"
#define BLACK "\x1B[30m"
#define GREEN "\x1B[32m"
#define YELLOW "\x1B[33m"
#define BLUE "\x1B[34m"
#define MAGENTA "\x1B[35m"
#define CYAN "\x1B[36m"
#define WHITE "\x1B[37m"
#define RESET "\x1B[0m"

#define BGRED "\x1B[41m"
#define BGGREEN "\x1B[42m"
#define BGYELLOW "\x1B[43m"
#define BGBLUE "\x1B[44m"
#define BGMAGENTA "\x1B[45m"
#define BGCYAN "\x1B[46m"
#define BGWHITE "\x1B[47m"
#define BGRESET "\x1B[49m"

#include "builtins.c"
#include "jobs.c"

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
    snprintf(str, LINE_SIZE, YELLOW "%s " GREEN "%s@%s " BLACK "" BLUE "%s " BLACK "" WHITE "\n$ " RESET, timebuf, username, hostname, dir);
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
    printf(WHITE "  \n   Version 1.0 Written by: Krishnan R\n\n" RESET);
}

void print_args(char *argv[], int argc, char *prompt)
{
    int i;
    if (prompt)
        printf("%s", prompt);
    printf(" [");
    for (i = 0; i < argc; i++)
    {
        printf("'%s',", argv[i]);
    }
    printf("]\n");
}

int parse_command(char *cmd, char *argv[], char **infile, char **outfile, char *is_background)
{
    int argc = 0;
    char str[LINE_SIZE], *pch;
    strcpy(str, cmd);
    if (DEBUG)
        printf("Splitting command \"%s\" into tokens: [", str);
    pch = strtok(str, " ");
    while (pch != NULL)
    {
        if (DEBUG)
            printf(" %s, ", pch);

        if (pch[0] == '<')
        {
            if (strlen(pch) == 1)
                pch = strtok(NULL, " ");
            else
                pch++;
            if (pch)
            {
                *infile = strdup(pch);
                pch = strtok(NULL, " ");
            }
            continue;
        }

        if (pch[0] == '>')
        {
            if (strlen(pch) == 1)
                pch = strtok(NULL, " ");
            else
                pch++;
            if (pch)
            {
                *outfile = strdup(pch);
                pch = strtok(NULL, " ");
            }
            continue;
        }

        argv[argc] = (char *)malloc(strlen(pch));
        strcpy(argv[argc], pch);
        argc++;
        pch = strtok(NULL, " ");
    }

    if (argc > 0 && strcmp(argv[argc - 1], "&") == 0)
    {
        *is_background = 1;
        argc--;
        free(argv[argc]);
        argv[argc] = NULL;
        if (DEBUG)
            print_args(argv, argc, "After Background: ");
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
        char *cmdline, *cmdlist[ARG_LEN] = {NULL}, *infile = NULL, *outfile = NULL;
        char is_background = 0;
        clean_and_wait_jobs();
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
            if (history_expand(cmdline2, &cmdline) == 0)
                add_history(cmdline);
        }
        //printf("%s", cmdline);

        int cmdlen = parse_command(cmdline, cmdlist, &infile, &outfile, &is_background);
        if (DEBUG)
            print_args(cmdlist, cmdlen, "Parsed Command: ");
        if (DEBUG)
            printf("Infile: %s Outfile %s Background: %d\n", infile, outfile, is_background);
        if (cmdlen == 0)
        {
            for (i = 0; i < cmdlen; i++)
                free(cmdlist[i]);
            continue;
        }

        int builtin = check_builtin(cmdlist[0]);
        if (builtin >= 0)
        {
            FILE *of;
            if (outfile)
            {
                of = fopen(outfile, "w");
                if (!of)
                {
                    printf("Error: %s - File not found.\n", outfile);
                }
            }
            else
                of = stdout;

            run_builtin(builtin, cmdlist, cmdlen, of);
            if (outfile)
                fclose(of);
        }
        else
        {
            if ((pid = fork()) == -1)
            {
                perror("Fork Error");
            }

            else if (pid == 0)
            { //run inside child
                char result[LINE_SIZE];
                snprintf(result, LINE_SIZE, "Exec result: '%s'", cmdlist[0]);
                if (infile)
                {
                    if (DEBUG)
                        printf("Redirecting stdin via %s \n", infile);
                    int fin = open(infile, O_RDONLY);
                    if (fin == -1)
                    {
                        perror("stdin Redirect Error");
                    }
                    else
                    {
                        dup2(fin, 0);
                        close(fin);
                    }
                }
                if (outfile)
                {
                    if (DEBUG)
                        printf("Redirecting stdout to %s \n", outfile);
                    int fout = open(outfile, O_WRONLY | O_CREAT, S_IRWXU);
                    if (fout == -1)
                    {
                        perror("stdout Redirect Error");
                    }
                    else
                    {
                        dup2(fout, 1);
                        close(fout);
                    }
                }
                execvp(cmdlist[0], cmdlist);
                perror(result);
                return EXIT_FAILURE;
            }

            else
            {
                int status, wait_pid = -1;
                int jobid = add_job(pid, cmdlist[0]);
                if (!is_background)
                {
                    wait_pid = waitpid(pid, &status, 0);
                    if (wait_pid == -1)
                        perror("waitpid error:");
                    remove_job(wait_pid);
                }
                else
                {
                    printf("started [%d] %d\n", jobid, pid);
                    clean_and_wait_jobs();
                }
            }
        };

        for (i = 0; i < cmdlen; i++)
            free(cmdlist[i]);
        if (infile)
            free(infile);
        if (outfile)
            free(outfile);
    }
    return 0;
}
