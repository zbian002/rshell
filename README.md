ssignment2

Author

Zhen Bian

About Program

Write a command shell called rshell in C++. My shell perform the following steps:

Print a command prompt (e.g. $)

Read in a command on one line. Commands have the form:

cmd = executable [ argumentList ] [ connector cmd ]

connector = || or && or ;

where executable is an executable program in the PATH and argumentList is a list of zero or more arguments separated by spaces. The connector is an optional way that run multiple commands at once. If a command is followed by ;, then the next command is always executed; if a command is followed by &&, then the next command is executed only if the first one succeeds; if a command is followed by ||, then the next command is executed only if the first one fails. For example:

$ ls -a

$ echo hello

$ mkdir test

is equivalent to:

$ ls -a; echo hello; mkdir test

There should be no limit to the number of commands that can be chained together using these operators, and your program must be able to handle any combination of operators. For example, you should be able to handle the command:

$ ls -a; echo hello && mkdir test || echo world; git status

Execute the command. This require using the syscalls fork, execvp, and waitpid.

A special built-in command of exit which exits your shell.

Anything that appears after a # character is considered a comment. For example, in the command ls -lR /, you would execute the program /bin/ls passing into it the parameters -lR and /. But in the command ls # -lR /, you would execute /bin/ls, but you would not pass any parameters because they appear in the comment section. You should also note that the # may or may not be followed by a space before the comment begins

However, the thing is I failed to execute commands with ;, &&, || and with comments, I could only execute single commands.
