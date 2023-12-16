#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <vector>
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <sys/utsname.h>
#include <string>
#include <sstream>
#include <fstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/wait.h>
#include <vector>
#include <unordered_map>
#include <string>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <csignal>
#include <cstring>
#include <cerrno>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <csignal>
#include <cstdlib>
using namespace std;
void execute_command(vector<string> arg,bool background) 
{
    pid_t pid=fork();
    int status;
    int n=arg.size();
    char** arguments=new char *[n+1];
    for(int i=0;i<n;i++)
    {
        arguments[i]=const_cast<char*>(arg[i].c_str());
    }
    const char * command=const_cast<char*>(arg[0].c_str());
    if (pid < 0) 
    {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    } 
    else if (pid == 0) 
    {
        // child process
        execvp(command, arguments);
        perror("Execution failed");
        exit(EXIT_FAILURE);
    } 
    else 
    {
        // Parent process
        if(background)
        {
            cout<<"Pid is - "<<pid<<endl;
        }
        else
        {
            int status;
            waitpid(pid,&status,WUNTRACED);         //for waiting and also ctrl+z
        }
    }
}

