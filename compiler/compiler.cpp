//
// Created by ximu on 4/17/17.
//

#include "Parser.h"

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

    Parser parser(argv[1]);
    parser.run();

    return 0;
}