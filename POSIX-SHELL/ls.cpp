#include <fcntl.h>
#include <iostream>
#include <dirent.h>
#include <cstring>
#include <cerrno>
#include <vector>
#include <sstream>
#include <iostream>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <ctime>
#include <sys/stat.h>
#include <sys/types.h>
#include <iomanip>
using namespace std;
const char* dir_path;
void my_ls(vector<std::string> arg)
{
    int flag=1;
    bool hidden = false;
    bool details = false;
    vector<string> dir_path;
    dir_path.push_back(".");            //initially current directory
    //iterate over the complete input
    int i=1;
    while(i<arg.size())
    {
        if(arg[i]=="-a")
        {
            hidden=true;
            details=false;
        }
        else if(arg[i]=="-al")
        {
            hidden=true;
            details=true;
        }
        else if(arg[i]=="-l")
        {
            hidden=false;
            details=true;
        }
        else if(arg[i]=="-la")
        {
            hidden=true;
            details=true;
        }
        else
        {
            if(flag)
            {
                dir_path.pop_back();
                flag=0;
            }
            dir_path.push_back(arg[i]);
        }
        i++;
    }
    int j=0;
    while(j<dir_path.size())
    {
        struct stat file_obj;
        if(stat(dir_path[j].c_str(), &file_obj)==0 && S_ISDIR(file_obj.st_mode))
        {
            DIR* dir = opendir(dir_path[j].c_str());
            if(dir==nullptr)
            {
                cout<<"Error Opening Directory: "<<endl;
            }
            struct dirent* entry;
            while((entry=readdir(dir))!=nullptr)
            {
                if (!hidden && entry->d_name[0] == '.') 
                {
                    continue; // Skip hidden files if not showing hidden
                }
                std::string file_path = std::string(dir_path[j].c_str()) + "/" + entry->d_name;
                if (stat(file_path.c_str(), &file_obj) == -1) 
                {
                    std::cerr << "Error getting file information for " << entry->d_name << std::endl;
                    continue;
                }
                if (details) 
                {
                std::cout<< (S_ISDIR(file_obj.st_mode) ? "d" : "-")
                <<(file_obj.st_mode & S_IRUSR ? "r" : "-")<< (file_obj.st_mode & S_IWUSR ? "w" : "-")<< (file_obj.st_mode & S_IXUSR ? "x" : "-")
                << (file_obj.st_mode & S_IRGRP ? "r" : "-")<< (file_obj.st_mode & S_IWGRP ? "w" : "-")<< (file_obj.st_mode & S_IXGRP ? "x" : "-")
                << (file_obj.st_mode & S_IROTH ? "r" : "-")<< (file_obj.st_mode & S_IWOTH ? "w" : "-")<< (file_obj.st_mode & S_IXOTH ? "x" : "-")
                << " " << setw(4)<<right<<file_obj.st_nlink<< " " 
                << getpwuid(file_obj.st_uid)->pw_name<< " " 
                << getgrgid(file_obj.st_gid)->gr_name<< " "<<std::setw(6)<<std::right 
                << file_obj.st_size<<" "<<std::setw(25)<<std::right<<entry->d_name<<" "<<std::setw(28)<<std::right<<std::asctime(std::localtime(&file_obj.st_mtime));
                }     
                else 
                {
                    std::cout<<entry->d_name<<"\t"; 
                }
            }
            closedir(dir);
            std::cout<<endl;
        }
        else
        {
            if(S_ISREG(file_obj.st_mode))
            {
                if(details)
                {
                    std::cout<< (S_ISREG(file_obj.st_mode)? "-" : "d")
                    <<(file_obj.st_mode & S_IRUSR? "r" : "-")<< (file_obj.st_mode & S_IWUSR? "w" : "-")<< (file_obj.st_mode & S_IXUSR? "x" : "-")
                    << (file_obj.st_mode & S_IRGRP? "r" : "-")<< (file_obj.st_mode & S_IWGRP? "w" : "-")<< (file_obj.st_mode & S_IXGRP? "x" : "-")
                    << (file_obj.st_mode & S_IROTH? "r" : "-")<< (file_obj.st_mode & S_IWOTH? "w" : "-")<< (file_obj.st_mode & S_IXOTH ? "x" : "-")
                    << " " << file_obj.st_nlink<< " " 
                    << getpwuid(file_obj.st_uid)->pw_name<< " " 
                    << getgrgid(file_obj.st_gid)->gr_name<< " "<<std::setw(6)<<std::right 
                    << file_obj.st_size<<" "<<std::setw(15)<<std::right<<dir_path[j]<<" "<<std::setw(28)<<std::right<<std::asctime(std::localtime(&file_obj.st_mtime));
                }
                else
                {
                    std::cout<<dir_path[j]<<endl;
                }
            }
            else
            {
                cout<<"Error Invalid File/Directory Name or Path"<<endl;
            }
        }
        j++;
    }
}
