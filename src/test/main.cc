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
  _tprintf(_T("rax=%016llx rbx=%016llx rcx=%016llx\n"), Context->Rax,
           Context->Rbx, Context->Rcx);
  _tprintf(_T("rdx=%016llx rsi=%016llx rdi=%016llx\n"), Context->Rdx,
           Context->Rsi, Context->Rdi);
  _tprintf(_T("rip=%016llx rsp=%016llx rbp=%016llx\n"), Context->Rip,
           Context->Rsp, Context->Rbp);
  _tprintf(_T(" r8=%016llx  r9=%016llx r10=%016llx\n"), Context->R8,
           Context->R9, Context->R10);
  _tprintf(_T("r11=%016llx r12=%016llx r13=%016llx\n"), Context->R11,
           Context->R12, Context->R13);
  _tprintf(_T("r14=%016llx r15=%016llx\n"), Context->R14, Context->R15);
  _tprintf(_T("cs=%04x ss=%04x ds=%04x es=%04x fs=%04x gs=%04x    ")
           _T("         efl=%08x\n"),
           Context->SegCs, Context->SegSs, Context->SegDs, Context->SegEs,
           Context->SegFs, Context->SegGs, Context->EFlags);

  _tprintf(_T("-----\nPhysical memory:\n"));

  for (const auto [Pa, Page] : Dmp.GetPhysmem()) {
    _tprintf(_T("0x%08llx: "), Pa);
    for (uint32_t ByteIdx = 0; ByteIdx < 16; ByteIdx++) {
      _tprintf(_T("0x%02x"), Page[ByteIdx]);
      if ((ByteIdx + 1) == 16) {
        _tprintf(_T("...\n"));
      } else {
        _tprintf(_T(" "));
      }
    }
  }

  return EXIT_SUCCESS;
}