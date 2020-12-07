// Mastho - 2020
#define PY_SSIZE_T_CLEAN

#include "kdmp-parser.h"
#include <Python.h>

//
// Python object handling all interactions with the library.
//

typedef struct {
  PyObject_HEAD kdmpparser::KernelDumpParser *DumpParser;
} PythonDumpParser;

//
// Python Dump type functions declarations (class instance creation and instance
// destruction).
//

PyObject *NewDumpParser(PyTypeObject *Type, PyObject *Args, PyObject *Kwds);
void DeleteDumpParser(PyObject *Object);

//
// Python Dump object methods functions declarations.
//

PyObject *DumpParserGetType(PyObject *Object, PyObject *NotUsed);
PyObject *DumpParserGetContext(PyObject *Object, PyObject *NotUsed);
PyObject *DumpParserGetPhysicalPage(PyObject *Object, PyObject *Args);
PyObject *DumpParserVirtTranslate(PyObject *Object, PyObject *Args);
PyObject *DumpParserGetVirtualPage(PyObject *Object, PyObject *Args);
PyObject *DumpParserGetBugCheckParameters(PyObject *Object, PyObject *NotUsed);

//
// Object methods of Python Dump type.
//

PyMethodDef DumpObjectMethod[] = {
    {"type", DumpParserGetType, METH_NOARGS,
     "Show Dump Type (FullDump, KernelDump)"},
    {"context", DumpParserGetContext, METH_NOARGS, "Get Register Context"},
    {"get_physical_page", DumpParserGetPhysicalPage, METH_VARARGS,
     "Get Physical Page Content"},
    {"virt_translate", DumpParserVirtTranslate, METH_VARARGS,
     "Translate Virtual to Physical Address"},
    {"get_virtual_page", DumpParserGetVirtualPage, METH_VARARGS,
     "Get Virtual Page Content"},
    {"bugcheck", DumpParserGetBugCheckParameters, METH_NOARGS,
     "Get BugCheck Parameters"},
    {nullptr, nullptr, 0, nullptr}};

//
// Define PythonDumpParserType (size, name, initialization & destruction
// functions and object methods).
//

static PyTypeObject PythonDumpParserType = []() {
  PyTypeObject Ty = {
      PyVarObject_HEAD_INIT(&PyType_Type, 0) "kdmp.Dump", /* tp_name */
  };
  Ty.tp_basicsize = sizeof(PythonDumpParser);
  Ty.tp_dealloc = DeleteDumpParser;
  Ty.tp_flags = Py_TPFLAGS_DEFAULT;
  Ty.tp_doc = "Dump object";
  Ty.tp_methods = DumpObjectMethod;
  Ty.tp_new = NewDumpParser;
  return Ty;
}();

//
// KDMP Module definition.
//

static struct PyModuleDef KDMPModule = {
    PyModuleDef_HEAD_INIT, /* m_base */
    "kdmp",                /* m_name */
    nullptr,               /* m_doc */
    -1,                    /* m_size */
    nullptr,               /* m_methods */
    nullptr,               /* m_slots */
    nullptr,               /* m_traverse */
    nullptr,               /* m_clear */
    nullptr,               /* m_free */
};

//
// KDMP Module initialization function.
//

PyMODINIT_FUNC PyInit_kdmp(void);