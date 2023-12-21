add_library(cemu_libretro SHARED
    CemuLibretro.cpp
    CemuLibretro.h
    Environment.cpp
    Environment.h
    Logging.cpp
    Main.cpp
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
        add_compile_definitions(EMULATOR_VERSION_MINOR=${CEMU_VERSION_MINOR})
        target_compile_definitions(CemuCafe PRIVATE EMULATOR_VERSION_MINOR=${CEMU_VERSION_MINOR})
    endif()
endif()

include_directories(..)

target_link_libraries(cemu_libretro PRIVATE CemuConfig)
target_link_libraries(cemu_libretro PRIVATE Boost::headers wx::base)
