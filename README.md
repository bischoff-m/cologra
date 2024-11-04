# ColoGra

## Links

[STCE Chair Software Lab
Description](https://www.stce.rwth-aachen.de/teaching/winter-2024-25/software-lab-parallel-graph-algorithms)

[Zotero Group](https://www.zotero.org/groups/5682542/parallel_graph_algorithms)

## Usage

You need to have CMake and
[OpenMPI](https://docs.open-mpi.org/en/v5.0.x/launching-apps/quickstart.html)
installed. If you are using Ubuntu, you can apt-get install them with `make
install`. Use the other `make` commands to build/run the project and to
remove the build directory.

```shell
make install
make build
make run
make clean
```

## Editor Setup

If you are using CLion, you can use [this
setup](https://www.jetbrains.com/help/clion/openmpi.html).

For VS Code you can install the [CMake Tools
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
