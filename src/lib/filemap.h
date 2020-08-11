// Axel '0vercl0k' Souchet - April 28 2020
#include "platform.h"
#include <cstdio>

#if defined(WINDOWS)
class FileMap {
  //
  // Handle to the input file.
  //

  HANDLE File_;

  //
  // Handle to the file mapping.
  //

  HANDLE FileMap_;

  //
  // Base address of the file view.
  //

  PVOID ViewBase_;

public:
  FileMap() : File_(nullptr), FileMap_(nullptr), ViewBase_(nullptr) {}

  ~FileMap() {
    //
    // Unmap the view of the mapping..
    //

    if (ViewBase_ != nullptr) {
      UnmapViewOfFile(ViewBase_);
      ViewBase_ = nullptr;
    }

    //
    // Close the handle to the file mapping..
    //

    if (FileMap_ != nullptr) {
      CloseHandle(FileMap_);
      FileMap_ = nullptr;
    }

    //
    // And finally the file itself.
    //

    if (File_ != nullptr) {
      CloseHandle(File_);
      File_ = nullptr;
    }
  }

  void *ViewBase() { return ViewBase_; }

  bool MapFile(const char *PathFile) {
    bool Success = true;
    HANDLE File = nullptr;
    HANDLE FileMap = nullptr;
    PVOID ViewBase = nullptr;

    //
    // Open the dump file in read-only.
    //

    File = CreateFileA(PathFile, GENERIC_READ, FILE_SHARE_READ, nullptr,
                       OPEN_EXISTING, 0, nullptr);

    if (File == NULL) {

      //
      // If we fail to open the file, let the user know.
      //

      const DWORD GLE = GetLastError();
      printf("CreateFile failed with GLE=%lu.\n", GLE);

      if (GLE == ERROR_FILE_NOT_FOUND) {
        printf("  The file %s was not found.\n", PathFile);
      }

      Success = false;
      goto clean;
    }

    //
    // Create the ro file mapping.
    //

    FileMap = CreateFileMappingA(File, nullptr, PAGE_READONLY, 0, 0, nullptr);

    if (FileMap == nullptr) {

      //
      // If we fail to create a file mapping, let
      // the user know.
      //

      const DWORD GLE = GetLastError();
      printf("CreateFileMapping failed with GLE=%lu.\n", GLE);
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
      printf("MapViewOfFile failed with GLE=%lu.\n", GLE);
      Success = false;
      goto clean;
    }

    //
    // Everything went well, so grab a copy of the handles for
    // our class and null-out the temporary variables.
    //

    File_ = File;
    File = nullptr;

    FileMap_ = FileMap;
    FileMap = nullptr;

    ViewBase_ = ViewBase;
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
};

#elif defined(LINUX)

#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

class FileMap {
  void *ViewBase_;
  off_t ViewSize_;
  int Fd_;

public:
  FileMap() : ViewBase_(nullptr), ViewSize_(0), Fd_(-1) {}

  ~FileMap() {
    if (ViewBase_) {
      munmap(ViewBase_, ViewSize_);
      ViewBase_ = nullptr;
      ViewSize_ = 0;
    }

    if (Fd_ != -1) {
      close(Fd_);
      Fd_ = -1;
    }
  }

  void *ViewBase() { return ViewBase_; }

  bool MapFile(const char *PathFile) {
    Fd_ = open(PathFile, O_RDONLY);
    if (Fd_ < 0) {
      perror("Could not open dump file.\n");
      return false;
    }

    struct stat Stat;
    if (fstat(Fd_, &Stat) < 0) {
      perror("Could not stat dump file.\n");
      return false;
    }

    ViewSize_ = Stat.st_size;
    ViewBase_ = mmap(nullptr, ViewSize_, PROT_READ, MAP_SHARED, Fd_, 0);
    return ViewBase_ != MAP_FAILED;
  }
};

#endif