set(libop_net_SRCS
  Mutex.C
  Thread.H
)

# Declare the library
add_library(libop_net STATIC
    ${libop_net_SRCS}
)

set_target_properties(libop_net PROPERTIES LINKER_LANGUAGE CXX)

# Specify here the include directories exported
# by this library
target_include_directories(libop_net PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
