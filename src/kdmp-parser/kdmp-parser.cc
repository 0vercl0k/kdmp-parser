// Axel '0vercl0k' Souchet - February 15 2019
#include "kdmp-parser.h"

KernelDumpParser::KernelDumpParser(const TCHAR *PathFile)
    : m_File(nullptr), m_FileMap(nullptr), m_ViewBase(nullptr),
      m_DmpHdr(nullptr), m_PathFile(PathFile) {}

KernelDumpParser::~KernelDumpParser() {

  //
  // Unmap the view of the mapping..
  //

  if (m_ViewBase != nullptr) {
    UnmapViewOfFile(m_ViewBase);
    m_ViewBase = nullptr;
  }

  //
  // Close the handle to the file mapping..
  //

  if (m_FileMap != nullptr) {
    CloseHandle(m_FileMap);
    m_FileMap = nullptr;
  }

  //
  // And finally the file itself.
  //

  if (m_File != nullptr) {
    CloseHandle(m_File);
    m_File = nullptr;
  }
}

bool KernelDumpParser::Parse() {
  bool Success = true;

  //
  // Map a view of the file.
  //

  Success = MapFile();
  if (!Success) {
    _tprintf(_T("MapFile failed.\n"));
    goto clean;
  }

  Success = ParseDmpHeader();
clean:
  return Success;
}

bool KernelDumpParser::ParseDmpHeader() {

  //
  // The base of the view points on the DMP_HEADER64.
  //

  m_DmpHdr = (KDMP_PARSER_HEADER64 *)m_ViewBase;
  m_DmpHdr->Display();

  //
  // Now let's make sure the structures look right.
  //

  if (!m_DmpHdr->LooksGood()) {
    _tprintf(_T("The header looks wrong.\n"));
    return false;
  }

  return true;
}

const KDMP_PARSER_CONTEXT *KernelDumpParser::GetContext() {

  //
  // Give the user a view of the context record.
  //

  return &m_DmpHdr->ContextRecord;
}

bool KernelDumpParser::MapFile() {
  bool Success = true;
  HANDLE File = nullptr;
  HANDLE FileMap = nullptr;
  PVOID ViewBase = nullptr;

  //
  // Open the dump file in read-only.
  //

  File = CreateFile(m_PathFile, GENERIC_READ, FILE_SHARE_READ, nullptr,
                    OPEN_EXISTING, 0, nullptr);

  if (File == NULL) {

    //
    // If we fail to open the file, let the user know.
    //

    const DWORD GLE = GetLastError();
    _tprintf(_T("CreateFile failed with GLE=%d.\n"), GLE);

    if (GLE == ERROR_FILE_NOT_FOUND) {
      _tprintf(_T("  The file %s was not found.\n"), m_PathFile);
    }

    Success = false;
    goto clean;
  }

  //
  // Create the ro file mapping.
  //

  FileMap = CreateFileMapping(File, nullptr, PAGE_READONLY, 0, 0,
                              _T("Kernel crash-dump."));

  if (FileMap == nullptr) {

    //
    // If we fail to create a file mapping, let
    // the user know.
    //

    const DWORD GLE = GetLastError();
    _tprintf(_T("CreateFileMapping failed with GLE=%d.\n"), GLE);
    Success = false;
    goto clean;
  }

  //
  // Map a view of the file in memory.
  //

  ViewBase = MapViewOfFile(FileMap, FILE_MAP_READ, 0, 0, 0);

  if (ViewBase == nullptr) {

    //
    // If we fail to map the view, let the user know.
    //

    const DWORD GLE = GetLastError();
    _tprintf(_T("MapViewOfFile failed with GLE=%d.\n"), GLE);
    Success = false;
    goto clean;
  }

  //
  // Everything went well, so grab a copy of the handles for
  // our class and null-out the temporary variables.
  //

  m_File = File;
  File = nullptr;

  m_FileMap = FileMap;
  FileMap = nullptr;

  m_ViewBase = ViewBase;
  ViewBase = nullptr;

clean:

  //
  // Unmap the view of the mapping..
  //

  if (ViewBase != nullptr) {
    UnmapViewOfFile(ViewBase);
    ViewBase = nullptr;
  }

  //
  // Close the handle to the file mapping..
  //

  if (FileMap != nullptr) {
    CloseHandle(FileMap);
    FileMap = nullptr;
  }

  //
  // And finally the file itself.
  //

  if (File != nullptr) {
    CloseHandle(File);
    File = nullptr;
  }

  return Success;
}

void KernelDumpParser::Runs() {

  //
  // Walk through the runs.
  //

  uint8_t *RunBase = (uint8_t *)m_DmpHdr + 0x2000;
  const uint32_t NumberOfRuns =
      m_DmpHdr->PhysicalMemoryBlockBuffer.NumberOfRuns;

  for (uint32_t RunIdx = 0; RunIdx < NumberOfRuns; RunIdx++) {

    //
    // Grab the current run as well as its base page and page count.
    //

    const KDMP_PARSER_PHYSMEM_RUN *Run =
        &m_DmpHdr->PhysicalMemoryBlockBuffer.Run[RunIdx];
    const uint64_t BasePage = Run->BasePage;
    const uint64_t PageCount = Run->PageCount;

    _tprintf(_T("Run[%02d] - BasePage=%08llx - PageCount=%08llx\n"), RunIdx,
             BasePage, PageCount);
    //
    // Walk the pages from the run.
    //

    for (uint32_t PageIdx = 0, NumberOfPagesDisplayed = 0;

         //
         // We want to display at most 5 pages per run.
         //

         PageIdx < PageCount && NumberOfPagesDisplayed < 5;
         PageIdx++, NumberOfPagesDisplayed++) {

      //
      // Compute the current PFN as well as the actual physical address of the
      // page.
      //

      const uint64_t Pfn = BasePage + PageIdx;
      const uint64_t Pa = Pfn * 0x1000;

      //
      // Now one thing to understand is that the Runs structure allows to skip
      // for holes in memory. Instead of, padding them with empty spaces to
      // conserve a 1:1 mapping between physical address and file offset, the
      // Run gives you the base Pfn. This means that we don't have a 1:1 mapping
      // between file offset and physical addresses so we need to keep track of
      // where the Run starts in memory and then we can simply access our pages
      // one after the other.
      //
      // If this is not clear enough, here is a small example:
      //  Run[0]
      //    BasePage = 1337, PageCount = 2
      //  Run[1]
      //    BasePage = 1400, PageCount = 1
      //
      // In the above we clearly see that there is a hole between the two runs;
      // the dump file has 2+1 memory pages. Their Pfns are: 1337+0, 1337+1,
      // 1400+0.
      //
      // Now if we want to get the file offset of those pages we start at Run0:
      //   Run0 starts at file offset 0x2000 so Page0 is at file offset 0x2000,
      //   Page1 is at file offset 0x3000. Run1 starts at file offset
      //   0x2000+(2*0x1000) so Page3 is at file offset
      //   0x2000+(2*0x1000)+0x1000.
      //
      // That is the reason why the computation below is RunBase + (PageIdx *
      // 0x1000) instead of RunBase + (Pfn * 0x1000).

      const uint8_t *PageBase = RunBase + (PageIdx * 0x1000);

      //
      // Display the PFN and the first 16 bytes.
      //

      _tprintf(_T("0x%08llx: "), Pa);
      for (uint32_t ByteIdx = 0; ByteIdx < 16; ByteIdx++) {
        _tprintf(_T("0x%02x"), PageBase[ByteIdx]);
        if ((ByteIdx + 1) == 16) {
          _tprintf(_T("...\n"));
        } else {
          _tprintf(_T(" "));
        }
      }
    }

    //
    // Make it obvious that we are about to display pages from another
    // run.
    //

    _tprintf(_T("--\n"));

    //
    // Move the run base past all the pages in the current run.
    //

    RunBase += PageCount * 0x1000;
  }
}