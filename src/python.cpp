#include <Python.h>
#include <color.h>
#include <fitness.h>
#include <iostream>

//< RAII wrapper for PyObject
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

static PyObject *perception(PyObject *self, PyObject *args) {
  color::LAB fg, bg;
  unsigned long M;
  bool quiet;

  if (!PyArg_ParseTuple(args, "(ddd)(ddd)k|p", &fg.l, &fg.a, &fg.b, &bg.l,
                        &bg.a, &bg.b, &M, &quiet))
    return NULL;

  const auto result = perceptionL(fg, bg, M, quiet);
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
    {"perception", perception, METH_VARARGS,
     "perception colors\n-----------------\n\n@param foreground "
     "(l,a,b)\n@param background (l,a,b)\n@param M number of colors\n@param "
     "quiet default:False"},
    {NULL, NULL, 0, NULL}};

static struct PyModuleDef module = {
    PyModuleDef_HEAD_INIT, "pyperception",
    "Python interface for perception colors library", -1, methods};

PyMODINIT_FUNC PyInit_pyperception(void) { return PyModule_Create(&module); }