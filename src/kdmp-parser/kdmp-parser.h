// Axel '0vercl0k' Souchet - February 15 2019
#pragma once
#ifndef _WIN64
#error Only support is for Win 64bits.
#endif

#include "kdmp-parser-structs.h"
#include <cstdint>
#include <tchar.h>
#include <windows.h>

class KernelDumpParser {
public:
  KernelDumpParser(const TCHAR *PathFile);
  ~KernelDumpParser();

  //
  // Actually do the parsing of the file.
  //

  bool Parse();

  void Runs();

  //
  // Give a view of Context record to the user.
  //

  const KDMP_PARSER_CONTEXT *GetContext();

private:
  //
  // Parse the DMP_HEADER.
  //

  bool ParseDmpHeader();

  //
  // Map a view of the file in memory.
  //

  bool MapFile();

  //
  // Handle to the input file.
  //

  HANDLE m_File;

  //
  // Handle to the file mapping.
  //

  HANDLE m_FileMap;

  //
  // Base address of the file view.
  //

  PVOID m_ViewBase;

  //
  // Header of the crash-dump.
  //

  KDMP_PARSER_HEADER64 *m_DmpHdr;

  //
  // File path to the crash-dump.
  //

  const TCHAR *m_PathFile;
};