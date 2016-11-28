file(GLOB SRCS *.C)

ADD_EXECUTABLE(testds ${SRCS})

TARGET_LINK_LIBRARIES(testds
    libop
    libgtest
    libgmock
)

add_test(NAME testds
         COMMAND testds)
