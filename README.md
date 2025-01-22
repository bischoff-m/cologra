# Cologra

## Links

[STCE Chair Software Lab
Description](https://www.stce.rwth-aachen.de/teaching/winter-2024-25/software-lab-parallel-graph-algorithms)

[Zotero Group](https://www.zotero.org/groups/5682542/parallel_graph_algorithms)

## Installation

You need to have CMake and
[OpenMPI](https://docs.open-mpi.org/en/v5.0.x/launching-apps/quickstart.html)
installed. If you want to format the code, you also need to have
[ClangFormat](https://clang.llvm.org/docs/ClangFormat.html) installed. For
testing, we use [Google Test](https://github.com/google/googletest), but you
don't need to install it.

```shell
sudo apt-get install cmake
sudo apt-get install openmpi-bin openmpi-common libopenmpi-dev
sudo apt-get install clang-format
sudo apt-get install valgrind valgrind-mpi
```

## Usage

To compile, run the tests, and install:

```shell
make
make test
sudo make install
```

Here is a full list of the available commands:

```shell
make build # Same as make
make run
make debug
make test
make test_verbose
make install
make uninstall
make format
make clean
```

## Package Manager

We use [vcpkg](https://github.com/Microsoft/vcpkg) as a package manager. Follow
the instructions on the GitHub page to install it. Then you can install the
required packages with:

```shell
vcpkg install
```

## Editor Setup

### CLion

ClangFormat is already configured in the `.clang-format` file and will
automatically be used by CLion. You can use [this
setup](https://www.jetbrains.com/help/clion/openmpi.html) to configure CLion to
use OpenMPI.

### VS Code

Install the [clang-format
extension](https://marketplace.visualstudio.com/items?itemName=xaver.clang-format)
and configure it according to the extension's instructions.
Also, install the [CMake Tools
extension](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cmake-tools)
and GDB.

```shell
sudo apt-get install gdb
```

`.vscode/tasks.json`

```json
{
  "version": "2.0.0",
  "tasks": [
    {
      "type": "shell",
      "label": "CMake build",
      "command": "sh",
      "args": ["cmd/build.sh"],
      "options": {
        "cwd": "${workspaceFolder}"
      },
      "problemMatcher": ["$gcc"],
      "group": {
        "kind": "build",
        "isDefault": true
      }
    }
  ]
}
```

`.vscode/launch.json`

```json
{
  "version": "0.2.0",
  "configurations": [
    {
      "name": "Build and Debug",
      "type": "cppdbg",
      "request": "launch",
      "program": "${workspaceFolder}/build/cologra",
      "cwd": "${workspaceFolder}",
      "externalConsole": false,
      "showDisplayString": true,
      "preLaunchTask": "CMake build",
      "stopAtEntry": false,
      "miDebuggerPath": "/usr/bin/gdb"
    }
  ]
}
```

## Errors

When trying to run on the cluster, I got the following error:

```shell
build/src/cologra_cli: symbol lookup error: build/src/cologra_cli: undefined symbol: __libm_feature_flag
```

To fix this, I compiled using `GCCcore/.11.3.0` instead of the standard
compiler. To unload the Intel compiler, use:

```shell
module unload intel-compilers/2022.1.0
```
