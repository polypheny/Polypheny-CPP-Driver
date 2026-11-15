project('polypheny_prism_api', 'cpp', default_options : ['cpp_std=c++17'])

# protoc and the C++ library should come from the same Protobuf installation.
protobuf_dep = dependency('protobuf')
protoc = find_program('protoc', native : true)
fs = import('fs')
proto_dir = 'org/polypheny/prism'
prism_files = files(
    proto_dir / 'connection_requests.proto',
    proto_dir / 'connection_responses.proto',
    proto_dir / 'document_frame.proto',
    proto_dir / 'error.proto',
    proto_dir / 'graph_frame.proto',
    proto_dir / 'meta_requests.proto',
    proto_dir / 'meta_responses.proto',
    proto_dir / 'namespace_meta_requests.proto',
    proto_dir / 'namespace_meta_responses.proto',
    proto_dir / 'protointerface.proto',
    proto_dir / 'relational_frame.proto',
    proto_dir / 'statement_requests.proto',
    proto_dir / 'statement_responses.proto',
    proto_dir / 'transaction_requests.proto',
    proto_dir / 'transaction_responses.proto',
    proto_dir / 'value.proto',
)
if fs.exists(proto_dir / 'stream_requests.proto')
    prism_files += files(proto_dir / 'stream_requests.proto', proto_dir / 'stream_responses.proto')
endif

prism_gen = generator(protoc,
    output : ['@BASENAME@.pb.cc', '@BASENAME@.pb.h'],
    arguments : ['--proto_path=@CURRENT_SOURCE_DIR@', '--cpp_out=@BUILD_DIR@', '@INPUT@'],
)
prism_src = prism_gen.process(prism_files, preserve_path_from : meson.current_source_dir())

# Meson places generated files in the static library's private build directory.
generated_dir = 'libpolypheny_prism_api.a.p'
run_command('mkdir', '-p', meson.current_build_dir() / generated_dir, check : true)
prism_inc = include_directories(generated_dir)
prism_lib = static_library('polypheny_prism_api', prism_src,
    dependencies : protobuf_dep,
    include_directories : prism_inc,
)

polypheny_prism_api_dep = declare_dependency(
    link_with : prism_lib,
    include_directories : prism_inc,
    dependencies : protobuf_dep,
)
