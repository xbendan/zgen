#!/usr/bin/env bash
set -e

rm_path=$(cd -- "$(dirname "$0")" && cd .. && pwd)
rm_config="$rm_path/config.conf"

if [ -f rm_config ]; then
    source rm_config
fi

llvm_version=${llvm_version:-"21.1.8"}
llvm_package="llvm-project-${llvm_version}.src"
llvm_dir="$rm_path/ext/llvm"
llvm_src="${llvm_dir}/${llvm_version}-src"
llvm_dest="${llvm_dir}/${llvm_package}.tar.xz"
llvm_url="https://github.com/llvm/llvm-project/releases/download/llvmorg-${llvm_version}/${llvm_package}.tar.xz"

mkdir -p ${llvm_dir}
echo "download llvm-project source code..."

if [ -d "$llvm_src" ]; then
    echo "llvm-project source dir already exists, skipped."
else
    if [ ! -f "$llvm_dest" ]; then
        echo "downloading llvm-project source code from ${llvm_url}"
        curl -L -C - "${llvm_url}" -o "${llvm_dest}"
    else
        echo "llvm-project archive already exists, skipped."
    fi

    echo "unpacking to ${llvm_src}..."
    mkdir -p "${llvm_src}"
    tar -xJf "${llvm_dest}" -C "${llvm_src}" --strip-components=1
    echo "unpacked."
fi