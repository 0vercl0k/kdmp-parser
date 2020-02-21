// Axel '0vercl0k' Souchet - February 15 2019
#include "kdmp-parser.h"
#include <tchar.h>

//
// Delimiter.
//

#define DELIMITER                                                              \
  _T("----------------------------------------------------------------------") \
  _T("----------")

//
// The options available for the parser.
//

struct Options_t {

  //
  // This is enabled if -c is used.
  //

  bool ShowContextRecord;

  //
  // This is enabled if -a is used.
  //

  bool ShowAllStructures;

  //
  // This is enabled if -e is used.
  //

  bool ShowExceptionRecord;

  //
  // This is enable if -p is used.
  //

  bool ShowPhysicalMem;

  //
  // This is on if the user specified a physical address.
  //

  bool HasPhysicalAddress;

  //
  // If an optional physical address has been passed to -p then this is the
  // physical address.

  uint64_t PhysicalAddress;

  //
  // The path to the dump file.
  //

  TCHAR *DumpPath;

  //
  // Initialize all teh things!
  //

  Options_t()
      : ShowContextRecord(false), ShowPhysicalMem(false),
        ShowAllStructures(false), ShowExceptionRecord(false),
        HasPhysicalAddress(0), PhysicalAddress(0), DumpPath(nullptr) {}
};

//
// Display usage
//

void Usage() {
  _tprintf(_T("parser.exe [-p [<physical address>]] [-c] [-e] <kdump path>\n"));
  _tprintf(_T("\n"));
  _tprintf(_T("Examples:\n"));
  _tprintf(_T("  Show every structures of the dump:\n"));
  _tprintf(_T("    parser.exe -a full.dmp\n"));
  _tprintf(_T("\n"));
  _tprintf(_T("  Show the context record:\n"));
  _tprintf(_T("    parser.exe -c full.dmp\n"));
  _tprintf(_T("\n"));
  _tprintf(_T("  Show the exception record:\n"));
  _tprintf(_T("    parser.exe -e full.dmp\n"));
  _tprintf(_T("\n"));
  _tprintf(_T("  Show all the physical memory (first 16 bytes of every ")
           _T("pages):\n"));
  _tprintf(_T("    parser.exe -p full.dmp\n"));
  _tprintf(_T("\n"));
  _tprintf(_T("  Show the context record as well as the page at physical ")
           _T("address 0x1000:\n"));
  _tprintf(_T("    parser.exe -c -p 0x1000 full.dmp\n"));
}

//
// Copied from https://github.com/pvachon/tsl/blob/master/tsl/hexdump.c.
// Phil is the man.
//

void Hexdump(const uint64_t Address, const void *Buffer, size_t Len) {
  const uint8_t *ptr = (uint8_t *)Buffer;

  for (size_t i = 0; i < Len; i += 16) {
    _tprintf(_T("%08llx: "), Address + i);
    for (int j = 0; j < 16; j++) {
      if (i + j < Len) {
        _tprintf(_T("%02x "), ptr[i + j]);
      } else {
        _tprintf(_T("   "));
      }
    }
    _tprintf(_T(" |"));
    for (int j = 0; j < 16; j++) {
      if (i + j < Len) {
        _tprintf(_T("%c"), isprint(ptr[i + j]) ? (char)ptr[i + j] : '.');
      } else {
        _tprintf(_T(" "));
      }
    }
    _tprintf(_T("|\n"));
  }
}

//
// Let's do some work!
//

int _tmain(int argc, TCHAR *argv[]) {

  //
  // This holds the options passed to the program.
  //

  Options_t Opts;

  //
  // Parse the arguments passed to the program.
  //

  for (int ArgIdx = 1; ArgIdx < argc; ArgIdx++) {
    TCHAR *Arg = argv[ArgIdx];
    const int IsLastArg = (ArgIdx + 1) >= argc;

    if (_tcscmp(Arg, _T("-c")) == 0) {

      //
      // Show the context record.
      //

      Opts.ShowContextRecord = 1;
    } else if (_tcscmp(Arg, _T("-p")) == 0) {

      //
      // Show the physical memory.
      //

      Opts.ShowPhysicalMem = 1;

      //
      // If the next argument is not the last one, we assume that it is followed
      // by a physical address.
      //

      const int NextArgIdx = ArgIdx + 1;
      const bool IsNextArgLast = (NextArgIdx + 1) >= argc;

      if (!IsNextArgLast) {

        //
        // In which case we convert it to an actual integer.
        //

        Opts.HasPhysicalAddress = true;
        Opts.PhysicalAddress = _tcstoull(argv[NextArgIdx], nullptr, 0);

        //
        // Skip the next argument.
        //

        ArgIdx++;
      }
    } else if (_tcscmp(Arg, _T("-e")) == 0) {

      //
      // Show the exception record.
      //

      Opts.ShowExceptionRecord = 1;
    } else if (_tcscmp(Arg, _T("-a")) == 0) {

      //
      // Show all the structures.
      //

      Opts.ShowAllStructures = true;
    } else if (IsLastArg) {

      //
      // If this is the last argument then this must be the dump path.
      //

      Opts.DumpPath = Arg;
    } else {

      //
      // Otherwise it seems that the user passed something wrong?
      //

      _tprintf(_T("The argument %s is not recognized.\n\n"), Arg);
      Usage();
      return EXIT_FAILURE;
    }
  }

  //
  // The only thing we actually need is a file path. So let's make sure we
  // have one.
  //

  if (!Opts.DumpPath) {
    _tprintf(_T("You didn't provide the path to the dump file.\n\n"));
    Usage();
    return EXIT_FAILURE;
  }

  //
  // If we only have a path, at least force to dump the context
  // structure.
  //

  if (!Opts.ShowContextRecord && !Opts.ShowPhysicalMem &&
      !Opts.ShowAllStructures && !Opts.ShowExceptionRecord) {
    _tprintf(_T("Forcing to show the context record as no option as been ")
             _T("passed.\n\n"));
    Opts.ShowContextRecord = 1;
  }

  //
  // Create the parser instance.
  //

  KernelDumpParser Dmp(Opts.DumpPath);

  //
  // Parse the dump file.
  //

  bool Success = Dmp.Parse();
  if (!Success) {
    _tprintf(_T("Parsing of the dump failed, exiting.\n"));
    return EXIT_FAILURE;
  }

  //
  // If the user wants all the structures, then show them.
  //

  if (Opts.ShowAllStructures) {
    _tprintf(DELIMITER _T("\nDump structures:\n"));
    Dmp.ShowAllStructures(2);
  }

  //
  // If the user wants the context, then show it.
  //

  if (Opts.ShowContextRecord) {
    _tprintf(DELIMITER _T("\nContext Record:\n"));
    Dmp.ShowContextRecord(2);
  }

  //
  // If the user wants the exception record, then show it.
  //

  if (Opts.ShowExceptionRecord) {
    _tprintf(DELIMITER _T("\nException Record:\n"));
    Dmp.ShowExceptionRecord(2);
  }

  //
  // If the user wants some physical memory, then show it.
  //

  if (Opts.ShowPhysicalMem) {
    _tprintf(DELIMITER _T("\nPhysical memory:\n"));

    //
    // If the user specified a physical address this is the one we
    // will dump.
    //

    if (Opts.PhysicalAddress) {

      //
      // Retrieve the page for the specified PhysicalAddress.
      // If it doesn't exist then display a message, else dump it on stdout.
      //

      const uint8_t *Page = Dmp.GetPhysicalAddress(Opts.PhysicalAddress);
      if (Page == nullptr) {
        _tprintf(_T("0x%llx is not a valid physical address.\n"),
                 Opts.PhysicalAddress);
      } else {
        Hexdump(Opts.PhysicalAddress, Page, 0x1000);
      }
    } else {

      //
      // If the user didn't specify a physical address then dump the first
      // 16 bytes of every physical pages.
      //

      for (const auto [PhysicalAddress, Page] : Dmp.GetPhysmem()) {
        Hexdump(PhysicalAddress, Page, 16);
      }
    }
  }

  return EXIT_SUCCESS;
}