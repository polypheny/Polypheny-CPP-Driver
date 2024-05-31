project('Polypheny-CPP-Driver', 'cpp', version: '1.0.0')
add_project_arguments('-Wall', '-Wextra', language: 'cpp')
add_project_link_arguments('-lm', language: 'cpp')

protobuf_version = '3.14.4'
protobuf_enforce_exact_match = false

dep_protobuf = dependency('protobuf', version: protobuf_version, required: false)
if not dep_protobuf.found()
    dep_protobuf = declare_dependency(
        include_directories: include_directories('/snap/protobuf/current/include'),
        link_args: ['-L/snap/protobuf/current/lib', '-lprotobuf']
    )
    warning('Using Snap fallback for Protobuf.')
endif

protoc = find_program('protoc', required: true)
protoc_version = run_command(protoc, '--version').stdout().strip().split()[-1]

message('Protobuf version: ' + protobuf_version)
message('protoc version: ' + protoc_version)

if protobuf_enforce_exact_match and protoc_version != protobuf_version
    error('protoc version (' + protoc_version + ') does not match protobuf version (' + protobuf_version + ')')
endif

gen_prism = generator(protoc,
                output: ['@BASENAME@.pb.cc', '@BASENAME@.pb.h'],
                arguments: [
                    '--proto_path=@CURRENT_SOURCE_DIR@/prism/org/polypheny/prism',
                    '--cpp_out=@BUILD_DIR@',
                    '-I=@CURRENT_SOURCE_DIR@/prism',
                    '--experimental_allow_proto3_optional',
                    '@INPUT@'
                ]
)

prism_files = files(
    'prism/org/polypheny/prism/connection_requests.proto',
    'prism/org/polypheny/prism/connection_responses.proto',
    'prism/org/polypheny/prism/document_frame.proto',
    'prism/org/polypheny/prism/error.proto',
    'prism/org/polypheny/prism/graph_frame.proto',
    'prism/org/polypheny/prism/meta_requests.proto',
    'prism/org/polypheny/prism/meta_responses.proto',
    'prism/org/polypheny/prism/namespace_meta_requests.proto',
    'prism/org/polypheny/prism/namespace_meta_responses.proto',
    'prism/org/polypheny/prism/protointerface.proto',
    'prism/org/polypheny/prism/relational_frame.proto',
    'prism/org/polypheny/prism/statement_requests.proto',
    'prism/org/polypheny/prism/statement_responses.proto',
    'prism/org/polypheny/prism/transaction_requests.proto',
    'prism/org/polypheny/prism/transaction_responses.proto',
    'prism/org/polypheny/prism/value.proto'
)

src_prism = gen_prism.process(prism_files)
# inc_prism = include_directories('.')

inc_polypheny = include_directories('include')

src_polypheny = files(
    'src/BigDecimal.cpp',
    'src/Connection.cpp',
    'src/ConnectionProperties.cpp',
    'src/Cursor.cpp',
    'src/Document.cpp',
    'src/Interval.cpp',
    'src/library.cpp',
    'src/PlainTransport.cpp',
    'src/PrismInterfaceClient.cpp',
    'src/ProtoUtils.cpp',
    'src/TypedValue.cpp'
)

library('polypheny-cpp-library',
        src_polypheny + src_prism,
        dependencies: [dep_protobuf],
        include_directories: [inc_polypheny]
)
