project('Polypheny-CPP-Driver', 'cpp', version : '1.0.0')

inc = include_directories('include')

protobuf_dep = dependency('protobuf', required : true)


gen_src_files = files(
    'src/org/polypheny/prism/connection_requests.pb.cc',
    'src/org/polypheny/prism/connection_responses.pb.cc',
    'src/org/polypheny/prism/document_frame.pb.cc',
    'src/org/polypheny/prism/error.pb.cc',
    'src/org/polypheny/prism/graph_frame.pb.cc',
    'src/org/polypheny/prism/meta_requests.pb.cc',
    'src/org/polypheny/prism/meta_responses.pb.cc',
    'src/org/polypheny/prism/namespace_meta_requests.pb.cc',
    'src/org/polypheny/prism/namespace_meta_responses.pb.cc',
    'src/org/polypheny/prism/protointerface.pb.cc',
    'src/org/polypheny/prism/relational_frame.pb.cc',
    'src/org/polypheny/prism/statement_requests.pb.cc',
    'src/org/polypheny/prism/statement_responses.pb.cc',
    'src/org/polypheny/prism/transaction_requests.pb.cc',
    'src/org/polypheny/prism/value.pb.cc'
)

src_files = files(
    'src/library.cpp',
    'src/PlainTransport.cpp',
    'src/PrismInterfaceClient.cpp'
)

add_project_arguments('-Wall', '-Wextra', language : 'cpp')
add_project_link_arguments('-lm', language : 'cpp')

library('polypheny-cpp-library',
        src_files + gen_src_files,
        include_directories : [inc],
        dependencies : [protobuf_dep]
)
