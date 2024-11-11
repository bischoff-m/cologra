#-------------------------------------------------------------------------------
# cologra/Makefile
#
# Reference: https://github.com/GraphBLAS/LAGraph/blob/stable/Makefile
#-------------------------------------------------------------------------------
#
# A simple Makefile, which relies on cmake to do the actual build. All the work
# is done in cmake so this Makefile is just for convenience.
#
# To compile and run the tests:
#
#     make
#     make test
#
# To compile with an alternate compiler:
#
#     make CC=gcc CXX=g++
#
# To compile/install for system-wide usage (typically in /usr/local):
#
#     make
#     sudo make install
#
# To clean up the files:
#
#     make clean
#
# To uninstall:
#
#     make uninstall
#
#-------------------------------------------------------------------------------

JOBS ?= 8

# Ensures that make always rebuilds the project
.PHONY: build debug

default: build

all: build

build:
	( cd build && cmake .. --preset=vcpkg && cmake --build . --config Release -j${JOBS} )

run: build
	- mpirun -np 1 build/src/cologra_cli

debug:
	( cd build && cmake -DCMAKE_BUILD_TYPE=Debug .. --preset=vcpkg && cmake --build . --config Release -j${JOBS} )

test: build
	( cd build/tests && ctest . || ctest . --rerun-failed --output-on-failure )

verbose_test: build
	( cd build/tests && ctest . --verbose || ctest . --rerun-failed --output-on-failure )

install:
	( cd build && cmake --install . )

# Remove any installed libraries and #include files
uninstall:
	- xargs rm < build/install_manifest.txt

format:
	- find src tests -iname '*.hpp' -o -iname '*.cpp' | xargs clang-format -i

# Remove all build files
clean:
	- $(RM) -r build/*
