file(GLOB SRCS *.cc)

# After FIND_PACKAGE is done, some variables are set automatically
FIND_PACKAGE(Boost COMPONENTS system filesystem REQUIRED)

# If boost library is found, build the searchEngine
IF(Boost_FOUND)
  INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})
  ADD_EXECUTABLE(searchEngine ${SRCS})
  TARGET_LINK_LIBRARIES(searchEngine ${Boost_LIBRARIES})
  TARGET_LINK_LIBRARIES(searchEngine libop_ds)
ENDIF()


