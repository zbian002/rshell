Assignment2

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

Assignment 3

Coding Requirements

The Test Command:

For this assignment I added the test command to rshell, as well as its symbolic equivalent [ ]. The square brackets [ ] are actually set up as the test command in bash, and try it out on my own. This command returns 0 (TRUE) if the test succeeds and 1 (FALSE) if the test fails. This command is very useful for writing conditions that can be combined with && and || to write more complex bash command structures.

Your subset of the test command should allow the user to run the command using the keyword test

$ test -e test/file/path

Additionally, your rshell should allow the user to use the symbolic version of the command

$ [ -e test/file/path ]

You command should also allow the user to run tests using the following flags

-e checks if the file/directory exists

-f checks if the file/directory exists and is a regular file

-d checks if the file/directory exists and is a directory

If a user does not specify a flag, then the -e functionality will be used by default.

You will also add an extra feature that the test command currently does not have. Your test command will print out to the terminal if it evaluated to True or False

If the command test -e /test/file/path evaluates to True, then print display the following

(True)

And likewise, if the above command evaluates to False, then print False in the same manner

(False)

Additionally, your test command should work with the connectors && and || that you have written previously in assignment 2, as well as all other functionality from assignment 2.

$ test -e /test/file/path && echo “path exists”

- or -

$ [ -e /test/file/path ] && echo “path exists”

This will check if path exists at /test/file/path, and if path does exist will print “path exists”.

When your input requires you to have multiple outputs, simply print the (True) or (False) labels as they are evaluated, so the above command (assuming the path exists) would print

(True)

path exists

Your test function should be designed to work with both full directory paths and relative directory paths. In order to create this command, you will need to use the stat() command, You will need to use the stat() function along with the S_ISDIR and S_ISREG
macros in order to implement all the flags required for this program.

The Precedence Operators

Additionally, you will implement parentheses ( ) as precedence operators in your rshell. The parentheses ( ) operators are used to change the precedence of the returns of commands, connectors, and chains of connectors. For example

$ echo A && echo B || echo C && echo D

Would print the following

A

B

D

However, we can add parentheses to change the precedence of the connectors

$ (echo A && echo B) || (echo C && echo D)

which would print

A

B
This is because the parentheses change the precedence, so that when (echo A && echo B) returns true (since it execute correctly) to the || operator then it will not run the entire group (echo C && echo D) since the parentheses have now grouped these commands together. Note that if for some reason echo A failed, echo B would not execute, and (echo C && echo D) would be executed.

If you are unsure about how the parentheses will change how a set of commands runs, try out some examples in your terminal.
