#!/bin/bash
case $1 in
    "build")
        mkdir -p build
        cd build
        cmake .. --preset=vcpkg
        make
        cd ..
        ;;
    "run")
        mpirun -np 1 build/cologra
        ;;
    "build-run")
        source cologra.sh build && source cologra.sh run
        ;;
    "install")
        sudo apt-get install cmake openmpi-bin openmpi-common libopenmpi-dev clang-format
        vcpkg install
        ;;
    "clean")
        rm -rf build
        ;;
    "format")
        find src -iname '*.h' -o -iname '*.cpp' | xargs clang-format -i
        ;;
    "test")
        ./build/test/cologra-test
        ;;
    *)
        echo "Usage: $0 {build|run|build-run|install|clean|format|test}"
        exit 1
        ;;
esac
