typedef struct job_node
{
    int pid;
    int jobid;
    struct job_node *next;
    char *cmd;
} job_node;

struct job_list
{
    int id;
    job_node *head;
    int count;
} jobs = {0, NULL, 0};

int get_job_id(int pid)
{
    job_node *job = jobs.head;
    while (job)
    {
        if (job->pid == pid)
            return job->jobid;
        job = job->next;
    }
    return -1;
}

int get_job_pid(int id)
{
    job_node *job = jobs.head;
    while (job)
    {
        if (job->jobid == id)
            return job->pid;
        job = job->next;
    }
    return -1;
}

int add_job(int pid, char *command)
{
    job_node *job = (job_node *)malloc(sizeof(job_node));
    job->pid = pid;
    job->next = jobs.head;
    jobs.head = job;
    jobs.id++;
    jobs.count++;
    job->jobid = jobs.id;
    job->cmd = (char *)malloc(LINE_SIZE);
    strcpy(job->cmd, command);
    return job->jobid;
}

int remove_job(int pid)
{
    job_node *job = jobs.head;
    if (job && jobs.head->pid == pid)
    {
        jobs.head = jobs.head->next;
        free(job->cmd);
        free(job);
        jobs.count--;
        return 1;
    }
    else if (job)
    {
        while (job->next)
        {
            if (job->next->pid == pid)
            {
                job_node *t = job->next;
                job->next = job->next->next;
                free(t->cmd);
                free(t);
                jobs.count--;
                return 1;
            }
            job = job->next;
        }
    }
    return 0;
}

int remove_job_id(int id)
{
    return remove_job(get_job_pid(id));
}

void print_jobs()
{
    if (DEBUG)
        printf("Jobs - count: %d ids: %d\n", jobs.count, jobs.id);
    job_node *job = jobs.head;
    while (job)
    {
        printf("[%d] Running %d %s \n", job->jobid, job->pid, job->cmd);
        job = job->next;
    }
}

void clean_and_wait_jobs()
{
    if (jobs.count > 0)
    {
        int wait_pid = 0, status;
        do
        {
            wait_pid = waitpid(-1, &status, WNOHANG);
            if (wait_pid == -1)
                if (DEBUG)
                    perror("waitpid2 error:");
            if (wait_pid > 0)
            {
                printf("completed [%d] %d\n", get_job_id(wait_pid), wait_pid);
                remove_job(wait_pid);
            }
            if(DEBUG)printf("Wait pid  result%d\n", wait_pid);
        } while (wait_pid > 0);
    }
}
