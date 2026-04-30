#pragma once

#include <tuple>
#include <string>

std::tuple<int, std::string, std::string> secure_execute(const std::string &cmd, int timeout);