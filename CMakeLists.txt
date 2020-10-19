cmake_minimum_required(VERSION 3.15)
project(maze)
set(CMAKE_CXX_STANDARD 14)


find_path(INCLUDE_DIR NAMES SDL2/SDL.h)
message("FOUND SDL2 include dir = ${INCLUDE_DIR}")

include_directories(${INCLUDE_DIR}/SDL2)
include_directories(/Library/Frameworks/SDL2.framework/Headers)
include_directories(/Library/Frameworks/SDL2_image.framework/Headers)
include_directories(/Library/Frameworks/SDL2_ttf.framework/Headers)



# include_directories(/usr/include/SDL2)

find_library(mingw32_LIB mingw32 HINT C:/mingw-w64/mingw64/x86_64-w64-mingw32/lib)
find_library(SDL2_LIB SDL2 )
find_library(SDL2MAIN_LIB SDL2main)
find_library(SDL2_TTF_LIB SDL2_ttf)
find_library(SDL2_IMG_LIB SDL2_image)


message("FOUND mingw32 LIB = ${mingw32_LIB}")
message("FOUND SDL2 LIB = ${SDL2_LIB}")
message("FOUND SDL2 MAIN LIB = ${SDL2MAIN_LIB}")
message("FOUND SDL2 TFF LIB = ${SDL2_TTF_LIB}")
message("FOUND SDL2 IMAGE LIB = ${SDL2_IMG_LIB}")


add_executable(maze main.cpp LTexture.cpp LTexture.h)

if( mingw32_LIB)
  target_link_libraries(maze PRIVATE "${mingw32_LIB}")
  target_link_libraries(maze PRIVATE "${SDA2MAIN_LIB}")
endif()
target_link_libraries(maze PRIVATE "${SDL2_LIB}")
target_link_libraries(maze PRIVATE "${SDL2_TTF_LIB}")
target_link_libraries(maze PRIVATE "${SDL2_IMG_LIB}")

