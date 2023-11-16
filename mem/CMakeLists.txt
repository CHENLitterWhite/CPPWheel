INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/mem)

SET(SRC_LISTS testMem.cpp)
ADD_EXECUTABLE(memT ${SRC_LISTS})
target_link_libraries(memT -pthread)