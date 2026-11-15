#!/usr/bin/env bash
set -euo pipefail

output_dir=${1:?Usage: bash scripts/package-release.sh OUTPUT_DIR [RELEASE_TAG]}
release_tag=${2:-}
version=$(meson introspect --projectinfo meson.build |
    python3 -c 'import json, sys; print(json.load(sys.stdin)["version"])')

if [[ ! $version =~ ^[0-9]+\.[0-9]+\.[0-9]+$ ]]; then
    printf 'Expected a stable Meson project version, got %s\n' "$version" >&2
    exit 1
fi
if [[ -n $release_tag && $release_tag != "v$version" ]]; then
    printf 'Release tag %s does not match the Meson project version v%s\n' "$release_tag" "$version" >&2
    exit 1
fi

package_name="Polypheny-CPP-Driver-$version"
temp_dir=$(mktemp -d)
trap 'rm -rf -- "$temp_dir"' EXIT
mkdir -p "$temp_dir/download" "$temp_dir/source" "$output_dir"

# Meson checks the .wrap hashes while fetching the three source archives.
git archive HEAD | tar -xf - -C "$temp_dir/download"
meson subprojects download --sourcedir "$temp_dir/download" \
    polypheny_prism_api polypheny_prism_streaming catch2

# Stage the tracked sources and the original archives, without extracted dependencies or build outputs.
git archive --prefix="$package_name/" HEAD | tar -xf - -C "$temp_dir/source"
cp -R "$temp_dir/download/subprojects/packagecache" "$temp_dir/source/$package_name/subprojects/"
commit_time=$(git show -s --format=%ct HEAD)
tar --sort=name --mtime="@$commit_time" --owner=0 --group=0 --numeric-owner \
    -cf - -C "$temp_dir/source" "$package_name" | gzip -n > "$output_dir/$package_name.tar.gz"

(
    cd "$output_dir"
    sha256sum "$package_name.tar.gz" > "$package_name.tar.gz.sha256"
)
