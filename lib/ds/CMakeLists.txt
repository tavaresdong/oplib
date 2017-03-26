set(libop_ds_SRCS
    alloc.H
    traits.H
    memutil.H
    Vector.H
    Linkedlist.H
    TSTTrie.h
    MinHeap.h
    RBTree.H
)

# Declare the library
add_library(libop_ds STATIC
    ${libop_ds_SRCS}
)

set_target_properties(libop_ds PROPERTIES LINKER_LANGUAGE CXX)

# Specify here the include directories exported
# by this library
target_include_directories(libop_ds PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
