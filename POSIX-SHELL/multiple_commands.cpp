#include <iostream>
#include <string>
#include <sstream>
#include <vector>
using namespace std;
std::vector<std::string> command(string input)
{
    std::vector<std::string> commands;
    char* token = strtok(&input[0], ";");
    std::vector<std::string> tokens;
    while (token != nullptr) {
        commands.push_back(token);
        token = strtok(nullptr, ";");
    }
    return commands;
}

