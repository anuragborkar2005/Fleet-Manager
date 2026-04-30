#include "utils/config.hpp"

#include <fstream>

json load_config_file(const std::string &filename)
{
    std::ifstream config_file{"config.json"};
    if (!config_file.is_open())
    {
        return json::object();
    }

    try
    {
        json cfg = json::parse(config_file);
        return cfg;
    }
    catch (const json::parse_error &e)
    {
        return json::object();
    }
}
