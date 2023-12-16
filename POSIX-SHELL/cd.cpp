#include <iostream>
#include <unistd.h>
#include <string.h>
#include "current_dir.cpp"
using namespace std;
string temp1 ="";
void change_dir(vector<std::string> arg,string home_dir) 
{
    string new_directory;
    
    string direct_to_change=temp1;
    temp1=get_current_dir(); 
    //handling number of arguments
    if(arg.size()==1)
    {  
        new_directory=home_dir;
    }
    else if(arg.size()==2)
    {
        if(arg[1]=="~")
        {
            new_directory=home_dir;
        }
        else if(arg[1]=="-")
        {
            new_directory=direct_to_change;
            cout <<direct_to_change<<endl;
        }
        else
        {
            new_directory = arg[1];
        }   
    }
    else if(arg.size()>2)
    {
        cout<<"Too many arguments"<<endl;
        return;
    }
    //cout<<"hypehn dir "<<direct_to_change<<endl;
    if (chdir(new_directory.c_str()) == 0) 
    {
        //std::cout << new_directory << std::endl;
    } 
    else 
    {
        std::cerr << "Error changing directory.." << std::endl;
        cout<<"Invalid Directoty path or name"<<endl;
    }
}
  