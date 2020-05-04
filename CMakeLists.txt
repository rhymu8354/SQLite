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

# ---------------------------------------------------------------------------
# SQLPlay3: inserting, updating, and deleting rows
# ---------------------------------------------------------------------------

set (Playground3 SQLPlay3)

set (PlaygroundSources3
    examples/play3/main.cpp
)

add_executable(${Playground3} ${PlaygroundSources3})
set_target_properties(${Playground3} PROPERTIES
    FOLDER Applications
)

target_link_libraries(${Playground3} PUBLIC
    ${This}
)

add_custom_command(TARGET ${Playground3} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_PROPERTY:${This},SOURCE_DIR>/examples/play3/test.db $<TARGET_FILE_DIR:${Playground3}>/play3.db
)

if(SQLITE_INCLUDE_JSON1)
    # ---------------------------------------------------------------------------
    # SQLPlay4: examples using the JSON1 extension
    # ---------------------------------------------------------------------------

    set (Playground4 SQLPlay4)

    set (PlaygroundSources4
        examples/play4/main.cpp
    )

    add_executable(${Playground4} ${PlaygroundSources4})
    set_target_properties(${Playground4} PROPERTIES
        FOLDER Applications
    )

    target_link_libraries(${Playground4} PUBLIC
        ${This}
    )

    add_custom_command(TARGET ${Playground4} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_PROPERTY:${This},SOURCE_DIR>/examples/play4/test.db $<TARGET_FILE_DIR:${Playground4}>/play4.db
    )
endif(SQLITE_INCLUDE_JSON1)
