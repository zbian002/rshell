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
#include <sys/stat.h>
#include <dirent.h>
#define LSH_TOK_BUFSIZE 64
const int max_length = 256;

using namespace std;

void type_prompt() {
	struct passwd *pwd;
	char hostname[max_length];
	int length;
	pwd = getpwuid(getuid());
	if(gethostname(hostname,max_length)==0)
		cout << "[rshell]" << pwd->pw_name << "@" << hostname << "$ ";
	else
		cout << "[rshell]" << pwd->pw_name << "@" << "anonymous" << "$ ";
	return;
}

char **parse_space(char *cmd) {
	int bufsize = LSH_TOK_BUFSIZE, position = 0;
	char **tokens = (char**)malloc(bufsize * sizeof(char*));
	char *token;

	if (!tokens) {
		fprintf(stderr, "lsh: allocation error\n");
		exit(1);
	}

	token = strtok(cmd, " ");
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
	token = strtok(NULL, " ");
	}
	tokens[position] = NULL;
	return tokens;
}

string dealwithPound(string input){
         unsigned i=0;
	for(i=0;i<input.size();i++){
		if(input[i]=='#'){
			input=input.substr(0,i);
		}
	}
	return input;
}

string getConnector(string input) {
	string connector;
	for(unsigned i=0;i<input.size()-1;i++) {
		if(input[i]=='&'&&input[i+1]=='&') 
			connector=connector+"&";
		
		else if(input[i]=='|'&&input[i+1]=='|') 
			connector=connector+"|";
		
		else if(input[i]==';')
			connector=connector+";";
	}
	return connector;
}

int command_exec(char **args) {
	pid_t pid, wpid;
	int status;
	pid = fork();
	switch(pid) {  
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
	return status;
}  

void loop(string input) {
	char **args;
	int status;
	int bufsize = LSH_TOK_BUFSIZE;
	int position = 0;
	bool flag=0;
	char **tokens = (char**)malloc(bufsize * sizeof(char*));
	char *token;
	
	input=dealwithPound(input);
	string connector=getConnector(input);
	char*line = (char*)input.data();
	if (!tokens) {
		fprintf(stderr, "lsh: allocation error\n");
		exit(1);
	}

	token = strtok(line, ";&|\r\n");
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

		token = strtok(NULL, ";&|\r\n");
	}
	tokens[position] = NULL;
	
	for (int i = 0, j = 0; i < position; i++, j++) {
		if (flag==0) {
			
			if ((tokens[i][0] == '[') || (tokens[i][1] == '[') || ((tokens[i][0] == 't') && 
			(tokens[i][1] == 'e') || (tokens[i][1] == 't') && (tokens[i][2] == 'e'))) {
				struct stat info;
				int k = 0;
				if ((tokens[i][0] == '[') || (tokens[i][1] == '[')) {
					while (1) {
						if (tokens[i][k] == ']')
							break;
						k++;
					}
					string test = "test";
					string temp2 = tokens[i];
					string temp3=test+temp2.substr(1, k-2);
					char* test_cmd = (char*)temp3.data();
					args = parse_space(test_cmd);
					string path = args[2];
					int a = stat (path.c_str(), &info);
					if(a == 1)
						cout<<"(False)"<<endl;
					else if(strcmp(args[1],"-e")==0&&(S_ISREG(info.st_mode)||S_ISDIR(info.st_mode)))
						cout<<"(True)"<<endl;
					else if(strcmp(args[1],"-d")==0&&(S_ISDIR(info.st_mode)))
						cout<<"(True)"<<endl;
					else if(strcmp(args[1],"-f")==0&&(S_ISREG(info.st_mode)))
						cout<<"(True)"<<endl;
					else
						cout<<"(False)"<<endl;
				}
				else {
					args = parse_space(tokens[i]);
					string path = args[2];
					int a = stat (path.c_str(), &info);
					if(a == 1)
						cout<<"(False)"<<endl;
					else if(strcmp(args[1],"-e")==0&&(S_ISREG(info.st_mode)||S_ISDIR(info.st_mode)))
						cout<<"(True)"<<endl;
					else if(strcmp(args[1],"-d")==0&&(S_ISDIR(info.st_mode)))
						cout<<"(True)"<<endl;
					else if(strcmp(args[1],"-f")==0&&(S_ISREG(info.st_mode)))
						cout<<"(True)"<<endl;
					else
						cout<<"(False)"<<endl;
				}
			}
			else {
				args = parse_space(tokens[i]);
				status = command_exec(args);
			}
			free(args);
		}
		if(connector[j]==';')
			flag=0;
		else if(connector[i]=='&'&&status!=0)
			flag=1;
		else if(connector[i]=='|'&&status==0)
			flag=1;
	}
}

int main(int argc, char **argv) {
	string input;
	
	while(1) {
		type_prompt();
		getline(cin,input);
		if (input=="exit")
			break;
		loop(input);
	}
	return 0;
}
