# Cologra

## Links

[STCE Chair Software Lab
Description](https://www.stce.rwth-aachen.de/teaching/winter-2024-25/software-lab-parallel-graph-algorithms)

[Zotero Group](https://www.zotero.org/groups/5682542/parallel_graph_algorithms)

## Usage

You need to have CMake and
[OpenMPI](https://docs.open-mpi.org/en/v5.0.x/launching-apps/quickstart.html)
installed. If you are using Ubuntu, you can apt-get install them with `./cologra.sh
install`. Use the other `./cologra.sh` commands to build/run the project and to
remove the build directory.

```shell
./cologra.sh install
./cologra.sh build
./cologra.sh run
./cologra.sh build-run
./cologra.sh clean
./cologra.sh format
./cologra.sh test
```

## Package Manager

We use [vcpkg](https://github.com/Microsoft/vcpkg) as a package manager. Follow
the instructions on the GitHub page to install it. Then you can install the
required packages with:

```shell
vcpkg install
```

## Code Style

We use [ClangFormat](https://clang.llvm.org/docs/ClangFormat.html) to format the
code. You can run it with:

```shell
./cologra.sh format
```

## Testing

We use [Google Test](https://github.com/google/googletest) for testing. After
building the project, you can run the tests with:

```shell
./cologra.sh test
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
