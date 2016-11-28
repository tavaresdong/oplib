set(libop_SRCS
    foo.h
    foo.cpp
    ibar.h
    alloc.H
    traits.H
    memutil.H
    vector.H
)

# Declare the library
add_library(libop STATIC
    ${libop_SRCS}
)

# Specify here the include directories exported
# by this library
target_include_directories(libop PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
