add_rules("mode.debug", "mode.release")
add_requires("boost", "nlohmann_json", "protobuf", "grpc", {system = true}) -- 如果全局安装了grpc，就指定{system = true}
set_targetdir("./bin")

before_build(function (target)
  os.execv("protoc -I $(projectdir)/src/protos --grpc_out=$(projectdir)/src/protos --plugin=protoc-gen-grpc=/usr/bin/grpc_cpp_plugin $(projectdir)/src/protos/online_shopping.proto")
end)

target("client")
    set_kind("binary")
    add_packages("protobuf", "grpc", "nlohmann_json") 
    add_files("src/client/*.cc")
    add_files("src/protos/**.proto", {rules = "protobuf.cpp", proto_rootdir = "src/protos"})
    add_files("src/protos/**.cc")
    add_includedirs("src/protos")
    add_links("grpc++_unsecure")
    add_links("grpc_unsecure")
    

target("server")
    set_kind("binary")
    add_packages("protobuf", "grpc", "nlohmann_json") 
    add_files("src/server/*.cc")
    add_files("src/protos/**.proto", {rules = "protobuf.cpp", proto_rootdir = "src/protos"})
    add_files("src/protos/**.cc")
    add_includedirs("src/protos")
    add_links("grpc++_unsecure")
    add_links("grpc_unsecure")

after_build(function (target)
    import("core.project.task")
    task.run("project", {kind = "compile_commands", outputdir = "."})
end)