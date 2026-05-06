cmake_minimum_required(VERSION 3.22)
project(fleet-backend LANGUAGES CXX)

# Enable vcpkg integration automatically
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    if(DEFINED ENV{VCPKG_ROOT})
        set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
            CACHE STRING "")
    else()
        message(FATAL_ERROR "VCPKG_ROOT environment variable not set. Please set it to your vcpkg installation path.")
    endif()
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(BUILD_SHARED_LIBS OFF)

include_directories(include)
include_directories(src)

find_package(fmt CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(Crow CONFIG REQUIRED)
find_package(libssh CONFIG REQUIRED)
find_package(CURL CONFIG REQUIRED)
find_package(SQLiteCpp CONFIG REQUIRED)
find_package(jwt-cpp CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)


file(GLOB_RECURSE HEADER_FILES "include/*hpp")
file(GLOB_RECURSE CPP_SOURCES "src/*.cpp")



add_executable(${PROJECT_NAME}
    ${CPP_SOURCES}
    ${HEADER_FILES}
)

target_link_directories(${PROJECT_NAME} PRIVATE include src)

target_link_libraries(${PROJECT_NAME} PRIVATE
    fmt::fmt
    spdlog::spdlog
    Crow::Crow
    SQLiteCpp
    ssh
    jwt-cpp::jwt-cpp
    nlohmann_json::nlohmann_json
    CURL::libcurl
)
