cmake_minimum_required(VERSION 3.5)

project (lvt)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(DEMO_PROTO lvt)

get_filename_component(hw_proto "${CMAKE_CURRENT_SOURCE_DIR}/resources/protos/${DEMO_PROTO}.proto" ABSOLUTE)
get_filename_component(hw_proto_path "${hw_proto}" PATH)

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/protos)

find_program(GRPC_CPP_PLUGIN_BIN grpc_cpp_plugin)

# Generated sources
set(hw_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/protos/${DEMO_PROTO}.pb.cc")
set(hw_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/protos/${DEMO_PROTO}.pb.h")
set(hw_grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/protos/${DEMO_PROTO}.grpc.pb.cc")
set(hw_grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/protos/${DEMO_PROTO}.grpc.pb.h")
add_custom_command(
      OUTPUT "${hw_proto_srcs}" "${hw_proto_hdrs}" "${hw_grpc_srcs}" "${hw_grpc_hdrs}"
      COMMAND "protoc"
      ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}/protos"
        --cpp_out "${CMAKE_CURRENT_BINARY_DIR}/protos"
        -I "${hw_proto_path}"
        --plugin=protoc-gen-grpc="${GRPC_CPP_PLUGIN_BIN}"
        "${hw_proto}"
      DEPENDS "${hw_proto}")
add_library(protos ${hw_proto_srcs} ${hw_grpc_srcs})
include_directories("${CMAKE_CURRENT_BINARY_DIR}/protos")



file(GLOB_RECURSE DEMO_PROJECT_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

foreach(DEMO_SRC_FILE ${DEMO_PROJECT_SOURCE})
    if("${DEMO_SRC_FILE}" MATCHES "(.*)/src/main.cpp")
        list(REMOVE_ITEM DEMO_PROJECT_SOURCE ${DEMO_SRC_FILE})
    endif()
endforeach(DEMO_SRC_FILE)

include_directories(${lvt_SOURCE_DIR}/src)

add_library(lvt_lib STATIC ${DEMO_PROJECT_SOURCE})
target_link_libraries(lvt_lib protos)

add_executable(lvt_server ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)
target_link_libraries(lvt_server lvt_lib pistache grpc++_unsecure protobuf grpc_unsecure pqxx pq)

add_executable(http_pistache_test tests/http_pistache_test.cpp)
target_link_libraries(http_pistache_test lvt_lib gtest gtest_main pistache)

add_executable(grpc_test tests/grpc_test.cpp)
target_link_libraries(grpc_test lvt_lib gtest gtest_main grpc++_unsecure protobuf grpc_unsecure)

add_executable(postgresql_test tests/postgresql_test.cpp)
target_link_libraries(postgresql_test lvt_lib gtest gtest_main grpc++_unsecure protobuf grpc_unsecure pqxx pq)
