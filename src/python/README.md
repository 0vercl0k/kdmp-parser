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

## Python 3 bindings

The bindings allow you to: read the context, read physical memory and to do virtual memory translations:

```py
from kdmp_parser import Dump, FullDump, BMPDump

dmp = Dump(sys.argv[2])
assert(dmp.type() == FullDump or dmp.type() == BMPDump)

ctx = dmp.context()
dtb = ctx['dtb'] & ~0xfff # remove PCID

assert(ctx['rip'] == 0xfffff805108776a0)
assert(dtb == 0x6d4000)

page = dmp.get_physical_page(0x5000)
assert(page[0x34:0x38] == b'MSFT')

assert(dmp.virt_translate(0xfffff78000000000) == 0x0000000000c2f000)
assert(dmp.virt_translate(0xfffff80513370000) == 0x000000003d555000)

assert(dmp.get_virtual_page(0xfffff78000000000) == dmp.get_physical_page(0x0000000000c2f000))
assert(dmp.get_virtual_page(0xfffff80513370000) == dmp.get_physical_page(0x000000003d555000))

v = 0xfffff80513568000
assert(dmp.get_virtual_page(v) == dmp.get_physical_page(dmp.virt_translate(v)))
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
