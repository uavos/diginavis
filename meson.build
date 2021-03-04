project('diginavis', 'cpp',
         version : '0.1',
         default_options : ['warning_level=2', 
                            'cpp_std=c++17',
                            'libdir=',
                            'prefix=~/Документы/UAVOS/Plugins/'])

qt5_dep = dependency('qt5', modules: ['Core',
                                      'Gui',
                                      'Network',
                                      'Positioning',
                                      'Qml',
                                      'Quick',
                                      'Widgets'])

cpp = meson.get_compiler('cpp')
#apx
apx_sdk_inc = '/home/pavel/sources/apx-gcs/'
apx_inc = include_directories(join_paths(apx_sdk_inc, 'src/lib/ApxGcs'),
                              join_paths(apx_sdk_inc, 'src/lib/ApxCore'),
                              join_paths(apx_sdk_inc, 'src/lib/ApxData'),
                              join_paths(apx_sdk_inc, 'shared'),
                              join_paths(apx_sdk_inc, 'shared/common'),
                              join_paths(apx_sdk_inc, 'shared/crc/include'),
                              '/home/pavel/sources/apx-gcs/build/modules/shared.mandala/gensrc/')
apx_sdk_lib = '/home/pavel/sources/apx-gcs/build/out/'
apx_dep = [cpp.find_library('ApxCore', dirs: join_paths(apx_sdk_lib, 'lib')),
           cpp.find_library('ApxData', dirs: join_paths(apx_sdk_lib, 'lib')),
           cpp.find_library('ApxGcs', dirs: join_paths(apx_sdk_lib, 'lib'))]

#qt
qt5 = import('qt5')
moc_files = qt5.preprocess(moc_headers: ['asyncclient.h',
                                         'authorization.h',
                                         'datetimefact.h',
                                         'diginavisplugin.h',
                                         'diginavis.h',
                                         'dronecreator.h',
                                         'drones.h',
                                         'flightrequestapproved.h',
                                         'flightrequestcreator.h',
                                         'flightrequests.h',
                                         'httpapibase.h',
                                         'telemetry.h'],
                           include_directories: apx_inc,
                           dependencies: qt5_dep)

protoc = find_program('protoc')
gen = generator(protoc,
                output: ['@BASENAME@.grpc.pb.cc', '@BASENAME@.grpc.pb.h'],
                arguments: ['--proto_path=@CURRENT_SOURCE_DIR@', 
                            '--grpc_out=@BUILD_DIR@', 
                            '--plugin=protoc-gen-grpc=/usr/bin/grpc_cpp_plugin',
                            '@INPUT@'])
protogrpc_src = gen.process('Mission.proto', 'Tracking.proto')
gen = generator(protoc,
                output: ['@BASENAME@.pb.cc', '@BASENAME@.pb.h'],
                arguments: ['--proto_path=@CURRENT_SOURCE_DIR@', 
                            '--cpp_out=@BUILD_DIR@', 
                            '@INPUT@'])
proto_src = gen.process('Mission.proto', 'Tracking.proto')
grpcpp_dep = dependency('grpc++')
protobuf_dep = dependency('protobuf')
gpr_dep = dependency('gpr')

#other
thread_dep = dependency('threads')
zlib = dependency('zlib')

shared_library('diginavis',
               'asyncclient.cpp',
               'authorization.cpp',
               'datetimefact.cpp',
               'diginavisplugin.cpp',
               'diginavis.cpp',
               'dronecreator.cpp',
               'drones.cpp',
               'flightrequestapproved.cpp',
               'flightrequestcreator.cpp',
               'flightrequests.cpp',
               'httpapibase.cpp',
               'telemetry.cpp',
                protogrpc_src,
                proto_src,
                moc_files,
                include_directories: [apx_inc],
                dependencies: [apx_dep,
                               gpr_dep,
                               grpcpp_dep,
                               thread_dep,
                               protobuf_dep,
                               qt5_dep,
                               zlib],
                install : true)
