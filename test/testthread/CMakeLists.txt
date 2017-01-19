file(GLOB NONRECUR test_NonrecursiveLock.cc)
file(GLOB SINGLETON test_Singleton.cc)
file(GLOB THREAD test_Thread.cc)
file(GLOB LATCHQUEUE test_LatchAndQueue.cc)
file(GLOB TIMESTAMP test_Timestamp.cc)

ADD_EXECUTABLE(testNonrecur ${NONRECUR})
ADD_EXECUTABLE(testSingleton ${SINGLETON})
ADD_EXECUTABLE(testThread ${THREAD})
ADD_EXECUTABLE(testLatchAndQueue ${LATCHQUEUE})
ADD_EXECUTABLE(testTimestamp ${TIMESTAMP})



TARGET_LINK_LIBRARIES(testNonrecur
    libop_thread
)

TARGET_LINK_LIBRARIES(testSingleton
    libop_thread
)

TARGET_LINK_LIBRARIES(testThread
    libop_thread
)

TARGET_LINK_LIBRARIES(testLatchAndQueue
    libop_thread
)

TARGET_LINK_LIBRARIES(testTimestamp
    libop_thread
)
