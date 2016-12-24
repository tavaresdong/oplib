file(GLOB SRCS *.C)

ADD_EXECUTABLE(testds ${SRCS})

TARGET_LINK_LIBRARIES(testds
    libop_ds
    libgtest
    libgmock
)

add_test(NAME testds
         COMMAND testds)
