# Build BB_Launcher for Windows

## Option 1: VSCode with Visual Studio Build Tools

If your default IDE is VSCode, we have a fully functional example for that as well.

### Requirements

* [**Git for Windows**](https://git-scm.com/download/win)
* [**LLVM 19.1.1**](https://github.com/llvm/llvm-project/releases/download/llvmorg-19.1.1/LLVM-19.1.1-win64.exe)
* [**CMake 4.2.3 or newer**](https://github.com/Kitware/CMake/releases/download/v4.2.3/cmake-4.2.3-windows-x86_64.msi)
* [**Ninja 1.13.2 or newer**](https://github.com/ninja-build/ninja/releases/download/v1.13.2/ninja-win.zip)

**The main reason we use clang19 is because that version is used in CI for formatting.**

### Installs

1. Go through the Git for Windows installation as normal
2. Download and Run LLVM Installer and `Add LLVM to the system PATH for all users`
3. Download and Run CMake Installer and `Add CMake to the system PATH for all users`
4. Download Ninja and extract it to `C:\ninja` and add it to the system PATH for all users    
    * You can do this by going to `Search with Start Menu -> Environment Variables -> System Variables -> Path -> Edit -> New -> C:\ninja`

### Validate the installs

```bash
git --version
# git version 2.49.0.windows.1

cmake --version
# cmake version 4.2.3

ninja --version
# 1.13.2

clang --version
# clang version 19.1.1
```

### Install Visual Studio Build Tools

1. Download [Visual Studio Build Tools](https://aka.ms/vs/17/release/vs_BuildTools.exe)
2. Select `MSVC - Windows SDK` and install (you don't need to install an IDE)

* Or you can install via `.vsconfig` file:

```bash
{
  "version": "1.0",
  "components": [
    "Microsoft.VisualStudio.Component.Roslyn.Compiler",
    "Microsoft.Component.MSBuild",
    "Microsoft.VisualStudio.Component.CoreBuildTools",
    "Microsoft.VisualStudio.Workload.MSBuildTools",
    "Microsoft.VisualStudio.Component.Windows10SDK",
    "Microsoft.VisualStudio.Component.VC.CoreBuildTools",
    "Microsoft.VisualStudio.Component.VC.Tools.x86.x64",
    "Microsoft.VisualStudio.Component.VC.Redist.14.Latest",
    "Microsoft.VisualStudio.Component.Windows11SDK.26100",
    "Microsoft.VisualStudio.Component.TestTools.BuildTools",
    "Microsoft.VisualStudio.Component.VC.ASAN",
    "Microsoft.VisualStudio.Component.TextTemplating",
    "Microsoft.VisualStudio.ComponentGroup.NativeDesktop.Core",
    "Microsoft.VisualStudio.Workload.VCTools"
  ],
  "extensions": []
}

# Save the file as `.vsconfig` and run the following command:

%userprofile%\Downloads\vs_BuildTools.exe --passive --config ".vsconfig"

# Be carefull path to vs_BuildTools.exe and .vsconfig file.
```

__This will install the necessary components to build BB_Launcher.__

### Project structure

```
bb_launcher/
  ├── shared (bb_launcher main files)
  └── bb_launcher.code-workspace
```

### Content of `bb_launcher.code-workspace`

```json
{
  "folders": [
    {
      "path": "shared"
    }
  ],
  "settings": {
    "cmake.generator": "Ninja",
    
	"cmake.configureEnvironment": {
    "CMAKE_CXX_STANDARD": "23",
    "CMAKE_CXX_STANDARD_REQUIRED": "ON",
    "CMAKE_EXPORT_COMPILE_COMMANDS": "ON"
  },

    "cmake.configureOnOpen": false,

    "C_Cpp.intelliSenseEngine": "Disabled",

    "clangd.arguments": [
      "--background-index",
      "--clang-tidy",
      "--completion-style=detailed",
      "--header-insertion=never",
      "--compile-commands-dir=Build/x64-Clang-Release"
    ],

    "clang-format.executable": "clang-format",
    "cmake.configureArgs": [
      "-DCRYPTOPP_DISABLE_ASM=ON"
    ]
  },

  "extensions": {
    "recommendations": [
      "llvm-vs-code-extensions.vscode-clangd",
      "ms-vscode.cmake-tools",
      "xaver.clang-format"
    ]
  }
}
```

### Cloning the source code

1. Open your terminal and where to BB_Launcher folder: `cd bb_launcher\shared`
3. Clone the repository by running  
    `git clone --depth 1 --recursive https://github.com/rainmakerv3/BB_Launcher.git .`

_or fork link_

* If you have already cloned repo:
```bash
git submodule update --init --recursive
```

### Requirements VSCode extensions
1. CMake Tools
2. Clangd
3. Clang-Format

_These plugins are suggested in the workspace file above and are already configured._

### Building
1. Open VS Code, `File > Open workspace from file > bb_launcher.code-workspace`
2. Go to the CMake Tools extension on left side bar
3. Change Clang x64 Debug to Clang x64 Release if you want a regular, non-debug build.
4. Click build.

Your BB_Launcher.exe will be in `bb_launcher\shared\Build\x64-Clang-Release\`