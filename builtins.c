void quit();
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
    else
        return -1;
}

void run_builtin(int builtin, char *argv[], int argc)
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
        break;
    case 3: // kill
        break;
    case 4: // pwd
        getcwd(str, LINE_SIZE);
        printf("%s\n", str);
        break;
    case 5: // set
        break;
    case 6: // source
        break;
    case 7: // type
        for (int i = 1; i < argc; i++)
        {
            if (check_builtin(argv[i]) >= 0)
                printf("%s is a shell builtin\n", argv[i]);
            else
            {
                char *path=NULL;
                path=realpath(argv[i], path);
                if (path!=NULL)
                    printf("%s is %s\n", argv[i], path);
            }
        }
        break;
    case 8: // echo
        for(int i=1;i<argc;i++){
            printf("%s",argv[i]);
            if(i<argc-1)printf(" ");
        }
        printf("\n");
        break;
    case 9: // history
        break;
    default:
        break;
    }
}