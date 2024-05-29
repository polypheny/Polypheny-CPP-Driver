project('Polypheny-CPP-Driver', 'cpp', version : '1.0.0')

# Dependencies
protobuf_dep = dependency('protobuf', required : true)

# Include directory
inc = include_directories('include')

# Source files
src_files = [
    'src/library.cpp',
    'src/PlainTransport.cpp',
    'src/PrismInterfaceClient.cpp'
]

# Protobuf files
proto_files = files(
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
    'prism/org/polypheny/prism/value.proto'
)

protobuf_gen = generator(find_program('protoc'),
                         output : ['@BASENAME@.pb.cc', '@BASENAME@.pb.h'],
                         arguments : [
                             '--proto_path=@CURRENT_SOURCE_DIR@/prism',  # Adjust the path to your .proto files
                             '--cpp_out=@CURRENT_BINARY_DIR@',
                             '@INPUT@'
                         ]
)

protobuf_gen.process(proto_files)

gen_src_files = [
    'src/generated/org/polypheny/prism/connection_requests.pb.cc',
    'src/generated/org/polypheny/prism/connection_responses.pb.cc',
    'src/generated/org/polypheny/prism/document_frame.pb.cc',
    'src/generated/org/polypheny/prism/error.pb.cc',
    'src/generated/org/polypheny/prism/graph_frame.pb.cc',
    'src/generated/org/polypheny/prism/meta_requests.pb.cc',
    'src/generated/org/polypheny/prism/meta_responses.pb.cc',
    'src/generated/org/polypheny/prism/namespace_meta_requests.pb.cc',
    'src/generated/org/polypheny/prism/namespace_meta_responses.pb.cc',
    'src/generated/org/polypheny/prism/protointerface.pb.cc',
    'src/generated/org/polypheny/prism/relational_frame.pb.cc',
    'src/generated/org/polypheny/prism/statement_requests.pb.cc',
    'src/generated/org/polypheny/prism/statement_responses.pb.cc',
    'src/generated/org/polypheny/prism/transaction_requests.pb.cc',
    'src/generated/org/polypheny/prism/value.pb.cc'
]

src_files += gen_src_files
inc_gen = 'src/generated/org/polypheny/prism'

# Project arguments
add_project_arguments('-Wall', '-Wextra', language : 'cpp')
add_project_link_arguments('-lm', language : 'cpp')

# Combine include directories
inc_all = [inc, inc_gen]

# Library with generated files
library('polypheny-cpp-library', src_files,
        include_directories : inc_all,
        dependencies : [protobuf_dep]
)
