#include <iostream>
#include <vector>
#include <sstream>

std::vector<std::string> my_string_tokenizer(std::string input)
{
    char* temp = strtok(&input[0], " \t");
    std::vector<std::string> tokens;
    while (temp != nullptr) 
    {
        tokens.push_back(temp);
        temp = strtok(nullptr, " \t");
    }
    return tokens;
}
