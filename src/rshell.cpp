#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>
#include <cstdlib>
#include <sys/types.h>
#include <stdlib.h>
#include <pwd.h>
#define LSH_TOK_BUFSIZE 64
#define LSH_TOK_DELIM " \t\r\n\a"
const int max_length = 256;

using namespace std;

void type_prompt()
{
    struct passwd *pwd;
    char hostname[max_length];
    pwd = getpwuid(getuid());
    if(gethostname(hostname,max_length)==0)
	cout << "[rshell]" << pwd->pw_name << "@" << hostname << "$ ";
    else
	cout << "[rshell]" << pwd->pw_name << "@" << "anonymous" << "$ ";
    return;
}

char **parse_line(char *line)
{
  int bufsize = LSH_TOK_BUFSIZE, position = 0;
  char **tokens = (char**)malloc(bufsize * sizeof(char*));
  char *token;

  if (!tokens) {
    fprintf(stderr, "lsh: allocation error\n");
    exit(1);
  }

  token = strtok(line, LSH_TOK_DELIM);
  while (token != NULL) {
    tokens[position] = token;
    position++;

    if (position >= bufsize) {
      bufsize += LSH_TOK_BUFSIZE;
      tokens = (char**)realloc(tokens, bufsize * sizeof(char*));
      if (!tokens) {
        fprintf(stderr, "lsh: allocation error\n");
        exit(1);
      }
    }

    token = strtok(NULL, LSH_TOK_DELIM);
  }
  tokens[position] = NULL;
  return tokens;
}

int command_exec(char **args)
{
  pid_t pid, wpid;
  int status;
  pid = fork();
  switch(pid){  
   case -1:  
     perror("fork failed");  
     exit(1);  
   case 0:  
     execvp(args[0], args);
     perror("execvp failed");  
     exit(1);  
   default:  
     do {
      wpid = waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));  
  }
  return 1;
}  

void loop(void)
{
  char **args;
  int status;
  char *line;
  string input;

  do {
    type_prompt();
    getline(cin,input);
    if (input=="exit")
	break;
    char*line = (char*)input.data();
    args = parse_line(line);
    status = command_exec(args);
    free(args);
  }while (status);
}

int main()
{
 
  loop();

  return 0;
}
