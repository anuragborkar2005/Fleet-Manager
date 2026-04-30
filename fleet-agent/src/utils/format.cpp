#include "utils/format.hpp"

#include <iomanip>
#include <sstream>

std::string Format::Format(int time)
{
    return std::string(2 - std::to_string(time).length(), '0') + std::to_string(time);
}
std::string Format::ElapsedTime(long seconds)
{
    int hours = 0;
    int mins = 0;
    int secs = 0;

    hours = seconds / (60 * 60);
    seconds = seconds % (60 * 60);
    mins = seconds / 60;
    seconds = seconds % 60;
    secs = seconds;

    return Format(hours) + ":" + Format(mins) + ":" + Format(secs);
}
std::string Format::KBtoMB(float kb)
{
    float mb = kb / 1024;
    std::stringstream mb_stream;
    mb_stream << std::fixed << std::setprecision(1) << mb;
    return mb_stream.str();
}
