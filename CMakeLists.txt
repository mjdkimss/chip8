cmake_minimum_required(VERSION 3.16)
project(chip8 C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# ---------------------------------------------------------------
# SDL2: 공식 릴리스 소스를 내려받아 정적 라이브러리로 함께 빌드
# (처음 configure 할 때 한 번만 다운로드됨)
# ---------------------------------------------------------------
include(FetchContent)
set(SDL_SHARED OFF CACHE BOOL "" FORCE)
set(SDL_STATIC ON  CACHE BOOL "" FORCE)
set(SDL_TEST   OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
    SDL2
    URL https://github.com/libsdl-org/SDL/releases/download/release-2.30.9/SDL2-2.30.9.tar.gz
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(SDL2)

# ---------------------------------------------------------------
# 에뮬레이터 실행 파일
# ---------------------------------------------------------------
add_executable(chip8
    src/main.c
	src/chip8.c
    src/chip8.h
)

set_target_properties(chip8 PROPERTIES
	VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
	VS_DEBUGGER_COMMAND_ARGUMENTS "\"roms/IBM Logo.ch8\""
)

target_link_libraries(chip8 PRIVATE SDL2::SDL2main SDL2::SDL2-static)

if(MSVC)
    target_compile_options(chip8 PRIVATE /W4 /utf-8)
else()
    target_compile_options(chip8 PRIVATE -Wall -Wextra)
endif()
