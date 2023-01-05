cmake_minimum_required(VERSION 3.25)
project(shopping_system)

# Find protobuf, grpc
list(APPEND CMAKE_PREFIX_PATH "/third_party/protobuf" "/third_party/grpc")
find_package(Protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)

# Project settings
set(CMAKE_GENERATOR "Ninja")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE "Debug")
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb")

# Server
file(GLOB server_src CONFIGURE_DEPENDS "src/server/*.cc")
add_executable(server ${server_src})

# Client
file(GLOB client_src CONFIGURE_DEPENDS "src/client/*.cc")
add_executable(client ${client_src})

list(APPEND exe_targets server client)

# Proto files
file(GLOB protos "src/protos/*.proto")
get_target_property(grpc_cpp_plugin_location gRPC::grpc_cpp_plugin LOCATION)

foreach(exe_target ${exe_targets})
    target_link_libraries(${exe_target} PRIVATE protobuf::libprotobuf gRPC::grpc gRPC::grpc++ boost_filesystem boost_system)
    target_include_directories(${exe_target} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
    protobuf_generate(
        TARGET ${exe_target}
        LANGUAGE cpp
        PROTOS ${protos})
    protobuf_generate(
        TARGET ${exe_target}
        LANGUAGE grpc
        PROTOS ${protos}
        GENERATE_EXTENSIONS .grpc.pb.h .grpc.pb.cc
        PLUGIN "protoc-gen-grpc=${grpc_cpp_plugin_location}")
endforeach()