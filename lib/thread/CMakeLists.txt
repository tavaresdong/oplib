set(libop_thread_SRCS
  Mutex.cc
  Thread.cc
  Condition.cc
  CountdownLatch.cc
  Channel.h
  Singleton.h
)

# Declare the library
add_library(libop_thread STATIC
    ${libop_thread_SRCS}
)

target_link_libraries(libop_thread pthread)

set_target_properties(libop_thread PROPERTIES LINKER_LANGUAGE CXX)

# Specify here the include directories exported
# by this library
target_include_directories(libop_thread PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
