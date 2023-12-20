add_library(cemu_libretro SHARED
    libretro.cpp
)

set(LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}")
set_target_properties(cemu_libretro PROPERTIES PREFIX "")

find_package(Git)
if(GIT_FOUND)
    execute_process(WORKING_DIRECTORY ${PROJECT_SOURCE_DIR} COMMAND ${GIT_EXECUTABLE} describe --tags
        OUTPUT_VARIABLE CEMU_WC_DESCRIBE
        OUTPUT_STRIP_TRAILING_WHITESPACE)

    string(REGEX REPLACE "^v?[0-9]+\\.[0-9]+-([0-9]+)-.+" "\\1" CEMU_VERSION_MINOR "${CEMU_WC_DESCRIBE}")
    if (${CEMU_VERSION_MINOR} MATCHES "^[0-9]+$")
        add_definitions(-DEMULATOR_VERSION_MINOR=${CEMU_VERSION_MINOR})
    endif()
endif()

include_directories(..)
