cmake_minimum_required(VERSION 3.7)

project(miniclip-dynamodb)

find_package(AWSSDK REQUIRED)

find_package(cpprestsdk REQUIRED)

find_package(Protobuf REQUIRED)
include_directories(${PROTOBUF_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS model/payload.proto)

include_directories("./controller/foundation/include")

add_executable(${PROJECT_NAME} ./main.cpp 
							   ./util/pbjson.cpp
                               ./controller/rest_controller.cpp
                               ./controller/foundation/network_utils.cpp
                               ./controller/foundation/basic_controller.cpp
                               ./service/KvService.cpp
							   ./service/KvServiceMock.cpp
                               ${PROTO_SRCS}
                               ${PROTO_HDRS})
								
target_link_libraries(${PROJECT_NAME} aws-cpp-sdk-core aws-cpp-sdk-kms aws-cpp-sdk-dynamodb cpprestsdk::cpprest ${PROTOBUF_LIBRARIES} boost_system)
