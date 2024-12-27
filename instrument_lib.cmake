
set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

# Get JSON library for inspections
include(FetchContent)
FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz DOWNLOAD_EXTRACT_TIMESTAMP TRUE)
FetchContent_MakeAvailable(json)

set (INSTRUMENT_DIR ${CMAKE_CURRENT_LIST_DIR})

file (GLOB_RECURSE INSTRUMENT_LIB_SRCS 
            ${INSTRUMENT_DIR}/src/*.c 
            ${INSTRUMENT_DIR}/src/*.cpp 
            ${INSTRUMENT_DIR}/api/*.c
            ${INSTRUMENT_DIR}/api/*.cpp)

add_library (wasminstrument ${INSTRUMENT_LIB_SRCS})
target_include_directories (wasminstrument PRIVATE ${INSTRUMENT_DIR}/inc ${INSTRUMENT_DIR})
target_link_libraries(wasminstrument PRIVATE nlohmann_json::nlohmann_json)

set (INSTRUMENT_EXPORT_DIR ${INSTRUMENT_DIR}/api/export)

install (TARGETS wasminstrument DESTINATION .)
