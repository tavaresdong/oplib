set(libop_SRCS
    alloc.H
    traits.H
    memutil.H
    vector.H
    linkedlist.H
)

# Declare the library
add_library(libop STATIC
    ${libop_SRCS}
)

set_target_properties(libop PROPERTIES LINKER_LANGUAGE CXX)

# Specify here the include directories exported
# by this library
target_include_directories(libop PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
