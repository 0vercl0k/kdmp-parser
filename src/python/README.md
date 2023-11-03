# Python building for `kdmp-parser`

![Build status](https://github.com/0vercl0k/kdmp-parser/workflows/Builds/badge.svg)

This C++ library parses Windows kernel [full](https://docs.microsoft.com/en-us/windows-hardware/drivers/debugger/complete-memory-dump) dumps (`.dump /f` in WinDbg) as well as [BMP](https://docs.microsoft.com/en-us/windows-hardware/drivers/debugger/active-memory-dump) dumps (`.dump /ka` in WinDbg).

![parser](pics/parser.jpg)

The library supports loading 64-bit dumps and provides read access to things like:

- The context record,
- The exception record,
- The bugcheck parameters,
- The physical memory.

Compiled binaries are available in the [releases](https://github.com/0vercl0k/kdmp-parser/releases) section.

Special thanks to:
- [yrp604](https://github.com/yrp604) for being knowledgeable about the format,
- the [rekall](https://github.com/google/rekall) project and their [Python implementation](https://github.com/google/rekall/blob/master/rekall-core/rekall/plugins/overlays/windows/crashdump.py) (most of the structures in [kdmp-parser-structs.h](https://github.com/0vercl0k/kdmp-parser/blob/master/src/kdmp-parser/kdmp-parser-structs.h) have been adapted from it).

## Examples

Some test dump files can be found in `kdmp-parser-testdatas/`

### Get context, print the Program Counter

```py
import kdmp_parser
dmp = kdmp_parser.KernelDumpParser("./kdmp-parser-testdatas/full.dmp")
assert dmp.type == kdmp_parser.DumpType.FullDump
ctx = dmp.context
print(f"Dump RIP={ctx.Rip:#x}")
```

### Read a virtual memory page at address pointed by RAX

```python
import kdmp_parser
dmp = kdmp_parser.KernelDumpParser("./kdmp-parser-testdatas/full.dmp")
dmp.read_virtual_page( ctx.Rax )
```

### Explore the physical memory

```python
import kdmp_parser
dmp = kdmp_parser.KernelDumpParser("./kdmp-parser-testdatas/full.dmp")
pml4 = dmp.directory_table_base
print(f"{pml=:#x}")
dmp.read_physical_page(pml4)
```

### Resolve a virtual address to a physical one

```python
import kdmp_parser
dmp = kdmp_parser.KernelDumpParser("./kdmp-parser-testdatas/full.dmp")
VA = dmp.Rip
PA = dmp.translate_virtual(VA)
print(f"{VA=:#x} -> {PA=:#x}")
```

## Install from PIP

Simply run:

```bash
python -m pip install kdmp_parser
```

## Build from source

### Requirements

 * [CMake](https://cmake.org/)
 * [Python](https://python.org/) 3.8+ and `pip`
 * (Linux) Python development package

### Build

Run:

```bash
python -m pip install -r src/python/requirements.txt
python -m pip install src/python
```


# Authors

* Axel '[@0vercl0k](https://twitter.com/0vercl0k)' Souchet

With contributions from:
  - [@masthoon](https://github.com/masthoon)
  - [@hugsy](https://github.com/hugsy)
