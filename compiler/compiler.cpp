//
// Created by ximu on 4/17/17.
//

#include "Parser.h"

#include <llvm/Support/CommandLine.h>

#include <iostream>

void printVersion()
{
    std::cout << "Version: 0.0.1\n";
}

static llvm::cl::opt<std::string> input_filename(llvm::cl::Positional, llvm::cl::desc("<inputs>"));
static llvm::cl::opt<std::string> output_filename("o", llvm::cl::desc("Output filename"), llvm::cl::value_desc("filename"));

int main(int argc, char** argv)
{
    llvm::cl::SetVersionPrinter(printVersion);
    llvm::cl::ParseCommandLineOptions(argc, argv, "Xim Compiler\n");

    if (input_filename == "")
    {
        std::cerr << "Error: You must specify the name of the source file to be compiled.\nUse --help to see the options.\n";
        exit(-1);
    }

    if (output_filename == "")
    {
        output_filename = "a.ll";
    }

    std::cout << input_filename << "\n";
    std::cout << output_filename << "\n\n";

    Parser parser(input_filename.getValue());
    parser.run();
    parser.generate(output_filename);

    return 0;
}