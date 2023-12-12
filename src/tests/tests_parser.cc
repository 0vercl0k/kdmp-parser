// Axel '0vercl0k' Souchet - 2023
#define CATCH_CONFIG_MAIN

#include "kdmp-parser.h"
#include <array>
#include <catch2/catch_test_macros.hpp>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <unordered_map>

struct TestCaseValues {
  std::string_view File;
  kdmpparser::DumpType_t Type;
  uint64_t Size{};
  uint64_t ReadAddress{};
  std::array<uint8_t, 16> Bytes{};
  uint64_t Rax{};
  uint64_t Rbx{};
  uint64_t Rcx{};
  uint64_t Rdx{};
  uint64_t Rsi{};
  uint64_t Rdi{};
  uint64_t Rip{};
  uint64_t Rsp{};
  uint64_t Rbp{};
  uint64_t R8{};
  uint64_t R9{};
  uint64_t R10{};
  uint64_t R11{};
  uint64_t R12{};
  uint64_t R13{};
  uint64_t R14{};
  uint64_t R15{};
};

const static TestCaseValues TestCaseBmp{
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
    "bmp.dmp",
    kdmpparser::DumpType_t::BMPDump,
    0x54'4b,
    0x6d'4d'22,
    {0x6d, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x63, 0x88, 0x75, 0x00, 0x00, 0x00,
     0x00, 0x0a, 0x63, 0x98},
    0x00000000'00000003ULL,
    0xfffff805'0f4e9f70ULL,
    0x00000000'00000001ULL,
    0xfffff805'135684d0ULL,
    0x00000000'00000100ULL,
    0xfffff805'0f4e9f80ULL,
    0xfffff805'108776a0ULL,
    0xfffff805'135684f8ULL,
    0xfffff805'13568600ULL,
    0x00000000'00000003ULL,
    0xfffff805'135684b8ULL,
    0x00000000'00000000ULL,
    0xffffa884'8825e000ULL,
    0xfffff805'0f4e9f80ULL,
    0xfffff805'10c3c958ULL,
    0x00000000'00000000ULL,
    0x00000000'00000052ULL,
};
const static TestCaseValues TestCaseFull{
    "full.dmp",
    kdmpparser::DumpType_t::FullDump,
    0x03'fb'e6,
    0x6d'4d'22,
    {0x6d, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x63, 0x88, 0x75, 0x00, 0x00, 0x00,
     0x00, 0x0a, 0x63, 0x98},
    0x00000000'00000003ULL,
    0xfffff805'0f4e9f70ULL,
    0x00000000'00000001ULL,
    0xfffff805'135684d0ULL,
    0x00000000'00000100ULL,
    0xfffff805'0f4e9f80ULL,
    0xfffff805'108776a0ULL,
    0xfffff805'135684f8ULL,
    0xfffff805'13568600ULL,
    0x00000000'00000003ULL,
    0xfffff805'135684b8ULL,
    0x00000000'00000000ULL,
    0xffffa884'8825e000ULL,
    0xfffff805'0f4e9f80ULL,
    0xfffff805'10c3c958ULL,
    0x00000000'00000000ULL,
    0x00000000'00000052ULL,
};
const static TestCaseValues TestCaseKernelDump{
    "kerneldump.dmp",
    kdmpparser::DumpType_t::KernelMemoryDump,
    0xa02e,
    0x25892f0,
    {0x10, 0x8c, 0x24, 0x50, 0x0c, 0xc0, 0xff, 0xff, 0xa0, 0x19, 0x38, 0x51,
     0x0c, 0xc0, 0xff, 0xff},
    0x0000000000007a01ULL,
    0xffffc00c5191e010ULL,
    0x0000000000000001ULL,
    0x0000001200000000ULL,
    0xffffc00c51907bb0ULL,
    0x0000000000000002ULL,
    0xfffff803f2c35470ULL,
    0xfffff803f515ec28ULL,
    0x000000000c1c9800ULL,
    0x00000000000000b0ULL,
    0xffffc00c502ff000ULL,
    0x0000000000000057ULL,
    0xfffff803f3a04500ULL,
    0xfffff803f515ee60ULL,
    0x0000000000000003ULL,
    0xfffff803f1e9a180ULL,
    0x000000000000001fULL,
};
const static TestCaseValues TestCaseKernelUserDump{
    "kerneluserdump.dmp",
    kdmpparser::DumpType_t::KernelAndUserMemoryDump,
    0x1f7c7,
    0x25892f0,
    {0x10, 0x8c, 0x24, 0x50, 0x0c, 0xc0, 0xff, 0xff, 0xa0, 0x19, 0x38, 0x51,
     0x0c, 0xc0, 0xff, 0xff},
    0x0000000000007a01ULL,
    0xffffc00c5191e010ULL,
    0x0000000000000001ULL,
    0x0000001200000000ULL,
    0xffffc00c51907bb0ULL,
    0x0000000000000002ULL,
    0xfffff803f2c35470ULL,
    0xfffff803f515ec28ULL,
    0x000000000c1c9800ULL,
    0x00000000000000b0ULL,
    0xffffc00c502ff000ULL,
    0x0000000000000057ULL,
    0xfffff803f3a04500ULL,
    0xfffff803f515ee60ULL,
    0x0000000000000003ULL,
    0xfffff803f1e9a180ULL,
    0x000000000000001fULL,
};
const static TestCaseValues TestCaseCompleteDump{
    "completedump.dmp",
    kdmpparser::DumpType_t::CompleteMemoryDump,
    0x1fbf9,
    0x25892f0,
    {0x10, 0x8c, 0x24, 0x50, 0x0c, 0xc0, 0xff, 0xff, 0xa0, 0x19, 0x38, 0x51,
     0x0c, 0xc0, 0xff, 0xff},
    0x0000000000007a01ULL,
    0xffffc00c5191e010ULL,
    0x0000000000000001ULL,
    0x0000001200000000ULL,
    0xffffc00c51907bb0ULL,
    0x0000000000000002ULL,
    0xfffff803f2c35470ULL,
    0xfffff803f515ec28ULL,
    0x000000000c1c9800ULL,
    0x00000000000000b0ULL,
    0xffffc00c502ff000ULL,
    0x0000000000000057ULL,
    0xfffff803f3a04500ULL,
    0xfffff803f515ee60ULL,
    0x0000000000000003ULL,
    0xfffff803f1e9a180ULL,
    0x000000000000001fULL,
};

const std::array g_ExpectedValues{
    TestCaseBmp, TestCaseFull,
    TestCaseKernelDump,   TestCaseKernelUserDump,
    TestCaseCompleteDump,
};

TEST_CASE("kdmp-parser", "parser") {
  SECTION("Test minidump exists") {
    for (const auto &TestCase : g_ExpectedValues) {
      REQUIRE(std::filesystem::exists(TestCase.File));
    }
  }

  SECTION("Basic parsing") {
    for (const auto &TestCase : g_ExpectedValues) {
      kdmpparser::KernelDumpParser Dmp;
      REQUIRE(Dmp.Parse(TestCase.File.data()));

      const auto Type = Dmp.GetDumpType();
      const auto &Physmem = Dmp.GetPhysmem();
      const auto &Path = Dmp.GetDumpPath();

      CHECK(Type == TestCase.Type);
      CHECK(Physmem.size() == TestCase.Size);
    }
  }

  SECTION("Context values") {
    for (const auto &TestCase : g_ExpectedValues) {
      kdmpparser::KernelDumpParser Dmp;
      REQUIRE(Dmp.Parse(TestCase.File.data()));
      const auto &Context = Dmp.GetContext();
      CHECK(Context.Rax == TestCase.Rax);
      CHECK(Context.Rbx == TestCase.Rbx);
      CHECK(Context.Rcx == TestCase.Rcx);
      CHECK(Context.Rdx == TestCase.Rdx);
      CHECK(Context.Rsi == TestCase.Rsi);
      CHECK(Context.Rdi == TestCase.Rdi);
      CHECK(Context.Rip == TestCase.Rip);
      CHECK(Context.Rsp == TestCase.Rsp);
      CHECK(Context.Rbp == TestCase.Rbp);
      CHECK(Context.R8 == TestCase.R8);
      CHECK(Context.R9 == TestCase.R9);
      CHECK(Context.R10 == TestCase.R10);
      CHECK(Context.R11 == TestCase.R11);
      CHECK(Context.R12 == TestCase.R12);
      CHECK(Context.R13 == TestCase.R13);
      CHECK(Context.R14 == TestCase.R14);
      CHECK(Context.R15 == TestCase.R15);
    }
  }

  SECTION("Memory access") {
    for (const auto &TestCase : g_ExpectedValues) {
      kdmpparser::KernelDumpParser Dmp;
      REQUIRE(Dmp.Parse(TestCase.File.data()));
      const uint64_t Address = TestCase.ReadAddress;
      const uint64_t AddressAligned = Address & 0xffffffff'fffff000;
      const uint64_t AddressOffset = Address & 0xfff;
      const auto &ExpectedContent = TestCase.Bytes;
      const uint8_t *Page = Dmp.GetPhysicalPage(AddressAligned);
      REQUIRE(Page != nullptr);
      CHECK(memcmp(Page + AddressOffset, ExpectedContent.data(),
                   sizeof(ExpectedContent)) == 0);
    }
  }
}
