# CMakeLists.txt for SQLite
#
# © 2020 by Richard Walters

cmake_minimum_required(VERSION 3.8)
set(This SQLite)

# This option enables some little playground apps for
# experimenting with SQLite.
option(SQLITE_INCLUDE_EXAMPLES "SQLite: Include example playground apps" OFF)

# This option enables the JSON1 extension, along with the examples
# which require it.
option(SQLITE_INCLUDE_JSON1 "SQLite: Include JSON1 extension" OFF)

# This option enables the SQLITE_ENABLE_DESERIALIZE compile-time symbol which
# will pull in the implementation of the sqlite3_serialize() and
# sqlite3_deserialize() interfaces.
option(SQLITE_INCLUDE_SERIALIZATION "SQLite: Define SQLITE_ENABLE_DESERIALIZE to enable serialization" OFF)

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

if(SQLITE_INCLUDE_JSON1)
    target_compile_definitions(${This} PRIVATE SQLITE_ENABLE_JSON1)
endif(SQLITE_INCLUDE_JSON1)

if(SQLITE_INCLUDE_SERIALIZATION)
    target_compile_definitions(${This} PRIVATE SQLITE_ENABLE_DESERIALIZE)
endif(SQLITE_INCLUDE_SERIALIZATION)

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

if(UNIX)
    target_link_libraries(${This} PUBLIC
        pthread
        dl
    )
endif(UNIX)

#############################################################################
# Here are some little playground apps for experimenting with SQLite
#############################################################################

if(SQLITE_INCLUDE_EXAMPLES)
    add_subdirectory(examples/play1)
    add_subdirectory(examples/play2)
    add_subdirectory(examples/play3)
    if(SQLITE_INCLUDE_JSON1)
        add_subdirectory(examples/play4)
    endif(SQLITE_INCLUDE_JSON1)
endif(SQLITE_INCLUDE_EXAMPLES)
