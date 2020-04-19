// Axel '0vercl0k' Souchet - February 15 2019
#pragma once

#include "kdmp-parser-structs.h"
#include <cstdint>
#include <windows.h>

#include <unordered_map>

using Physmem_t = std::unordered_map<uint64_t, const uint8_t *>;

class KernelDumpParser {
public:
  KernelDumpParser();
  ~KernelDumpParser();

  //
  // Actually do the parsing of the file.
  //

  bool Parse(const char *PathFile);

  //
  // Give the Context record to the user.
  //

  const KDMP_PARSER_CONTEXT *GetContext();

  //
  // Get the physmem.
  //

  const Physmem_t &GetPhysmem();

  //
  // Show the exception record.
  //

  void ShowExceptionRecord(const uint32_t Prefix) const;

  //
  // Show the context record.
  //

  void ShowContextRecord(const uint32_t Prefix) const;

  //
  // Show all the structures of the dump.
  //

  void ShowAllStructures(const uint32_t Prefix) const;

  //
  // Get the content of a physical address.
  //

  const uint8_t *GetPhysicalPage(const uint64_t PhysicalAddress) const;

private:
  //
  // Build a map of physical addresses / page data pointers for full dump.
  //

  bool BuildPhysmemFullDump();

  //
  // Build a map of physical addresses / page data pointers for BMP dump.
  //

  bool BuildPhysmemBMPDump();

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

  HANDLE File_;

  //
  // Handle to the file mapping.
  //

  HANDLE FileMap_;

  //
  // Base address of the file view.
  //

  PVOID ViewBase_;

  //
  // Header of the crash-dump.
  //

  KDMP_PARSER_HEADER64 *DmpHdr_;

  //
  // File path to the crash-dump.
  //

  const char *PathFile_;

  //
  // Mapping between physical addresses / page data.
  //

  Physmem_t Physmem_;
};