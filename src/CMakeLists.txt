set(fakebot_SRCS main.cpp)

add_executable(fakebot main.cpp FakeBot.cpp getWorld.cpp)
include_directories(${PROJECT_SOURCE_DIR}/include)
target_link_libraries(fakebot ${YARP_LIBRARIES})

install(TARGETS fakebot DESTINATION bin)
