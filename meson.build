project('Polypheny-CPP-Driver', 'cpp', version : '1.0.0', meson_version : '>=1.0',
        default_options : ['cpp_std=c++17'])

# ========================
# Dependencies
# ========================

# prism api
prism_streaming = get_option('prism_streaming')
polypheny_prism_api_subproject = subproject(prism_streaming ? 'polypheny_prism_streaming' : 'polypheny_prism_api')
polypheny_prism_api_dep = polypheny_prism_api_subproject.get_variable('polypheny_prism_api_dep')
prism_config = configuration_data()
prism_config.set10('POLYPHENY_PRISM_STREAMING', prism_streaming)
configure_file(output : 'PrismConfiguration.h', configuration : prism_config)
# pthreads
pthread_dep = dependency('threads')
# GNU Multiple Precision Arithmetic Library (used for BigDecimals)
gmp_dep = dependency('gmp')
gmpxx_dep = dependency('gmpxx')
# open ssl for transport security
ssl_dep = dependency('openssl')

# ========================
# Test Dependencies
# ========================

# unit test system
catch2_dep = dependency('catch2-with-main', version : '>=3', fallback : ['catch2', 'catch2_with_main_dep'])

# ========================
# Library and Dependencies
# ========================

polypheny_inc = include_directories('.', 'include', 'src')

polypheny_src = files(
    'src/connection/CallbackQueue.cpp',
    'src/connection/Connection.cpp',
    'src/transport/ConnectionClosedError.cpp',
    'src/connection/ConnectionProperties.cpp',
    'src/connection/Cursor.cpp',
    'src/results/DocumentResult.cpp',
    'src/results/GraphElement.cpp',
    'src/results/GraphResult.cpp',
    'src/results/Edge.cpp',
    'src/results/Node.cpp',
    'src/types/Interval.cpp',
    'src/types/File.cpp',
    'src/transport/SocketTransport.cpp',
    'src/transport/UnixTransport.cpp',
    'src/transport/PlainTcpTransport.cpp',
    'src/transport/SecureTcpTransport.cpp',
    'src/connection/PrismInterfaceClient.cpp',
    'src/utils/ProtoUtils.cpp',
    'src/utils/TypedValueUtils.cpp',
    'src/results/RelationalColumnMetadata.cpp',
    'src/results/RelationalMetadata.cpp',
    'src/results/RelationalResult.cpp',
    'src/results/Row.cpp',
    'src/results/ScalarResult.cpp',
    'src/connection/ServerError.cpp',
    'src/types/TypedValue.cpp',
    'src/streaming/StreamingIndex.cpp',
    'src/streaming/PrismOutputStream.cpp',
)
if prism_streaming
    polypheny_src += files(
        'src/streaming/BinaryPrismOutputStream.cpp',
        'src/streaming/FilePrismOutputStream.cpp',
        'src/streaming/StringPrismOutputStream.cpp',
        'src/streaming/StringPrismInputStream.cpp',
    )
endif

driver_dependencies = [pthread_dep, polypheny_prism_api_dep, gmp_dep, gmpxx_dep, ssl_dep]

polypheny_cpp_driver_lib = library('polypheny_cpp_library',
                                   polypheny_src,
                                   dependencies : driver_dependencies,
                                   include_directories : polypheny_inc,
)


polypheny_cpp_driver_dep = declare_dependency(
    link_with : polypheny_cpp_driver_lib,
    include_directories : polypheny_inc,
    dependencies : driver_dependencies
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
    'tests/SocketTransportTest.cpp',
    'tests/ProtoUtilsTest.cpp',
    'tests/TypedValueTest.cpp',
    'tests/DocumentMetadataTest.cpp',
)
if prism_streaming
    test_src += files('tests/StreamingTest.cpp')
endif

test_exe = executable('runTests',
                      test_src,
                      dependencies : [catch2_dep, polypheny_cpp_driver_dep],
)

test('runTests', test_exe)

# ========================
# Examples
# ========================

subdir('examples')
