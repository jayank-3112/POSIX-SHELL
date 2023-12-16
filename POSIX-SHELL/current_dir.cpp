#include <string>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include "current_dir.h"
using namespace std;
std::string get_current_dir()
{
    char current_directory[256];
    if (getcwd(current_directory, sizeof(current_directory)) != nullptr) {
        //std::cout << "Current directory: " << current_directory << std::endl;
        return current_directory;
    } 
    else 
    {
        return "error getting directory.";
    }
}
