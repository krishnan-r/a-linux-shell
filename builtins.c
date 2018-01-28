void quit();
void print_jobs();
int get_job_pid(int id);
void clean_and_wait_jobs();

int check_builtin(char *cmd)
{
    if (strcmp(cmd, "exit") == 0)
        return 0;
    if (strcmp(cmd, "cd") == 0)
        return 1;
    if (strcmp(cmd, "jobs") == 0)
        return 2;
    if (strcmp(cmd, "kill") == 0)
        return 3;
    if (strcmp(cmd, "pwd") == 0)
        return 4;
    if (strcmp(cmd, "set") == 0)
        return 5;
    if (strcmp(cmd, "source") == 0)
        return 6;
    if (strcmp(cmd, "type") == 0)
        return 7;
    if (strcmp(cmd, "echo") == 0)
        return 8;
    if (strcmp(cmd, "history") == 0)
        return 9;
    if (strcmp(cmd, "debug") == 0)
        return 10;
    else
        return -1;
}

void run_builtin(int builtin, char *argv[], int argc, FILE *outfile)
{
    char str[LINE_SIZE];
    switch (builtin)
    {
    case 0: // exit
        quit();
        break;
    case 1: // cd
        if (argv[1] != NULL)
            chdir(argv[1]);
        break;
    case 2: // jobs
        print_jobs();
        break;
    case 3: // kill
        if (argv[1])
        {
            if (argv[1][0] == '%')
            {
                int jobid = strtol(argv[1] + 1, NULL, 10);
                int pid = get_job_pid(jobid);
                if (pid > 0)
                {
                    printf("Killing job id %d with pid %d\n", jobid, pid);
                    if (kill(pid, SIGKILL) == -1)
                        perror("Kill Error");
                }
                else {
                    printf("No such job. \n");
                }
            }
            else
            {
                int pid = strtol(argv[1], NULL, 10);
                if (pid > 0)
                {
                    printf("Killing pid %d\n", pid);
                    if (kill(pid, SIGKILL) == -1)
                        perror("Kill Error");
                }
            }
        }
        clean_and_wait_jobs();
        break;
    case 4: // pwd
        getcwd(str, LINE_SIZE);
        fprintf(outfile, "%s\n", str);
        break;
    case 5: // set
        break;
    case 6: // source
        break;
    case 7: // type
        for (int i = 1; i < argc; i++)
        {
            if (check_builtin(argv[i]) >= 0)
                fprintf(outfile, "%s is a shell builtin\n", argv[i]);
            else
            {
                char *path = NULL;
                path = realpath(argv[i], path);
                if (path != NULL)
                    fprintf(outfile, "%s is %s\n", argv[i], path);
            }
        }
        break;
    case 8: // echo
        for (int i = 1; i < argc; i++)
        {
            fprintf(outfile, "%s", argv[i]);
            if (i < argc - 1)
                fprintf(outfile, " ");
        }
        fprintf(outfile, "\n");
        break;
    case 9: // history
        break;
    case 10: //debug
        DEBUG = DEBUG ? 0 : 1;
        printf("Setting debug to %d\n", DEBUG);
        break;
    default:
        break;
    }
}