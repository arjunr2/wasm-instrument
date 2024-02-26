
set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

set (INSTRUMENT_DIR ${CMAKE_CURRENT_LIST_DIR})

file (GLOB_RECURSE INSTRUMENT_LIB_SRCS 
            ${INSTRUMENT_DIR}/src/*.c 
            ${INSTRUMENT_DIR}/src/*.cpp 
            ${INSTRUMENT_DIR}/api/*.c
            ${INSTRUMENT_DIR}/api/*.cpp)

add_library (instrumentlib ${INSTRUMENT_LIB_SRCS})
target_include_directories (instrumentlib PRIVATE ${INSTRUMENT_DIR}/inc ${INSTRUMENT_DIR})

set (INSTRUMENT_EXPORT_DIR ${INSTRUMENT_DIR}/api/export)
