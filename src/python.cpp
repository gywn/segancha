#include <Python.h>
#include <color.h>
#include <fitness.h>
#include <iostream>

//< RAII wrapper for PyObject*
class PyObj {
public:
  PyObj() = delete;
  PyObj(PyObject *ptr) : ptr(ptr){};
  PyObj(const PyObj &obj) : ptr(obj.ptr) { Py_XINCREF(obj.ptr); };
  PyObj(PyObj &&obj) : ptr(obj.ptr) { obj.ptr = nullptr; };
  PyObj &operator=(const PyObj &obj) {
    ptr = obj.ptr;
    Py_XINCREF(ptr);
    return *this;
  };
  PyObj &operator=(PyObj &&obj) {
    ptr = obj.ptr;
    obj.ptr = nullptr;
    return *this;
  };
  ~PyObj() { Py_XDECREF(ptr); };

  PyObject *ptr;
};

static PyObject *LABtoRGB(PyObject *self, PyObject *args) {
  color::LAB lab;

  if (!PyArg_ParseTuple(args, "(ddd)", &lab.l, &lab.a, &lab.b))
    return nullptr;

  const auto rgb = color::LABtoRGB(lab);
  return Py_BuildValue("(ddd)", rgb.r, rgb.g, rgb.b);
}

static PyObject *pyperception(PyObject *self, PyObject *args, PyObject *kw) {
  // color::LAB fg, bg;
  unsigned long M;
  double L = -1;
  PyObject *fixed_pyo = nullptr;
  bool quiet = false;

  static char kw1[] = "M";
  static char kw2[] = "L";
  static char kw3[] = "fixed";
  static char kw4[] = "quiet";
  static char *kwlist[] = {kw1, kw2, kw3, kw4};
  if (!PyArg_ParseTupleAndKeywords(args, kw, "k|dOp", kwlist, &M, &L,
                                   &fixed_pyo, &quiet))
    return nullptr;

  std::vector<color::LAB> fixed;
  if (fixed_pyo && fixed_pyo != Py_None) {
    PyObj fixed_seq = PySequence_Fast(fixed_pyo, "expected a sequence of LAB");
    if (!fixed_seq.ptr)
      return nullptr;
    auto len = PySequence_Fast_GET_SIZE(fixed_seq.ptr);
    for (size_t i = 0; i < len; ++i) {
      PyObj lab_seq =
          PySequence_Fast(PySequence_Fast_GET_ITEM(fixed_seq.ptr, i),
                          "expect a tuple of (L, a, b)");
      if (!lab_seq.ptr)
        return nullptr;
      auto lab_len = PySequence_Fast_GET_SIZE(lab_seq.ptr);
      if (lab_len != 3) {
        PyErr_SetString(PyExc_TypeError, "expect a tuple of (L, a, b)");
        return nullptr;
      }
      fixed.push_back(color::LAB{
          PyFloat_AsDouble(PySequence_Fast_GET_ITEM(lab_seq.ptr, 0)),
          PyFloat_AsDouble(PySequence_Fast_GET_ITEM(lab_seq.ptr, 1)),
          PyFloat_AsDouble(PySequence_Fast_GET_ITEM(lab_seq.ptr, 2))});
      if (PyErr_Occurred())
        return nullptr;
    }
  }
  if (!quiet) {
    for (const auto &lab : fixed)
      std::cout << lab << " ";
    std::cout << std::endl;
  }

  const auto result = perception(M, L, &fixed, quiet);
  PyObj rgb = PyObj(PyList_New(0));
  for (const auto &c : result.rgb)
    PyList_Append(rgb.ptr, PyObj(Py_BuildValue("(ddd)", c.r, c.g, c.b)).ptr);
  PyObj fitness = PyObj(PyList_New(0));
  for (const auto d : result.fitness.prd)
    PyList_Append(fitness.ptr, PyObj(PyFloat_FromDouble(d)).ptr);

  return Py_BuildValue("{sksdsOsO}", "flags", result.flags, "L", result.L,
                       "rgb", rgb.ptr, "fitness", fitness.ptr);
}

static PyMethodDef methods[] = {
    {"perception", (PyCFunction)pyperception, METH_VARARGS | METH_KEYWORDS,
     "perception colors anchored by N colors\n"
     "--------------------------------------\n"
     "\n"
     "@param M number of colors\n"
     "@param L=-1 luminocity, < 0 for optimized value\n"
     "@param fixed=None iterable of (l, a, b)\n"
     "@param quiet=False print messages to stdout"},
    {"LABtoRGB", LABtoRGB, METH_VARARGS,
     "convert LAB to RGB\n"
     "------------------\n"
     "\n"
     "@param lab (l,a,b)\n"},
    {nullptr, nullptr, 0, nullptr}};

static struct PyModuleDef module = {
    PyModuleDef_HEAD_INIT, "pyperception",
    "Python interface for perception colors library", -1, methods};

PyMODINIT_FUNC PyInit_pyperception(void) { return PyModule_Create(&module); }