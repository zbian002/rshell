#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>
#include <cstdlib>
#include <sys/types.h>
#include <stdlib.h>
#include <fcntl.h>
#include <fstream>
#include <signal.h>
#include <pwd.h>
#include <sys/stat.h>
#include <csignal>
#include <limits.h>
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

void executer1(int signum)
{
	if(signum==SIGINT)
	{
		cout << endl<<flush;
	}
}

void start(char *inputchar,  char** argv)
{

	char *word = strtok(inputchar,"");
	int i =0;
	while (word != NULL)
	{
		argv[i] = word;
		
		word = strtok(NULL, "");
		i++;
	}
	argv[i]=0;
	delete[] word;
}
void checker(char** argvIN,char **argvOUT,char* ops,int& sz)
{
	int j=0;
	char *word= strtok(argvIN[0],ops);
	
	while (word!=NULL)
	{
		argvOUT[j] = word;

		word = strtok(NULL, ops);
		j++;
	}
	argvOUT[j]=0;
	sz=j;							
	delete[] word;				
}

int number_of_io_redirections =0;

void cd(string input)
{
	char *cd		= new char[3];
	char *spa		= new char[3];

	strcpy(cd,"cd");
	strcpy(spa," ");
	char *inputchar = new char [input.length()+1];
	strcpy(inputchar,input.c_str());
	
	char **argvIN     = new char*[strlen(inputchar)];
	char **argvSPACE  = new char*[strlen(inputchar)];
	int sz;
	start(inputchar,argvIN); 
	checker(argvIN,argvSPACE,spa,sz);
	string checker=argvSPACE[0];
	if (checker!="cd" || sz>2)
	{
		cout << "Error: invalid call of the cd command" << endl;
		delete[] cd;
		delete[] spa;
		
		delete[] inputchar;
		delete[] argvIN;
		delete[] argvSPACE;
		return;
	}

	char* pPath;
	char* cwd;
	char buff[PATH_MAX + 1];
	
	if(argvSPACE[1]==NULL)
	{
		cwd = getcwd( buff, PATH_MAX + 1 );
		if( cwd == NULL )
			perror("getcwd");
		if(-1==setenv("OLDPWD",cwd,1))
			perror("setenv");

		pPath=getenv("HOME");
		if(pPath==NULL)
			perror("getenv");
		if(-1==chdir(pPath))
			perror("chdir");
		if(-1==setenv("PWD",pPath,1))
			perror("setenv");
			pPath = getenv("OLDPWD");

		delete cd;
		delete spa;
		
		delete[] inputchar;
		delete[] argvIN;
		delete[] argvSPACE;
		return;

	}

	checker = argvSPACE[1];
	if(checker=="-")//case where it is cd -
	{
		cwd=getcwd(buff,PATH_MAX+1);
		if(cwd==NULL)
			perror("getcwd");
		pPath=getenv("OLDPWD");
		if(pPath==NULL)
			perror("getenv");

		if(-1==chdir(pPath))
			perror("chdir");
		if(-1==setenv("PWD",pPath,1))
			perror("setenv");
		if(-1==setenv("OLDPWD",cwd,1))
			perror("setenv");

	}
	else //case where it is cd <pathname>
	{
		cwd = getcwd( buff, PATH_MAX + 1 );
		if( cwd == NULL )
			perror("getcwd");
		if(-1==setenv("OLDPWD",cwd,1))
			perror("setenv");
		if(checker=="~")
		{
			pPath = getenv("HOME");
			if(pPath==NULL)
				perror("getenv");
			checker=pPath;
		}
		if(-1==chdir(checker.c_str()))
			perror("chdir");
		cwd = getcwd(buff,PATH_MAX+1);
		if(cwd==NULL)
			perror("getcwd");
		if(-1==setenv("PWD",cwd,1))
			perror("setenv");
	}
}

void executer(char **argv,int& status)
{
	
	int pid=fork();
	if (pid == -1)
	{
		perror("fork");
		exit(1);
	}
	else if (pid ==0){
	
		if(-1==execvp(argv[0],argv))
		{
			status = -1;
			perror("execvp");
		}
		exit (1);
	}
	else if (pid >0)
	{
		
		sigignore(SIGINT);
		if (-1 == wait(&status))
		{
			perror("wait");
			exit(1);
		}
		struct sigaction sa1;
		memset(&sa1, 0, sizeof(sa1));

		sa1.sa_handler = executer1;
		if(sigaction(SIGINT,&sa1,NULL)==-1)
			perror("sigaction");


	}
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

int main()
{
		struct sigaction sa1;
		memset(&sa1, 0, sizeof(sa1));

		sa1.sa_handler = executer1;
		
		if(sigaction(SIGINT,&sa1,NULL)==-1)
			perror("sigaction");

		char* cwd;

		char buff[PATH_MAX + 1];

		cwd = getcwd( buff, PATH_MAX + 1 );
		if( cwd == NULL )
			perror("getcwd");
		
		if(-1==setenv("OLDPWD",cwd,1))
			perror("setenv");

	int status=0;

	while (1)
	{
		// to change the host and the filename everytime
		cin.clear();
		string prompt; 
		char host[400];
		char *login = getlogin();	
		prompt = login;

		if (!login) 
			perror("getlogin"); //if its not there error

		if (gethostname(host,300) !=0) //get host info
			perror("gethostname");  // error otherwise
	
		char* pPath1;
		pPath1 = getenv ("HOME");
		if (pPath1==NULL)
			perror("getenv");

		char* cwd;

		char buff[PATH_MAX + 1];

		cwd = getcwd( buff, PATH_MAX + 1 );
		if( cwd == NULL )
			perror("getcwd");
		string tpath = pPath1;
		string tpath2 = cwd;
		string prompt1;
		if(tpath2.find(tpath)!=string::npos)
		{
			prompt1 = tpath2.substr(tpath.size(),tpath2.size());
			prompt1 = "~" + prompt1;
		}
		else
		{
			prompt1 = tpath2;
		}


		if (login && host!=NULL)
		{
			for (int i=0;i<50;i++)
			{
				if (host[i]=='.')
					host[i]='\0'; 
			}

			prompt = prompt+"@"+host+ prompt1 + " $ "; 
		}
		else 
		{
			prompt = prompt1 + "$ ";

		}
		string input;
		number_of_io_redirections=0;
		cout << prompt;
		
		getline(cin,input);
	    // chech the #
		if (input.find("#") != string::npos)
		{
			input.resize(input.find("#"));
		}
		
		bool cd_check = false;
			
		if(input.find("cd")!=string::npos)
		{
			char *spa		= new char[3];
			strcpy(spa," ");
			char *inputchar = new char [input.length()+1];
			strcpy(inputchar,input.c_str());
			
			char **argvIN     = new char*[strlen(inputchar)];
			char **argvSPACE  = new char*[strlen(inputchar)];
			int sz;
			start(inputchar,argvIN); 
			checker(argvIN,argvSPACE,spa,sz);
			string checker=argvSPACE[0];
			if(checker == "cd")
			{
				cd_check = true;
				cd(input);
			}

			delete[] spa;
			delete[] inputchar;
			delete[] argvIN;
			delete[] argvSPACE;
		}
		if(!cd_check)
		{
		    type_prompt();
			getline(cin,input);
		if (input=="exit")
			break;
		loop(input);
		}
	}
		return 0;
}