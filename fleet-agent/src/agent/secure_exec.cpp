#include "secure_exec.hpp"

#include <array>
#include <chrono>
#include <future>
#include <memory>

std::tuple<int, std::string, std::string> secure_execute(const std::string &cmd, int timeout)
{
    auto exec = [cmd]() -> std::tuple<int, std::string, std::string>
    {
        std::array<char, 128> buffer;
        std::string stdout_str, stderr_str;

        std::unique_ptr<FILE> pipe = std::make_unique<FILE>(popen(cmd.c_str(), "r"));
        if (!pipe)
        {
            return {-1, "", "Popen Failed"};
        }
        while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
        {
            stdout_str += buffer.data();
        }
        int exit_code = pclose(pipe.get());

        return {WEXITSTATUS(exit_code), stdout_str, stderr_str};
    };

    auto future = std::async(std::launch::async, exec);
    if (future.wait_for(std::chrono::seconds(timeout)) ==
        std::future_status::timeout)
    {
        return {-2, "", "Command timed out"};
    }
    return future.get();
}