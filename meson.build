project('Polypheny-CPP-Driver', 'cpp', version : '1.0.0', default_options : ['cpp_std=c++17'])
add_project_arguments('-Wall', '-Wextra', '-w', '-fcompare-debug-second', language : 'cpp')
add_project_link_arguments('-lm', language : 'cpp')

# ========================
# Dependencies
# ========================

# libprotobuf ant protoc
protobuf_subproject = subproject('protobuf')
protobuf_dep = protobuf_subproject.get_variable('protobuf_dep')
protoc = protobuf_subproject.get_variable('protoc')
# pthreads
pthread_dep = dependency('threads')

# ========================
# Test Dependencies
# ========================

# unit test system
catch2_subproject = subproject('catch2')
catch2_dep = catch2_subproject.get_variable('catch2_with_main_dep')

# ========================
# Prism API Compilation
# ========================

prism_gen = generator(protoc,
                      output : ['@BASENAME@.pb.cc', '@BASENAME@.pb.h'],
                      arguments : [
                          '--proto_path=@CURRENT_SOURCE_DIR@/prism',
                          '--cpp_out=@BUILD_DIR@',
                          '--experimental_allow_proto3_optional',
                          '@INPUT@'
                      ]
)

prism_files = files(
    'prism/connection_requests.proto',
    'prism/connection_responses.proto',
    'prism/document_frame.proto',
    'prism/error.proto',
    'prism/graph_frame.proto',
    'prism/meta_requests.proto',
    'prism/meta_responses.proto',
    'prism/namespace_meta_requests.proto',
    'prism/namespace_meta_responses.proto',
    'prism/protointerface.proto',
    'prism/relational_frame.proto',
    'prism/statement_requests.proto',
    'prism/statement_responses.proto',
    'prism/transaction_requests.proto',
    'prism/transaction_responses.proto',
    'prism/value.proto'
)

prism_src = prism_gen.process(prism_files)

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
    'src/library.cpp',
    'src/PlainTransport.cpp',
    'src/PrismInterfaceClient.cpp',
    'src/ProtoUtils.cpp',
    'src/RelationalResult.cpp',
    'src/ScalarResult.cpp',
    'src/TypedValue.cpp'
)

polypheny_cpp_driver_lib = library('polypheny-cpp-library',
        polypheny_src + prism_src,
        dependencies : [protobuf_dep, pthread_dep],
        include_directories : [polypheny_inc, polypheny_src_inc]
)

# ========================
# Tests
# ========================

test_src = files(
    'tests/Init.cpp',
    'tests/IntervalTest.cpp'
)

test_exe = executable('runTests',
                      test_src + polypheny_src + prism_src,
                      dependencies: [catch2_dep, protobuf_dep, pthread_dep],
                      include_directories : [polypheny_inc, polypheny_src_inc],
                      install: true
)

test('runTests', test_exe)