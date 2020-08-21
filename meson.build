project('diginavis', 'cpp',
         version : '0.1',
         default_options : ['warning_level=3', 
                            'cpp_std=c++14'])

qt5_dep = dependency('qt5', modules: ['Core'])

cpp = meson.get_compiler('cpp')
apx_sdk = '/home/pavel/apx_sdk'
apx_inc = include_directories(join_paths(apx_sdk, 'include/ApxGcs'),
                              join_paths(apx_sdk, 'include/ApxCore'),
                              join_paths(apx_sdk, 'include/ApxData'),
                              join_paths(apx_sdk, 'include/ApxShared'))
apx_dep = [cpp.find_library('ApxCore', dirs: join_paths(apx_sdk, 'lib')),
           cpp.find_library('ApxData', dirs: join_paths(apx_sdk, 'lib')),
           cpp.find_library('ApxGcs', dirs: join_paths(apx_sdk, 'lib')),
           cpp.find_library('ApxShared', dirs: join_paths(apx_sdk, 'lib'))]

qt5 = import('qt5')
moc_files = qt5.preprocess(moc_headers: ['diginavisplugin.h'],
                           include_directories: apx_inc,
                           dependencies: qt5_dep)

shared_library('diginavis',
               'diginavisplugin.cpp',
                moc_files,
                include_directories: apx_inc,
                dependencies: [apx_dep,
                               qt5_dep],
                install : true)
