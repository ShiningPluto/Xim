//
// Created by ximu on 4/20/17.
//

#pragma once

#include <string>
#include <memory>
#include <fstream>
#include <unordered_set>

class Token;

class Lexer
{
    std::shared_ptr<std::string> file_path;
    std::ifstream input;
    std::unordered_set<std::string> dictionary;

public:
    Lexer(std::shared_ptr<std::string> source);

    ~Lexer();

    Token nextToken();
};
