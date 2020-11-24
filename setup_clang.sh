#
# Environment setup metascript for arm64 Android kernel builds with Clang
# Copyright (C) 2019-2020 Danny Lin <danny@kdrag0n.dev>
#
# This script must be *sourced* from a Bourne-compatible shell in order to
# function. Nothing will happen if you execute it.
#

# Path to executables in Clang toolchain
clang_bin="$HOME/toolchains/proton-clang/bin"

# 64-bit GCC toolchain prefix
gcc_prefix64="aarch64-linux-gnu-"

# 32-bit GCC toolchain prefix
gcc_prefix32="arm-linux-gnueabi-"

# Number of parallel jobs to run
# Do not remove; set to 1 for no parallelism.
jobs=$(nproc)

# Do not edit below this point
# ----------------------------

# Load the shared helpers
source helpers.sh

_ksetup_old_path="$PATH"
export PATH="$clang_bin:$PATH"

# Index of variables for cleanup in unsetup
_ksetup_vars+=(
	clang_bin
	gcc_prefix64
	gcc_prefix32
	jobs
	kmake_flags
)

kmake_flags+=(
	CC="clang"
	AR="llvm-ar"
	NM="llvm-nm"
	OBJCOPY="llvm-objcopy"
	OBJDUMP="llvm-objdump"
	STRIP="llvm-strip"
	LD="ld.lld"

	CROSS_COMPILE="$gcc_prefix64"
	CROSS_COMPILE_ARM32="$gcc_prefix32"

	KBUILD_COMPILER_STRING="$(get_clang_version clang)"
)
