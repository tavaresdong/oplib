file(GLOB eventloop_t1 test_eventloop1.cc)
file(GLOB eventloop_t2 test_eventloop2.cc)
file(GLOB eventloop_t3 test_eventloop3.cc)
file(GLOB eventloop_t4 test_eventloop4.cc)
file(GLOB eventloop_t5 test_eventloop5.cc)
file(GLOB eventloop_t6 test_eventloop6.cc)
file(GLOB eventloop_t7 test_eventloop7.cc)
file(GLOB eventloop_t8 test_eventloop8.cc)
file(GLOB eventloop_t9 test_eventloop9.cc)
file(GLOB eventloop_t10 test_eventloop10.cc)
file(GLOB timertest test_timer.cc)
file(GLOB listenertest test_listener.cc)
file(GLOB tcpservertest test_tcpserver.cc)
file(GLOB sigpipetest test_sigpipe.cc)

ADD_EXECUTABLE(testeventLoop1 ${eventloop_t1})
ADD_EXECUTABLE(testeventLoop2 ${eventloop_t2})
ADD_EXECUTABLE(testeventLoop3 ${eventloop_t3})
ADD_EXECUTABLE(testeventLoop4 ${eventloop_t4})
ADD_EXECUTABLE(testeventLoop5 ${eventloop_t5})
ADD_EXECUTABLE(testeventLoop6 ${eventloop_t6})
ADD_EXECUTABLE(testeventLoop7 ${eventloop_t7})
ADD_EXECUTABLE(testeventLoop8 ${eventloop_t8})
ADD_EXECUTABLE(testeventLoop9 ${eventloop_t9})
ADD_EXECUTABLE(testeventLoop10 ${eventloop_t10})
ADD_EXECUTABLE(timertest ${timertest})
ADD_EXECUTABLE(listenertest ${listenertest})
ADD_EXECUTABLE(tcpservertest ${tcpservertest})
ADD_EXECUTABLE(sigpipetest ${sigpipetest})

TARGET_LINK_LIBRARIES(testeventLoop1
    libop_thread
    libop_net
)

TARGET_LINK_LIBRARIES(testeventLoop2
    libop_thread
    libop_net
)

TARGET_LINK_LIBRARIES(testeventLoop3
    libop_thread
    libop_net
)

TARGET_LINK_LIBRARIES(testeventLoop4
    libop_thread
    libop_net
)

TARGET_LINK_LIBRARIES(testeventLoop5
    libop_thread
    libop_net
)

TARGET_LINK_LIBRARIES(testeventLoop6
    libop_thread
    libop_net
)

TARGET_LINK_LIBRARIES(testeventLoop7
    libop_thread
    libop_net
)

TARGET_LINK_LIBRARIES(testeventLoop8
    libop_thread
    libop_net
)

TARGET_LINK_LIBRARIES(testeventLoop9
    libop_thread
    libop_net
)

TARGET_LINK_LIBRARIES(testeventLoop10
    libop_thread
    libop_net
)

TARGET_LINK_LIBRARIES(timertest
    libop_thread
    libop_net
)

TARGET_LINK_LIBRARIES(listenertest
    libop_thread
    libop_net
)

TARGET_LINK_LIBRARIES(tcpservertest
    libop_thread
    libop_net
)

TARGET_LINK_LIBRARIES(sigpipetest
    libop_thread
    libop_net
)
