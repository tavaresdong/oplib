file(GLOB NONRECUR test_NonrecursiveLock.C)
file(GLOB SINGLETON test_Singleton.C)
file(GLOB THREAD test_Thread.C)

ADD_EXECUTABLE(testNonrecur ${NONRECUR})
ADD_EXECUTABLE(testSingleton ${SINGLETON})
ADD_EXECUTABLE(testThread ${THREAD})


TARGET_LINK_LIBRARIES(testNonrecur
    libop_net
)

TARGET_LINK_LIBRARIES(testSingleton
    libop_net
)

TARGET_LINK_LIBRARIES(testThread
    libop_net
)