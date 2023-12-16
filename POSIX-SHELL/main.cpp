#include "ls.cpp"
#include "tokenize.cpp"
#include "execvp.cpp"
#include "search.cpp"
#include "current_dir.h"
#include "echo.cpp"
#include "cd.cpp"
#include "multiple_commands.cpp"
#include <filesystem>
#include <unistd.h>
#include <sys/utsname.h>
#include <string>
#include <cstring>
#include <fstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <vector>
using namespace std;
std::string hostname()
{
    char hostname[256];
    if(gethostname(hostname,sizeof(hostname))==-1)
    {
        perror("gethostname");
        return nullptr;
    }
    return hostname;
}
#include <iostream>
#include <cstring>
#include <cerrno>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <csignal>
#include <cstdlib>
class shell_history
{
    public:
        shell_history()
        {
            size=20;
            max_display=10;
            history_index=-1;
            load_history();
        }
        ~shell_history()
        {
            save_history();
        }
        int size;
        int max_display;
        vector<string> history;
        int history_index;
        void load_history();
        void save_history();
        void display_history(int num);
        void previous_command();
        void store_history(const string&);
};
void shell_history::load_history()
{
    ifstream file("history.txt");
    if(file.is_open())
    {
        string line;
        while(getline(file,line))
        {
            history.push_back(line);
        }
        file.close();
    }
}
void shell_history::save_history()
{
    ofstream file("history.txt");
    for(int i=0;i<history.size();i++)
    {
        file<<history[i]<<endl;
    }
    file.close();
}
void shell_history::display_history(int num)
{
    if(num>history.size())
    {
        cout<<"Index out of bound"<<endl;
    }
    int s=history.size()-num;
    for(int i=1;i<=num;i++)
    {
        cout<<history[s]<<endl;
        s++;
    }
} 
void shell_history::store_history(const string& input)
{
    if(history.size()>=size)
    {
        history.erase(history.begin());
    }
    history.push_back(input);
    history_index=history.size()-1;
}
void shell_history::previous_command()
{
    if(history_index==-1)
    {
        history_index=history.size()-1;
    }
    else
    {
        history_index--;
    }
    cout<<history[history_index]<<endl;
}
pid_t forg_process = -1;
void handling_control_z(int signum) 
{
    if (forg_process > 0) 
    {
        kill(forg_process, SIGSTOP);
        forg_process = -1;          //reset foreground process to -1 because pid will not be -1 for any process
    }
}
void handling_control_c(int signum)
{
    if (forg_process > 0)                   //foreground process exists
    {
        kill(forg_process, SIGINT);         
        forg_process = -1;                  //reset foreground process to -1 because pid will not be -1 for any process
    }
}
int main()
{
    string input;
    shell_history sh;
    int flag=1;
    char *username = std::getenv("USER");               //to get the username using environment variable
    string home_dir=get_current_dir();
    signal(SIGTSTP, handling_control_z); // handle CTRL-Z (SIGTSTP)
    signal(SIGINT, handling_control_c);  // handle CTRL-C (SIGINT)
    while(flag==1)
    {
        
        string temp=home_dir;
        //cout<<get_user_name()<<"@"<<hostname()<<":"<<get_current_dir()<<"->";
        string path=get_current_dir();
        if(path.size()>temp.size())
        {
            cout<<username<<"@"<<hostname()<<":~/"<<path.substr(temp.size()+1)<<">"<<" ";
        }
        else
        {
            cout<<username<<"@"<<hostname()<<":"<<"~>";   
        }
        if(!getline(cin,input))                     //ctrl+d
        {
            cout<<"Crl+d pressed Exiting the cell"<<endl;
            break;
        }
        sh.store_history(input);
        sh.save_history();
        std::vector<std::string> commands = command(input);
        for(int i=0;i<commands.size();i++)
        {
            std::vector<std::string> arg = my_string_tokenizer(commands[i].c_str());
            bool background = false;
            int n=arg.size();
            if(arg[n-1]=="&")    
            {
                background = true;
                arg.pop_back();
            }
            if(arg[0]=="ls")
            {
                my_ls(arg);
            }
            else if(arg[0]=="cd")
            {
                if(arg.size()==1)
                {
                    change_dir(arg,home_dir);
                }
                else if(arg.size()>1)
                {
                    if((strcmp(get_current_dir().c_str(),temp.c_str())==0)&&arg[1]=="..")
                    {
                        cout<<get_current_dir()<<endl;    
                    }
                    else
                    {
                        change_dir(arg,home_dir);
                    }
                }
            }
            else if(arg[0] == "echo")
            {
                my_echo(input);
                break;
            }
            else if(arg[0]=="pwd")
            {
                cout<<get_current_dir()<<endl;
            }
            else if(arg[0]=="pinfo")
            {
               //p_info(arg);
            }
            else if (arg[0]=="search")
            {
                if(search(arg))
                {
                    cout<<"true"<<endl;
                }
                else
                {
                    cout<<"false"<<endl;
                }
                break;
            }
            else if(arg[0]=="history"&&arg.size()>1)
            {
                sh.display_history(stoi(arg[1]));   
            }
            else if(arg[0]=="history")
            {
                cout<<"history"<<endl;
                sh.display_history(10);
            }
            else if (input == "^[[A" && sh.history_index >= 0) { // UP Arrow Key
                sh.previous_command();
            }
            else if(input !="exit")
            {
                execute_command(arg,background);
            }
            if(input == "exit")
            {
                cout<<"Exiting the cell";
                flag=0;
                break;
            }
        }
    }
    cout<<"\n";
    return 0;
}