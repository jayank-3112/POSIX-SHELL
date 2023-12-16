#include <iostream>
#include <cstring>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include "current_dir.h"
using namespace std;
bool search_file_folder_recursive(const char* dir_path, const char* name, bool folder, bool file) 
{
    DIR* dir = opendir(dir_path);
    if (dir == nullptr) 
    {
        cout<<"Error opening directory: "<<strerror(errno)<<endl;
        return false;
    }
    dirent* entry;
    while ((entry = readdir(dir)) != nullptr) 
    {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) 
        {
            continue; // parent and current skipped
        }
        string full_path = std::string(dir_path) + "/" + entry->d_name;
        struct stat fileInfo;
        if (stat(full_path.c_str(), &fileInfo) == 0) 
        {
            if (S_ISDIR(fileInfo.st_mode) && folder && strcmp(entry->d_name, name) == 0) 
            {
                closedir(dir);
                return true; 
            }

            if (S_ISDIR(fileInfo.st_mode)) 
            {
                if (search_file_folder_recursive(full_path.c_str(), name, folder, file)) {
                    closedir(dir);              //close dir
                    return true;   
                }
            }

            if (S_ISREG(fileInfo.st_mode) && file && strcmp(entry->d_name, name) == 0) 
            {
                closedir(dir); // close dir
                return true;   // return true if file found
            }
        }
    }
    closedir(dir); // close dir
    return false;  // not found
}
int search(vector<string> temp_arg) 
{ 
    if(temp_arg.size()==1)
    {
        cout<<"Error Too Few Parameters - Give file name \n";
        return 0; 
    }
    string name=temp_arg[1];
    bool folder = true;              
    bool file = true;                
    string directory=get_current_dir();
    if (search_file_folder_recursive(directory.c_str(), name.c_str(), folder, file)) 
    {
        return true;
    } 
    else 
    {
        return false;
    }
}
