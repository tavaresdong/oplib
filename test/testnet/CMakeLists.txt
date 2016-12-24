file(GLOB SRCS *.C)

ADD_EXECUTABLE(testnet ${SRCS})

TARGET_LINK_LIBRARIES(testnet
    libop_net
    libgtest
    libgmock
)
