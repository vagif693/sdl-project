cmake_minimum_required(VERSION 3.10)
project(SDL2_Zombie_Game_Single CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Hint Homebrew paths for Apple Silicon
set(CMAKE_PREFIX_PATH "/opt/homebrew")

# Find SDL2
find_package(SDL2 REQUIRED)
if (SDL2_FOUND)
    message(STATUS "SDL2 found: ${SDL2_LIBRARIES}")
else()
    message(FATAL_ERROR "SDL2 not found!")
endif()

# Find SDL2_image
find_package(PkgConfig)
pkg_check_modules(SDL2_IMAGE SDL2_image)
if (SDL2_IMAGE_FOUND)
    include_directories(${SDL2_IMAGE_INCLUDE_DIRS})
    message(STATUS "SDL2_image found: ${SDL2_IMAGE_LIBRARIES}")
else()
    # Fallback if pkg-config fails
    find_library(SDL2_IMAGE_LIBRARY NAMES SDL2_image HINTS /opt/homebrew/lib)
    find_path(SDL2_IMAGE_INCLUDE_DIR SDL_image.h HINTS /opt/homebrew/include/SDL2)
    if (SDL2_IMAGE_LIBRARY AND SDL2_IMAGE_INCLUDE_DIR)
        include_directories(${SDL2_IMAGE_INCLUDE_DIR})
        message(STATUS "SDL2_image found manually: ${SDL2_IMAGE_LIBRARY}")
    else()
        message(FATAL_ERROR "SDL2_image not found!")
    endif()
endif()

# Add executable
add_executable(${PROJECT_NAME}
    src/main.cpp
)

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2)
if (SDL2_IMAGE_FOUND)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${SDL2_IMAGE_LIBRARIES})
else()
    target_link_libraries(${PROJECT_NAME} PRIVATE ${SDL2_IMAGE_LIBRARY})
endif()

# Ensure the include directories are set for SDL2
target_include_directories(${PROJECT_NAME} PRIVATE /opt/homebrew/include/SDL2)