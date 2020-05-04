# CMakeLists.txt for SQLite
#
# © 2020 by Richard Walters

cmake_minimum_required(VERSION 3.8)
set(This SQLite)

# Headers are common to the library and its shell program.

set(Headers
    sqlite3.h
)

#############################################################################
# SQLite itself is a library we embed in programs.
#############################################################################
set(LibrarySources
    sqlite3.c
)

add_library(${This} STATIC ${LibrarySources} ${Headers})
set_target_properties(${This} PROPERTIES
    FOLDER Libraries
)

target_include_directories(${This} PUBLIC .)

#############################################################################
# SQLite comes with a command-line shell program.
#############################################################################

set (Shell sqlite3)

set(ShellSources
    shell.c
)

add_executable(${Shell} ${ShellSources})
set_target_properties(${Shell} PROPERTIES
    FOLDER Applications
)

target_link_libraries(${Shell} PUBLIC
    ${This}
)

#############################################################################
# Here is a little playground app for experimenting with SQLite
#############################################################################

set (Playground SQLPlay)

set (PlaygroundSources
    examples/play.cpp
)

add_executable(${Playground} ${PlaygroundSources})
set_target_properties(${Playground} PROPERTIES
    FOLDER Applications
)

target_link_libraries(${Playground} PUBLIC
    ${This}
)
