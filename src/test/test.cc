// Axel '0vercl0k' Souchet - February 15 2019
#include "kdmp-parser.h"
#include <tchar.h>


int _tmain(int argc, TCHAR *argv[]) {
  if (argc != 2) {
    _tprintf(_T("kdmp-parser.exe <kdump path>\n"));
    return EXIT_FAILURE;
  }

  KernelDumpParser Dmp(argv[1]);
  bool Success = Dmp.Parse();
  if (!Success) {
    return EXIT_FAILURE;
  }

  const KDMP_PARSER_CONTEXT *Context = Dmp.GetContext();
  return EXIT_SUCCESS;
}