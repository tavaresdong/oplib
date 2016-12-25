file(GLOB NONRECUR test_NonrecursiveLock.C)
file(GLOB SINGLETON test_Singleton.C)

ADD_EXECUTABLE(testNonrecur ${NONRECUR})
ADD_EXECUTABLE(testSingleton ${SINGLETON})

TARGET_LINK_LIBRARIES(testNonrecur
    libop_net
)

TARGET_LINK_LIBRARIES(testSingleton
    libop_net
)