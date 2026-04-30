cmake_minimum_required(VERSION 4.2)
project(fleet-agent LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)
file(GLOB_RECURSE SOURCES "src/*.cpp")


# Enable vcpkg integration automatically
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
        CACHE STRING "")
endif()

find_package(fmt CONFIG REQUIRED)
find_package(asio CONFIG REQUIRED)
find_package(Crow CONFIG REQUIRED)
find_package(CURL CONFIG REQUIRED)
find_package(jwt-cpp CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(prometheus-cpp CONFIG REQUIRED)
find_package(libssh CONFIG REQUIRED)


add_executable(${PROJECT_NAME}
    ${SOURCES}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    fmt::fmt
    Crow::Crow
    asio::asio
    CURL::libcurl
    jwt-cpp::jwt-cpp
    nlohmann_json::nlohmann_json
    prometheus-cpp::core
    prometheus-cpp::pull
    prometheus-cpp::util
    ssh
)
