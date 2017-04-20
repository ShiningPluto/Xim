//
// Created by ximu on 4/17/17.
//

#include <iostream>

void showHelpMessage()
{
    std::cout << "Compiler" << std::endl;
}

int main(int argc, char** argv)
{
    if (argc==1)
    {
        showHelpMessage();
        return 0;
    }
    std::cout << argv[1];
    return 0;
}