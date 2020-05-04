# CMakeLists.txt for SQLite
#
# © 2020 by Richard Walters

cmake_minimum_required(VERSION 3.8)
set(This SQLite)

# Headers are common to the library and its shell program.

set(Headers
    sqlite3.h
)

# This option enables the JSON1 extension, along with the examples
# which require it.
option(SQLITE_INCLUDE_JSON1 "Include JSON1 extension" OFF)

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

if(SQLITE_INCLUDE_JSON1)
    target_compile_definitions(${This} PRIVATE SQLITE_ENABLE_JSON1)
endif(SQLITE_INCLUDE_JSON1)

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
# Here are some little playground apps for experimenting with SQLite
#############################################################################

add_subdirectory(examples/play1)
add_subdirectory(examples/play2)
add_subdirectory(examples/play3)
if(SQLITE_INCLUDE_JSON1)
    add_subdirectory(examples/play4)
endif(SQLITE_INCLUDE_JSON1)
