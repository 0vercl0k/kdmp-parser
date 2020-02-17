# kdmp-parser

This is a small C++ library able to parse Windows kernel full dump (`.dump /f` in WIndbg) as well as BMP dump (`.dump /ka` in Windbg). The format has been introduced around Windows 8 timeframe according to the [rekall](https://github.com/google/rekall) project. Note most of the structures used in [kdmp-parser-structs.h]() have been adapted / taken from the [rekall](https://github.com/google/rekall) project and their [Python implementation](https://github.com/google/rekall/blob/master/rekall-core/rekall/plugins/overlays/windows/crashdump.py). 

It is only able to load up dumps from 64-bit systems; though it is able to do that from both 32/64-bit build build of the library. It provides read access (no write access) to:

- The context record,
- The exception record,
- The physical memory.

Special cheers to [yrp604](https://github.com/yrp604) for being knowledgeable about it.

## Building

You can build it using [Visual Studio 2019](https://visualstudio.microsoft.com/downloads/) by opening the `kdmp-parser.sln` solution file or via command line using `msbuild` (from a Visual Studio shell):

```text
(C:\ProgramData\Anaconda2) c:\work\codes\kdmp-parser\src>msbuild -t:Build -p:Configuration=Debug
Microsoft (R) Build Engine version 16.4.0+e901037fe for .NET Framework
Copyright (C) Microsoft Corporation. All rights reserved.

Building the projects in this solution one at a time. To enable parallel build, please add the "-m" switch.
Build started 2/16/2020 3:44:32 PM.
Project "c:\work\codes\kdmp-parser\src\kdmp-parser.sln" on node 1 (Build target(s)).
ValidateSolutionConfiguration:
  Building solution configuration "Debug|x64".
Project "c:\work\codes\kdmp-parser\src\kdmp-parser.sln" (1) is building "c:\work\codes\kdmp-parser\src\test\test.vcxproj.metaproj" (2) on node 1 (default targets).
Project "c:\work\codes\kdmp-parser\src\test\test.vcxproj.metaproj" (2) is building "c:\work\codes\kdmp-parser\src\kdmp-parser\kdmp-parser.vcxproj" (3) on node 1 (default targets).
InitializeBuildStatus:
  Creating "x64\Debug\kdmp-parser.tlog\unsuccessfulbuild" because "AlwaysCreate" was specified.
ClCompile:
  All outputs are up-to-date.
Lib:
  All outputs are up-to-date.
  kdmp-parser.vcxproj -> c:\work\codes\kdmp-parser\src\x64\Debug\kdmp-parser.lib
FinalizeBuildStatus:
  Deleting file "x64\Debug\kdmp-parser.tlog\unsuccessfulbuild".
  Touching "x64\Debug\kdmp-parser.tlog\kdmp-parser.lastbuildstate".
Done Building Project "c:\work\codes\kdmp-parser\src\kdmp-parser\kdmp-parser.vcxproj" (default targets).

Project "c:\work\codes\kdmp-parser\src\test\test.vcxproj.metaproj" (2) is building "c:\work\codes\kdmp-parser\src\test\test.vcxproj" (4) on node 1 (default targets).
InitializeBuildStatus:
  Creating "x64\Debug\test.tlog\unsuccessfulbuild" because "AlwaysCreate" was specified.
ClCompile:
  All outputs are up-to-date.
Link:
  All outputs are up-to-date.
  test.vcxproj -> c:\work\codes\kdmp-parser\src\x64\Debug\test.exe
FinalizeBuildStatus:
  Deleting file "x64\Debug\test.tlog\unsuccessfulbuild".
  Touching "x64\Debug\test.tlog\test.lastbuildstate".
Done Building Project "c:\work\codes\kdmp-parser\src\test\test.vcxproj" (default targets).

Done Building Project "c:\work\codes\kdmp-parser\src\test\test.vcxproj.metaproj" (default targets).

Done Building Project "c:\work\codes\kdmp-parser\src\kdmp-parser.sln" (Build target(s)).


Build succeeded.
    0 Warning(s)
    0 Error(s)

Time Elapsed 00:00:00.52
```