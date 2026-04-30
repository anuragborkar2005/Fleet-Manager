#pragma once
#include <string>

bool validate_jwt(const std::string& token, const std::string& secret);
