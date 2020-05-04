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
# Here are some little playground apps for experimenting with SQLite
#############################################################################

# ---------------------------------------------------------------------------
# SQLPlay1: extremely simple key-value kind of table
# ---------------------------------------------------------------------------

set (Playground1 SQLPlay1)

set (PlaygroundSources1
    examples/play1/main.cpp
)

add_executable(${Playground1} ${PlaygroundSources1})
set_target_properties(${Playground1} PROPERTIES
    FOLDER Applications
)

target_link_libraries(${Playground1} PUBLIC
    ${This}
)

add_custom_command(TARGET ${Playground1} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_PROPERTY:${This},SOURCE_DIR>/examples/play1/test.db $<TARGET_FILE_DIR:${Playground1}>/play1.db
)

# ---------------------------------------------------------------------------
# SQLPlay2: fetching multiple rows in one query
# ---------------------------------------------------------------------------

set (Playground2 SQLPlay2)

set (PlaygroundSources2
    examples/play2/main.cpp
)

add_executable(${Playground2} ${PlaygroundSources2})
set_target_properties(${Playground2} PROPERTIES
    FOLDER Applications
)

target_link_libraries(${Playground2} PUBLIC
    ${This}
)

add_custom_command(TARGET ${Playground2} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_PROPERTY:${This},SOURCE_DIR>/examples/play2/test.db $<TARGET_FILE_DIR:${Playground2}>/play2.db
)
