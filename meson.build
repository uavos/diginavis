project('diginavis', 'cpp',
         version : '0.1',
         default_options : ['warning_level=3', 
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
                                         'diginavisplugin.h',
                                         'diginavis.h',
                                         'dronecreator.h',
                                         'drones.h',
                                         'flightplan.h',
                                         'flightrequestcreator.h',
                                         'flightrequests.h',
                                         'httpapibase.h',
                                         'readonlyfact.h'],
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
            cpp.find_library('gpr', dirs: proto_sdk_lib, static: true),
            cpp.find_library('grpc++_unsecure', dirs: proto_sdk_lib, static: true),
            cpp.find_library('absl_raw_hash_set', dirs: proto_sdk_lib, static: true),
            cpp.find_library('absl_hashtablez_sampler', dirs: proto_sdk_lib, static: true),
            cpp.find_library('absl_exponential_biased', dirs: proto_sdk_lib, static: true),
            cpp.find_library('absl_hash', dirs: proto_sdk_lib, static: true),
            cpp.find_library('absl_city', dirs: proto_sdk_lib, static: true),
            cpp.find_library('absl_statusor', dirs: proto_sdk_lib, static: true),
            cpp.find_library('absl_bad_variant_access', dirs: proto_sdk_lib, static: true),
            cpp.find_library('absl_status', dirs: proto_sdk_lib, static: true),
            cpp.find_library('absl_cord', dirs: proto_sdk_lib, static: true),
            cpp.find_library('absl_str_format_internal', dirs: proto_sdk_lib, static: true),
            cpp.find_library('absl_synchronization', dirs: proto_sdk_lib, static: true),
            cpp.find_library('absl_graphcycles_internal', dirs: proto_sdk_lib, static: true),
            cpp.find_library('absl_symbolize', dirs: proto_sdk_lib, static: true),
            cpp.find_library('absl_demangle_internal', dirs: proto_sdk_lib, static: true),
            cpp.find_library('absl_stacktrace', dirs: proto_sdk_lib, static: true),
            cpp.find_library('absl_debugging_internal', dirs: proto_sdk_lib, static: true),
            cpp.find_library('absl_malloc_internal', dirs: proto_sdk_lib, static: true),
            cpp.find_library('absl_time', dirs: proto_sdk_lib, static: true),
            cpp.find_library('absl_time_zone', dirs: proto_sdk_lib, static: true),
            cpp.find_library('absl_civil_time', dirs: proto_sdk_lib, static: true),
            cpp.find_library('absl_strings', dirs: proto_sdk_lib, static: true),
            cpp.find_library('absl_strings_internal', dirs: proto_sdk_lib, static: true),
            cpp.find_library('absl_throw_delegate', dirs: proto_sdk_lib, static: true),
            cpp.find_library('absl_int128', dirs: proto_sdk_lib, static: true),
            cpp.find_library('absl_base', dirs: proto_sdk_lib, static: true),
            cpp.find_library('absl_spinlock_wait', dirs: proto_sdk_lib, static: true),
            cpp.find_library('absl_bad_optional_access', dirs: proto_sdk_lib, static: true),
            cpp.find_library('absl_raw_logging_internal', dirs: proto_sdk_lib, static: true),
            cpp.find_library('absl_log_severity', dirs: proto_sdk_lib, static: true),
            cpp.find_library('cares', dirs: proto_sdk_lib, static: true),
            cpp.find_library('address_sorting', dirs: proto_sdk_lib, static: true),
            cpp.find_library('grpc', dirs: proto_sdk_lib, static: true),
            cpp.find_library('upb', dirs: proto_sdk_lib, static: true)]

#other
thread_dep = dependency('threads')
zlib = dependency('zlib')

shared_library('diginavis',
               'asyncclient.cpp',
               'authorization.cpp',
               'diginavisplugin.cpp',
               'diginavis.cpp',
               'dronecreator.cpp',
               'drones.cpp',
               'flightplan.cpp',
               'flightrequestcreator.cpp',
               'flightrequests.cpp',
               'httpapibase.cpp',
               'readonlyfact.cpp',
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
