# Linux Shell - Operating Systems Lab Assignment
![t](https://user-images.githubusercontent.com/6822941/35486013-da4459b6-048d-11e8-80b2-bc690cd8cac1.png)

## Features
- Uses GNU Readline for interactive input and history navigation.
- History Management
    - History is persisted to disk in ~/.oslab_history
    - History expansions and shell builtins.
- Job Management:
    - Running multiple jobs in background.
    - Killing Jobs based on job numbers
- Common shell builtins found in bash.
- Redirecting standard streams to files.

## Building from source
```
gcc main.c -lreadline -Wall -o shell
```

## Documentation

### Shell Builtins
- exit - quit shell
~~~bash
exit
~~~
- cd - Change directory
~~~bash
cd [directory]
~~~
- jobs - List active jobs
~~~bash
jobs
~~~
- kill - Kill active Job/Process
~~~bash
kill %[jobid]
kill [pid]
~~~
- pwd - Display current directory
~~~bash
pwd
~~~
- type - Check type of command - builtin/executable
~~~bash
type [command]
~~~
- echo
~~~bash
echo Hello World
~~~
- history - List history
~~~bash
history
!!  # run previous command  
!n  # run nth command in history.  
!-n # run nth command from end in history.
~~~

- debug - Toggle debug output.

~~~bash
debug
~~~

### Stream Redirection
Redirect stdin/stdout using `>` and `<` operators.
~~~bash
command < infiletxt > outfile.txt
~~~
