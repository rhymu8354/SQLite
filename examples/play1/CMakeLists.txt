# CMakeLists.txt for SQLite playground example #1
#
# SQLPlay1 -- a small playground application which demonstrates using
# SQLite to fetch a value from a simple key-value table.
#
# © 2020 by Richard Walters

cmake_minimum_required(VERSION 3.8)
set (This SQLPlay1)

set (Sources
    main.cpp
)

add_executable(${This} ${Sources})
set_target_properties(${This} PROPERTIES
    FOLDER Applications
)

target_link_libraries(${This} PUBLIC
    SQLite
)

add_custom_command(TARGET ${This} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_PROPERTY:${This},SOURCE_DIR>/test.db $<TARGET_FILE_DIR:${This}>/test.db
)
