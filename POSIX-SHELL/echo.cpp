#include <fcntl.h>
#include <iostream>
#include <dirent.h>
#include <cstring>
#include <cerrno>
#include <vector>
#include <sstream>
#include <iostream>
#include <unistd.h>
using namespace std;
void my_echo(string input) 
{
    std::string output="";
    int i=0;
    while(input[i]!='\0')
    {
        if(input[i]==' '||input[i]=='\t')
            i++;
        else 
            break;
    }
    i=i+5;
    for(int j=i;input[i]!='\0';j++)
    {
        if(input[j]==';')
            break;
        output+=input[i];
        i++;
    }
    cout<<output<<endl;
}