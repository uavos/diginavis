project('diginavis', 'cpp',
         version : '0.1',
         default_options : ['warning_level=3', 
                            'cpp_std=c++17',
                            'libdir=',
                            'prefix=~/Документы/UAVOS/Plugins/'])

qt5_dep = dependency('qt5', modules: ['Core',
                                      'Positioning'])

cpp = meson.get_compiler('cpp')
#apx
apx_sdk = '/home/pavel/apx_sdk'
apx_inc = include_directories(join_paths(apx_sdk, 'include/ApxGcs'),
                              join_paths(apx_sdk, 'include/ApxCore'),
                              join_paths(apx_sdk, 'include/ApxData'),
                              join_paths(apx_sdk, 'include/ApxShared'))
apx_dep = [cpp.find_library('ApxCore', dirs: join_paths(apx_sdk, 'lib')),
           cpp.find_library('ApxData', dirs: join_paths(apx_sdk, 'lib')),
           cpp.find_library('ApxGcs', dirs: join_paths(apx_sdk, 'lib')),
           cpp.find_library('ApxShared', dirs: join_paths(apx_sdk, 'lib'))]

#qt
qt5 = import('qt5')
moc_files = qt5.preprocess(moc_headers: ['asyncclient.h',
                                         'diginavisplugin.h',
                                         'diginavis.h',
                                         'flightplan.h'],
                           include_directories: apx_inc,
                           dependencies: qt5_dep)

#grpc && protobuf
proto_sdk = '/home/pavel/grpc_install/'
proto_sdk_bin = join_paths(proto_sdk, 'bin')
proto_sdk_lib = join_paths(proto_sdk, 'lib')
proto_sdk_inc = include_directories(join_paths(proto_sdk, 'include'))
protoc = find_program(join_paths(proto_sdk_bin, 'protoc'))
gen = generator(protoc,
                output: ['@BASENAME@.grpc.pb.cc', '@BASENAME@.grpc.pb.h'],
                arguments: ['--proto_path=@CURRENT_SOURCE_DIR@', 
                            '--grpc_out=@BUILD_DIR@', 
                            '--plugin=protoc-gen-grpc=' + proto_sdk_bin + '/grpc_cpp_plugin',
                            '@INPUT@'])
protogrpc_src = gen.process('Mission.proto', 'Tracking.proto')
gen = generator(protoc,
                output: ['@BASENAME@.pb.cc', '@BASENAME@.pb.h'],
                arguments: ['--proto_path=@CURRENT_SOURCE_DIR@', 
                            '--cpp_out=@BUILD_DIR@', 
                            '@INPUT@'])
proto_src = gen.process('Mission.proto', 'Tracking.proto')
protobuf_dep = cpp.find_library('protobuf', dirs: proto_sdk_lib, static: true)
grpc_dep = [cpp.find_library('grpc++_unsecure', dirs: proto_sdk_lib, static: true),
            cpp.find_library('grpc_unsecure', dirs: proto_sdk_lib, static: true),
            cpp.find_library('gpr', dirs: proto_sdk_lib, static: true)]

#other
thread_dep = dependency('threads')
zlib = dependency('zlib')

shared_library('diginavis',
               'asyncclient.cpp',
               'diginavisplugin.cpp',
               'diginavis.cpp',
               'flightplan.cpp',
                protogrpc_src,
                proto_src,
                moc_files,
                include_directories: [apx_inc,
                                      proto_sdk_inc],
                dependencies: [apx_dep,
                               grpc_dep,
                               protobuf_dep,
                               thread_dep,
                               qt5_dep,
                               zlib],
                install : true)
