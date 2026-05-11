#pragma once

#include "utils/parser_constants.hpp"


#include <fstream>
#include <sstream>
#include <string>

namespace ParserHelper
{

    template <typename T>
    T GetValueByKey(const std::string &filter, const std::string &filename)
    {
        std::string line, key;
        T value{};

        std::ifstream filestream{ParserConstants::ProcDirectory + filename};
        if (filestream.is_open())
        {
            while (std::getline(filestream, line))
            {
                std::istringstream linestream{line};
                if (linestream >> key >> value)
                {
                    if (key == filter)
                    {
                        return value;
                    }
                }
            }
        }
        return value;
    }

    template <typename T>
    T GetValue(const std::string &filename)
    {
        std::string line;
        T value{};

        std::ifstream filestream{ParserConstants::ProcDirectory + filename};
        if (filestream.is_open())
        {
            std::getline(filestream, line);
            std::istringstream linestream{line};
            linestream >> value;
        }
        return value;
    }
}
