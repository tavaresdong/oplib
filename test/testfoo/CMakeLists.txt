file(GLOB SRCS *.cpp)

ADD_EXECUTABLE(testfoo ${SRCS})

TARGET_LINK_LIBRARIES(testfoo
    libalgo
    libgtest
    libgmock
)

add_test(NAME testfoo
         COMMAND testfoo)
