cmake_minimum_required(VERSION 3.14)
project(ExpenseSplitter VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add cpp-httplib
find_package(httplib REQUIRED)

# Add nlohmann/json
find_package(nlohmann_json REQUIRED)

# Include directories
include_directories(${PROJECT_SOURCE_DIR}/include)

# Source files
set(SOURCES
    src/main.cpp
    src/database/database.cpp
    src/services/balance_service.cpp
    src/services/expense_service.cpp
    src/api/api_server.cpp
)

# Executable
add_executable(expense_splitter ${SOURCES})

# Link libraries
target_link_libraries(expense_splitter
    httplib::httplib
    nlohmann_json::nlohmann_json
)