cmake_minimum_required(VERSION 3.30)
project(engine VERSION 1.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_library(${PROJECT_NAME} SHARED
        src/library.cpp
        src/main.cpp
        src/chess/board.h
        src/chess/board.cpp
        src/chess/piece.cpp
        src/chess/piece.h
)

target_include_directories(${PROJECT_NAME} PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        ###
        # For Testing Purposes only | TODO REMOVE LATER
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        ###
)

include(FetchContent)
FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG v1.x  # Use the latest version from the 1.x branch
)
FetchContent_MakeAvailable(spdlog)
target_link_libraries(${PROJECT_NAME} PRIVATE spdlog::spdlog)

###
# Test executable target | TODO REMOVE LATER
add_executable(${PROJECT_NAME}_test src/main.cpp)
target_link_libraries(${PROJECT_NAME}_test PRIVATE ${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME}_test PRIVATE spdlog::spdlog)
###

install(TARGETS ${PROJECT_NAME}
        LIBRARY DESTINATION lib
        PUBLIC_HEADER DESTINATION include
)
