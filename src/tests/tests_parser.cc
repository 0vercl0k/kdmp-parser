// Axel '0vercl0k' Souchet - 2023
#define CATCH_CONFIG_MAIN

#include "catch.hpp"

#include "kdmp-parser.h"
#include <array>
#include <cstring>
#include <filesystem>

//
// TEST_DATA_DIR must be defined to a string which corresponds to a path to the
// dump files
//
#ifndef TEST_DATA_DIR
#error Missing dump folder
#endif

const static std::filesystem::path g_TestDataDir(TEST_DATA_DIR);
const static std::array<std::filesystem::path, 2> g_TestDataFiles = {{
    g_TestDataDir / "full.dmp",
    g_TestDataDir / "bmp.dmp",
}};

TEST_CASE("kdmp-parser", "parser") {
  SECTION("Test minidump exists") {
    for (auto const &file : g_TestDataFiles) {
      CHECK(std::filesystem::exists(file));
    }
  }

  SECTION("Basic parsing") {
    for (auto const &file : g_TestDataFiles) {
      kdmpparser::KernelDumpParser Dmp{};
      CHECK(Dmp.Parse(file.string().c_str()));

      const kdmpparser::DumpType_t Type = Dmp.GetDumpType();
      const auto &Physmem = Dmp.GetPhysmem();
      if (file.filename() == "bmp.dmp") {
        CHECK(Type == kdmpparser::DumpType_t::BMPDump);
        CHECK(Physmem.size() == 0x544b);
      } else if (file.filename() == "full.dmp") {
        CHECK(Type == kdmpparser::DumpType_t::FullDump);
        CHECK(Physmem.size() == 0x3fbe6);
      } else {
        CHECK(false);
      }
    }
  }

  //
  // kd> r
  // rax=0000000000000003 rbx=fffff8050f4e9f70 rcx=0000000000000001
  // rdx=fffff805135684d0 rsi=0000000000000100 rdi=fffff8050f4e9f80
  // rip=fffff805108776a0 rsp=fffff805135684f8 rbp=fffff80513568600
  // r8=0000000000000003  r9=fffff805135684b8 r10=0000000000000000
  // r11=ffffa8848825e000 r12=fffff8050f4e9f80 r13=fffff80510c3c958
  // r14=0000000000000000 r15=0000000000000052
  // iopl=0         nv up ei pl nz na pe nc
  // cs=0010  ss=0018  ds=002b  es=002b  fs=0053  gs=002b efl=00040202
  //

  SECTION("Context values") {
    for (auto const &file : g_TestDataFiles) {
      kdmpparser::KernelDumpParser Dmp{};
      CHECK(Dmp.Parse(file.string().c_str()));
      const auto Context = Dmp.GetContext();
      CHECK(Context.Rax == 0x0000000000000003ULL);
      CHECK(Context.Rbx == 0xfffff8050f4e9f70ULL);
      CHECK(Context.Rcx == 0x0000000000000001ULL);
      CHECK(Context.Rdx == 0xfffff805135684d0ULL);
      CHECK(Context.Rsi == 0x0000000000000100ULL);
      CHECK(Context.Rdi == 0xfffff8050f4e9f80ULL);
      CHECK(Context.Rip == 0xfffff805108776a0ULL);
      CHECK(Context.Rsp == 0xfffff805135684f8ULL);
      CHECK(Context.Rbp == 0xfffff80513568600ULL);
      CHECK(Context.R8 == 0x0000000000000003ULL);
      CHECK(Context.R9 == 0xfffff805135684b8ULL);
      CHECK(Context.R10 == 0x0000000000000000ULL);
      CHECK(Context.R11 == 0xffffa8848825e000ULL);
      CHECK(Context.R12 == 0xfffff8050f4e9f80ULL);
      CHECK(Context.R13 == 0xfffff80510c3c958ULL);
      CHECK(Context.R14 == 0x0000000000000000ULL);
      CHECK(Context.R15 == 0x0000000000000052ULL);
    }
  }

  SECTION("Memory access") {
    for (auto const &file : g_TestDataFiles) {
      kdmpparser::KernelDumpParser Dmp{};
      CHECK(Dmp.Parse(file.string().c_str()));
      const uint64_t Address = 0x6d4d22;
      const uint64_t AddressAligned = Address & 0xfffffffffffff000;
      const uint64_t AddressOffset = Address & 0xfff;
      const uint8_t ExpectedContent[] = {0x6d, 0x00, 0x00, 0x00, 0x00, 0x0a,
                                         0x63, 0x88, 0x75, 0x00, 0x00, 0x00,
                                         0x00, 0x0a, 0x63, 0x98};
      const uint8_t *Page = Dmp.GetPhysicalPage(AddressAligned);
      CHECK(Page != nullptr);
      CHECK(::memcmp(Page + AddressOffset, ExpectedContent,
                     sizeof(ExpectedContent)) == 0);
    }
  }
}
