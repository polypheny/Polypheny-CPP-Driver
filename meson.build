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
# GNU Multiple Precision Arithmetic Library (used for BigDecimals)
gmp_dep = dependency('gmp', required : true, fallback : ['gmp', 'gmp_dep'])
gmpxx_dep = dependency('gmpxx', required : true, fallback : ['gmp', 'gmpxx_dep'])

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
    'src/connection/CallbackQueue.cpp',
    'src/connection/Connection.cpp',
    'src/transport/ConnectionClosedError.cpp',
    'src/connection/ConnectionProperties.cpp',
    'src/connection/Cursor.cpp',
    'src/connection/Statement.cpp',
    'src/results/DocumentResult.cpp',
    'src/results/GraphElement.cpp',
    'src/results/GraphResult.cpp',
    'src/results/Edge.cpp',
    'src/results/Node.cpp',
    'src/types/Interval.cpp',
    'src/transport/PlainTransport.cpp',
    'src/transport/UnixTransport.cpp',
    'src/connection/PrismInterfaceClient.cpp',
    'src/utils/ProtoUtils.cpp',
    'src/utils/TypedValueUtils.cpp',
    'src/results/RelationalColumnMetadata.cpp',
    'src/results/RelationalMetadata.cpp',
    'src/results/RelationalResult.cpp',
    'src/results/Row.cpp',
    'src/results/ScalarResult.cpp',
    'src/connection/ServerError.cpp',
    'src/types/TypedValue.cpp'
)

polypheny_cpp_driver_lib = library('polypheny_cpp_library',
                                   polypheny_src,
                                   dependencies : [pthread_dep, polypheny_prism_api_dep, gmp_dep, gmpxx_dep],
                                   include_directories : [polypheny_inc, polypheny_src_inc],
)


polypheny_cpp_driver_dep = declare_dependency(
    link_with : polypheny_cpp_driver_lib,
    include_directories : [polypheny_inc, polypheny_src_inc],
    dependencies : [pthread_dep, polypheny_prism_api_dep, gmp_dep, gmpxx_dep]
)

# ========================
# Tests
# ========================

test_src = files(
    'tests/Init.cpp',
    'tests/IntervalTest.cpp',
    'tests/CallbackQueueTest.cpp',
    'tests/ConnectionClosedErrorTest.cpp',
    'tests/ConnectionPropertiesTest.cpp',
    'tests/ProtoUtilsTest.cpp',
    'tests/TypedValueTest.cpp',
)

test_exe = executable('runTests',
                      test_src + polypheny_src,
                      dependencies : [catch2_dep, pthread_dep, polypheny_prism_api_dep, gmp_dep, gmpxx_dep],
                      include_directories : [polypheny_inc, polypheny_src_inc],
                      install : true
)

test('runTests', test_exe)

# ========================
# Examples
# ========================

subdir('examples')