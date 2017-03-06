set(libop_net_SRCS
    EventDispatcher.cc
    Poller.cc
    EventLoop.cc
    EventLoopThread.cc
    TimerManager.cc
)

# Declare the library
add_library(libop_net STATIC
    ${libop_net_SRCS}
)

target_link_libraries(libop_net pthread)

# Net lib depends on thread lib and util
target_link_libraries(libop_net libop_thread)
target_link_libraries(libop_net libop_util)

set_target_properties(libop_net PROPERTIES LINKER_LANGUAGE CXX)

# Specify here the include directories exported
# by this library
target_include_directories(libop_net PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
