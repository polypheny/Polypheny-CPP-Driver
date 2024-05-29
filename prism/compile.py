import os
import subprocess

# Define directories
prism_source_dir = 'prism/org/polypheny/prism'
includes_dir = 'prism'
generate_header_dir = 'include/'
generated_source_dir = 'src/'

# Ensure the output directories exist
os.makedirs(generate_header_dir, exist_ok=True)
os.makedirs(generated_source_dir, exist_ok=True)

# Compile proto files
for root, _, files in os.walk(prism_source_dir):
    for file in files:
        if file.endswith('.proto'):
            proto_file = os.path.join(root, file)
            command = [
                'protoc',
                f'--proto_path={includes_dir}',
                f'--cpp_out={generated_source_dir}',
                proto_file
            ]
            print(f"Compiling {proto_file}")
            subprocess.run(command, check=True)

# Organize generated files into the include and src directories
def move_generated_files(source_dir, header_dir, source_subdir):
    for root, _, files in os.walk(source_dir):
        for file in files:
            if file.endswith('.h'):
                relative_path = os.path.relpath(root, source_dir)
                target_dir = os.path.join(header_dir, relative_path)
                os.makedirs(target_dir, exist_ok=True)
                source_file = os.path.join(root, file)
                target_file = os.path.join(target_dir, file)
                print(f"Moving {source_file} to {target_file}")
                os.rename(source_file, target_file)
            elif file.endswith('.cc'):
                relative_path = os.path.relpath(root, source_dir)
                target_dir = os.path.join(source_subdir, relative_path)
                os.makedirs(target_dir, exist_ok=True)
                source_file = os.path.join(root, file)
                target_file = os.path.join(target_dir, file)
                print(f"Moving {source_file} to {target_file}")
                os.rename(source_file, target_file)

# Move the generated header and source files to the appropriate directories
move_generated_files(generated_source_dir, generate_header_dir, generated_source_dir)

print("Proto files compilation and organization complete.")
