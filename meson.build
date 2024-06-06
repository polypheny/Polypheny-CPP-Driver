project('Polypheny-CPP-Driver', 'cpp', version : '1.0.0', default_options : ['cpp_std=c++17'])
add_project_arguments('-Wall', '-Wextra', '-w', '-fcompare-debug-second', language : 'cpp')
add_project_link_arguments('-lm', language : 'cpp')

# ========================
# Dependencies
# ========================

# prism api
polypheny_prism_api_subproject = subproject('polypheny_prism_api')
polypheny_prism_api_dep = polypheny_prism_api_subproject.get_variable('polypheny_prism_api_dep')
# pthreads
pthread_dep = dependency('threads')

# ========================
# Test Dependencies
# ========================

# unit test system
catch2_subproject = subproject('catch2')
catch2_dep = catch2_subproject.get_variable('catch2_with_main_dep')

# ========================
# Library and Dependencies
# ========================

polypheny_inc = include_directories('include')
polypheny_src_inc = include_directories('src')

polypheny_src = files(
    'src/BigDecimal.cpp',
    'src/CallbackQueue.cpp',
    'src/Connection.cpp',
    'src/ConnectionClosedError.cpp',
    'src/ConnectionProperties.cpp',
    'src/Cursor.cpp',
    'src/Document.cpp',
    'src/DocumentResult.cpp',
    'src/GraphResult.cpp',
    'src/Interval.cpp',
    'src/PlainTransport.cpp',
    'src/PrismInterfaceClient.cpp',
    'src/ProtoUtils.cpp',
    'src/RelationalResult.cpp',
    'src/ScalarResult.cpp',
    'src/ServerError.cpp',
    'src/TransportUtils.cpp',
    'src/TypedValue.cpp'
)

polypheny_cpp_driver_lib = library('polypheny_cpp_library',
        polypheny_src,
        dependencies : [pthread_dep, polypheny_prism_api_dep],
        include_directories : [polypheny_inc, polypheny_src_inc],
)


polypheny_cpp_driver_dep = declare_dependency(
    link_with : polypheny_cpp_driver_lib,
    include_directories : [polypheny_inc, polypheny_src_inc],
    dependencies : [pthread_dep, polypheny_prism_api_dep]
)

# ========================
# Tests
# ========================

test_src = files(
    'tests/Init.cpp',
    'tests/IntervalTest.cpp',
    'tests/BigDecimalTest.cpp',
    'tests/CallbackQueueTest.cpp',
    'tests/ConnectionClosedErrorTest.cpp',
    'tests/ConnectionPropertiesTest.cpp',
    'tests/DocumentTest.cpp',
    'tests/ProtoUtilsTest.cpp',
    'tests/TypedValueTest.cpp',
)

test_exe = executable('runTests',
                      test_src + polypheny_src,
                      dependencies: [catch2_dep, pthread_dep, polypheny_prism_api_dep],
                      include_directories : [polypheny_inc, polypheny_src_inc],
                      install: true
)

test('runTests', test_exe)

# ========================
# Examples
# ========================

subdir('examples')