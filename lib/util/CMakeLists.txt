set(libop_util_SRCS_
    Common.h
    Timestamp.h
)

# Declare the library
add_library(libop_util STATIC
    ${libop_util_SRCS}
)

set_target_properties(libop_util PROPERTIES LINKER_LANGUAGE CXX)

# Specify here the include directories exported
# by this library
target_include_directories(libop_util PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
