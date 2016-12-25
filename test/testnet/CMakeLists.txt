file(GLOB NONRECUR test_NonrecursiveLock.C)

ADD_EXECUTABLE(testNonrecur ${NONRECUR})

TARGET_LINK_LIBRARIES(testNonrecur
    libop_net
)
