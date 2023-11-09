//
// This file is part of kdmp-parser project
//
// Released under MIT License, by 0vercl0k - 2020-2023
//
// With contributions from:
//  * hugsy - (github.com/hugsy)
//

#include "kdmp-parser.h"

#include <nanobind/nanobind.h>
#include <nanobind/stl/array.h>
#include <nanobind/stl/filesystem.h>
#include <nanobind/stl/optional.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/unordered_map.h>
#include <nanobind/stl/variant.h>

namespace nb = nanobind;
using namespace nb::literals;

NB_MODULE(_kdmp_parser, m) {

  m.doc() = "KDMP parser module";

  nb::class_<kdmpparser::Version>(m, "version")
      .def_ro_static("major", &kdmpparser::Version::Major)
      .def_ro_static("minor", &kdmpparser::Version::Minor)
      .def_ro_static("patch", &kdmpparser::Version::Patch)
      .def_ro_static("release", &kdmpparser::Version::Release);

  nb::class_<kdmpparser::uint128_t>(m, "uint128_t")
      .def(nb::init<>())
      .def_rw("Low", &kdmpparser::uint128_t::Low)
      .def_rw("High", &kdmpparser::uint128_t::High);

  nb::enum_<kdmpparser::DumpType_t>(m, "DumpType_t")
      .value("FullDump", kdmpparser::DumpType_t::FullDump)
      .value("KernelDump", kdmpparser::DumpType_t::KernelDump)
      .value("BMPDump", kdmpparser::DumpType_t::BMPDump)
      .export_values();

  nb::class_<kdmpparser::PHYSMEM_RUN>(m, "PHYSMEM_RUN")
      .def(nb::init<>())
      .def_rw("BasePage", &kdmpparser::PHYSMEM_RUN::BasePage)
      .def_rw("PageCount", &kdmpparser::PHYSMEM_RUN::PageCount)
      .def("Show", &kdmpparser::PHYSMEM_RUN::Show, "Prefix"_a);

  nb::class_<kdmpparser::PHYSMEM_DESC>(m, "PHYSMEM_DESC")
      .def(nb::init<>())
      .def_ro("NumberOfRuns", &kdmpparser::PHYSMEM_DESC::NumberOfRuns)
      .def_ro("Padding0", &kdmpparser::PHYSMEM_DESC::Padding0)
      .def_ro("NumberOfPages", &kdmpparser::PHYSMEM_DESC::NumberOfPages)
      .def_ro("Run", &kdmpparser::PHYSMEM_DESC::Run)
      .def("Show", &kdmpparser::PHYSMEM_DESC::Show, "Prefix"_a)
      .def("LooksGood", &kdmpparser::PHYSMEM_DESC::LooksGood);

  nb::class_<kdmpparser::BMP_HEADER64>(m, "BMP_HEADER64")
      .def(nb::init<>())
      .def_ro_static("ExpectedSignature",
                     &kdmpparser::BMP_HEADER64::ExpectedSignature)
      .def_ro_static("ExpectedSignature2",
                     &kdmpparser::BMP_HEADER64::ExpectedSignature2)
      .def_ro_static("ExpectedValidDump",
                     &kdmpparser::BMP_HEADER64::ExpectedValidDump)
      .def_ro("Signature", &kdmpparser::BMP_HEADER64::Signature)
      .def_ro("ValidDump", &kdmpparser::BMP_HEADER64::ValidDump)
      .def_ro("Padding0", &kdmpparser::BMP_HEADER64::Padding0)
      .def_ro("FirstPage", &kdmpparser::BMP_HEADER64::FirstPage)
      .def_ro("TotalPresentPages", &kdmpparser::BMP_HEADER64::TotalPresentPages)
      .def_ro("Pages", &kdmpparser::BMP_HEADER64::Pages)
      .def_ro("Bitmap", &kdmpparser::BMP_HEADER64::Bitmap)
      .def("Show", &kdmpparser::PHYSMEM_DESC::Show, "Prefix"_a)
      .def("LooksGood", &kdmpparser::PHYSMEM_DESC::LooksGood);

  nb::class_<kdmpparser::CONTEXT>(m, "CONTEXT")
      .def(nb::init<>())
      .def_ro("P1Home", &kdmpparser::CONTEXT::P1Home)
      .def_ro("P2Home", &kdmpparser::CONTEXT::P2Home)
      .def_ro("P3Home", &kdmpparser::CONTEXT::P3Home)
      .def_ro("P4Home", &kdmpparser::CONTEXT::P4Home)
      .def_ro("P5Home", &kdmpparser::CONTEXT::P5Home)
      .def_ro("P6Home", &kdmpparser::CONTEXT::P6Home)
      .def_ro("ContextFlags", &kdmpparser::CONTEXT::ContextFlags)
      .def_ro("MxCsr", &kdmpparser::CONTEXT::MxCsr)
      .def_ro("SegCs", &kdmpparser::CONTEXT::SegCs)
      .def_ro("SegDs", &kdmpparser::CONTEXT::SegDs)
      .def_ro("SegEs", &kdmpparser::CONTEXT::SegEs)
      .def_ro("SegFs", &kdmpparser::CONTEXT::SegFs)
      .def_ro("SegGs", &kdmpparser::CONTEXT::SegGs)
      .def_ro("SegSs", &kdmpparser::CONTEXT::SegSs)
      .def_ro("EFlags", &kdmpparser::CONTEXT::EFlags)
      .def_ro("Dr0", &kdmpparser::CONTEXT::Dr0)
      .def_ro("Dr1", &kdmpparser::CONTEXT::Dr1)
      .def_ro("Dr2", &kdmpparser::CONTEXT::Dr2)
      .def_ro("Dr3", &kdmpparser::CONTEXT::Dr3)
      .def_ro("Dr6", &kdmpparser::CONTEXT::Dr6)
      .def_ro("Dr7", &kdmpparser::CONTEXT::Dr7)
      .def_ro("Rax", &kdmpparser::CONTEXT::Rax)
      .def_ro("Rcx", &kdmpparser::CONTEXT::Rcx)
      .def_ro("Rdx", &kdmpparser::CONTEXT::Rdx)
      .def_ro("Rbx", &kdmpparser::CONTEXT::Rbx)
      .def_ro("Rsp", &kdmpparser::CONTEXT::Rsp)
      .def_ro("Rbp", &kdmpparser::CONTEXT::Rbp)
      .def_ro("Rsi", &kdmpparser::CONTEXT::Rsi)
      .def_ro("Rdi", &kdmpparser::CONTEXT::Rdi)
      .def_ro("R8", &kdmpparser::CONTEXT::R8)
      .def_ro("R9", &kdmpparser::CONTEXT::R9)
      .def_ro("R10", &kdmpparser::CONTEXT::R10)
      .def_ro("R11", &kdmpparser::CONTEXT::R11)
      .def_ro("R12", &kdmpparser::CONTEXT::R12)
      .def_ro("R13", &kdmpparser::CONTEXT::R13)
      .def_ro("R14", &kdmpparser::CONTEXT::R14)
      .def_ro("R15", &kdmpparser::CONTEXT::R15)
      .def_ro("Rip", &kdmpparser::CONTEXT::Rip)
      .def_ro("ControlWord", &kdmpparser::CONTEXT::ControlWord)
      .def_ro("StatusWord", &kdmpparser::CONTEXT::StatusWord)
      .def_ro("TagWord", &kdmpparser::CONTEXT::TagWord)
      .def_ro("Reserved1", &kdmpparser::CONTEXT::Reserved1)
      .def_ro("ErrorOpcode", &kdmpparser::CONTEXT::ErrorOpcode)
      .def_ro("ErrorOffset", &kdmpparser::CONTEXT::ErrorOffset)
      .def_ro("ErrorSelector", &kdmpparser::CONTEXT::ErrorSelector)
      .def_ro("Reserved2", &kdmpparser::CONTEXT::Reserved2)
      .def_ro("DataOffset", &kdmpparser::CONTEXT::DataOffset)
      .def_ro("DataSelector", &kdmpparser::CONTEXT::DataSelector)
      .def_ro("Reserved3", &kdmpparser::CONTEXT::Reserved3)
      .def_ro("MxCsr2", &kdmpparser::CONTEXT::MxCsr2)
      .def_ro("MxCsr_Mask", &kdmpparser::CONTEXT::MxCsr_Mask)
      .def_ro("FloatRegisters", &kdmpparser::CONTEXT::FloatRegisters)
      .def_ro("Xmm0", &kdmpparser::CONTEXT::Xmm0)
      .def_ro("Xmm1", &kdmpparser::CONTEXT::Xmm1)
      .def_ro("Xmm2", &kdmpparser::CONTEXT::Xmm2)
      .def_ro("Xmm3", &kdmpparser::CONTEXT::Xmm3)
      .def_ro("Xmm4", &kdmpparser::CONTEXT::Xmm4)
      .def_ro("Xmm5", &kdmpparser::CONTEXT::Xmm5)
      .def_ro("Xmm6", &kdmpparser::CONTEXT::Xmm6)
      .def_ro("Xmm7", &kdmpparser::CONTEXT::Xmm7)
      .def_ro("Xmm8", &kdmpparser::CONTEXT::Xmm8)
      .def_ro("Xmm9", &kdmpparser::CONTEXT::Xmm9)
      .def_ro("Xmm10", &kdmpparser::CONTEXT::Xmm10)
      .def_ro("Xmm11", &kdmpparser::CONTEXT::Xmm11)
      .def_ro("Xmm12", &kdmpparser::CONTEXT::Xmm12)
      .def_ro("Xmm13", &kdmpparser::CONTEXT::Xmm13)
      .def_ro("Xmm14", &kdmpparser::CONTEXT::Xmm14)
      .def_ro("Xmm15", &kdmpparser::CONTEXT::Xmm15)
      .def_ro("VectorRegister", &kdmpparser::CONTEXT::VectorRegister)
      .def_ro("VectorControl", &kdmpparser::CONTEXT::VectorControl)
      .def_ro("DebugControl", &kdmpparser::CONTEXT::DebugControl)
      .def_ro("LastBranchToRip", &kdmpparser::CONTEXT::LastBranchToRip)
      .def_ro("LastBranchFromRip", &kdmpparser::CONTEXT::LastBranchFromRip)
      .def_ro("LastExceptionToRip", &kdmpparser::CONTEXT::LastExceptionToRip)
      .def_ro("LastExceptionFromRip",
              &kdmpparser::CONTEXT::LastExceptionFromRip)
      .def("Show", &kdmpparser::CONTEXT::Show, "Prefix"_a)
      .def("LooksGood", &kdmpparser::CONTEXT::LooksGood);

  nb::class_<kdmpparser::EXCEPTION_RECORD64>(m, "EXCEPTION_RECORD64")
      .def(nb::init<>())
      .def_ro("ExceptionCode", &kdmpparser::EXCEPTION_RECORD64::ExceptionCode)
      .def_ro("ExceptionFlags", &kdmpparser::EXCEPTION_RECORD64::ExceptionFlags)
      .def_ro("ExceptionRecord",
              &kdmpparser::EXCEPTION_RECORD64::ExceptionRecord)
      .def_ro("ExceptionAddress",
              &kdmpparser::EXCEPTION_RECORD64::ExceptionAddress)
      .def_ro("NumberParameters",
              &kdmpparser::EXCEPTION_RECORD64::NumberParameters)
      .def_ro("__unusedAlignment",
              &kdmpparser::EXCEPTION_RECORD64::__unusedAlignment)
      .def_ro("ExceptionInformation",
              &kdmpparser::EXCEPTION_RECORD64::ExceptionInformation)
      .def("Show", &kdmpparser::EXCEPTION_RECORD64::Show, "Prefix"_a);

  nb::class_<kdmpparser::HEADER64>(m, "HEADER64")
      .def(nb::init<>())
      .def_ro_static("ExpectedSignature",
                     &kdmpparser::HEADER64::ExpectedSignature)
      .def_ro_static("ExpectedValidDump",
                     &kdmpparser::HEADER64::ExpectedValidDump)

      .def_ro("Signature", &kdmpparser::HEADER64::Signature)
      .def_ro("ValidDump", &kdmpparser::HEADER64::ValidDump)
      .def_ro("MajorVersion", &kdmpparser::HEADER64::MajorVersion)
      .def_ro("MinorVersion", &kdmpparser::HEADER64::MinorVersion)
      .def_ro("DirectoryTableBase", &kdmpparser::HEADER64::DirectoryTableBase)
      .def_ro("PfnDatabase", &kdmpparser::HEADER64::PfnDatabase)
      .def_ro("PsLoadedModuleList", &kdmpparser::HEADER64::PsLoadedModuleList)
      .def_ro("PsActiveProcessHead", &kdmpparser::HEADER64::PsActiveProcessHead)
      .def_ro("MachineImageType", &kdmpparser::HEADER64::MachineImageType)
      .def_ro("NumberProcessors", &kdmpparser::HEADER64::NumberProcessors)
      .def_ro("BugCheckCode", &kdmpparser::HEADER64::BugCheckCode)
      .def_ro("BugCheckCodeParameter",
              &kdmpparser::HEADER64::BugCheckCodeParameters)
      .def_ro("KdDebuggerDataBlock", &kdmpparser::HEADER64::KdDebuggerDataBlock)
      .def_prop_ro(
          "PhysicalMemoryBlock",
          [](kdmpparser::HEADER64 const &hdr) -> kdmpparser::PHYSMEM_DESC {
            return hdr.u1.PhysicalMemoryBlock;
          })
      .def_prop_ro(
          "ContextRecord",
          [](kdmpparser::HEADER64 const &hdr) { return hdr.u2.ContextRecord; })
      .def_ro("Exception", &kdmpparser::HEADER64::Exception)
      .def_ro("DumpType", &kdmpparser::HEADER64::DumpType)
      .def_ro("RequiredDumpSpace", &kdmpparser::HEADER64::RequiredDumpSpace)
      .def_ro("SystemTime", &kdmpparser::HEADER64::SystemTime)
      .def_ro("Comment", &kdmpparser::HEADER64::Comment)
      .def_ro("SystemUpTime", &kdmpparser::HEADER64::SystemUpTime)
      .def_ro("MiniDumpFields", &kdmpparser::HEADER64::MiniDumpFields)
      .def_ro("SecondaryDataState", &kdmpparser::HEADER64::SecondaryDataState)
      .def_ro("ProductType", &kdmpparser::HEADER64::ProductType)
      .def_ro("SuiteMask", &kdmpparser::HEADER64::SuiteMask)
      .def_ro("WriterStatus", &kdmpparser::HEADER64::WriterStatus)
      .def_ro("KdSecondaryVersion", &kdmpparser::HEADER64::KdSecondaryVersion)
      .def_ro("Attributes", &kdmpparser::HEADER64::Attributes)
      .def_ro("BootId", &kdmpparser::HEADER64::BootId)
      .def_ro("BmpHeader", &kdmpparser::HEADER64::BmpHeader)
      .def("Show", &kdmpparser::CONTEXT::Show, "Prefix"_a)
      .def("LooksGood", &kdmpparser::CONTEXT::LooksGood);

  m.attr("PageSize") = kdmpparser::Page::Size;
  m.def("PageAlign", &kdmpparser::Page::Align, "Address"_a,
        "Get the aligned value on the page for the given address.");
  m.def("PageOffset", &kdmpparser::Page::Offset, "Address"_a,
        "Get the offset to the page for the given address.");

  nb::class_<kdmpparser::BugCheckParameters_t>(m, "BugCheckParameters_t")
      .def(nb::init<>())
      .def_ro("BugCheckCode", &kdmpparser::BugCheckParameters_t::BugCheckCode)
      .def_ro("BugCheckCodeParameter;",
              &kdmpparser::BugCheckParameters_t::BugCheckCodeParameter);

  nb::class_<kdmpparser::KernelDumpParser>(m, "KernelDumpParser")
      .def(nb::init<>())
      .def("Parse", &kdmpparser::KernelDumpParser::Parse, "PathFile"_a)
      .def("GetContext", &kdmpparser::KernelDumpParser::GetContext)
      .def("GetBugCheckParameters",
           &kdmpparser::KernelDumpParser::GetBugCheckParameters)
      .def("GetDumpType", &kdmpparser::KernelDumpParser::GetDumpType)
      .def("ShowExceptionRecord",
           &kdmpparser::KernelDumpParser::ShowExceptionRecord, "Prefix"_a = 0)
      .def("ShowContextRecord",
           &kdmpparser::KernelDumpParser::ShowContextRecord, "Prefix"_a = 0)
      .def("ShowAllStructures",
           &kdmpparser::KernelDumpParser::ShowAllStructures, "Prefix"_a = 0)
      .def(
          "GetPhysicalPage",
          [](kdmpparser::KernelDumpParser const &x,
             uint64_t PhysicalAddress) -> std::optional<kdmpparser::Page_t> {
            auto ptr = x.GetPhysicalPage(PhysicalAddress);
            if (!ptr)
              return std::nullopt;
            kdmpparser::Page_t out{};
            ::memcpy(out.data(), ptr, kdmpparser::Page::Size);
            return out;
          },
          "PhysicalAddress"_a)
      .def("GetDirectoryTableBase",
           &kdmpparser::KernelDumpParser::GetDirectoryTableBase)
      .def("VirtTranslate", &kdmpparser::KernelDumpParser::VirtTranslate,
           "VirtualAddress"_a, "DirectoryTableBase"_a)
      .def(
          "GetVirtualPage",
          [](kdmpparser::KernelDumpParser const &x, uint64_t VirtualAddress,
             uint64_t DirectoryTableBase =
                 0) -> std::optional<kdmpparser::Page_t> {
            auto ptr = x.GetVirtualPage(VirtualAddress, DirectoryTableBase);
            if (!ptr)
              return std::nullopt;
            kdmpparser::Page_t out;
            ::memcpy(out.data(), ptr, kdmpparser::Page::Size);
            return out;
          },
          "VirtualAddress"_a, "DirectoryTableBase"_a = 0);
}
