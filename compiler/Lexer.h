//
// Created by ximu on 4/20/17.
//

#pragma once

#include <string>
#include <memory>

class Lexer
{
    std::shared_ptr<std::string> file_path;

public:
    Lexer(std::shared_ptr<std::string> source)
        : file_path(source)
    {

    }

    void run();
};
