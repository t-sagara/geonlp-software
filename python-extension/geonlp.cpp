#include <Python.h>
#include <cstdio>
#include "GeonlpService.h"

// ref: https://docs.python.org/3/extending/newtypes_tutorial.html

typedef struct {
  PyObject_HEAD
  geonlp::ServicePtr _ptrObj;
} GeonlpService;

static int geonlp_service_init(GeonlpService *self, PyObject *args, PyObject *kwds)
// Initialization function of the GeoNLP object
// see: https://stackoverflow.com/questions/48786693/how-to-wrap-a-c-object-using-pure-python-extension-api-python3
{
  char *profile = NULL;

  if (!PyArg_ParseTuple(args, "|s", &profile)) {
    return -1;
  }
  
  try {
    if (profile != NULL) {
      self->_ptrObj = geonlp::createService(profile);
    }
    else {
      self->_ptrObj = geonlp::createService(); // Use default value
    }
  }
  catch (geonlp::ServiceCreateFailedException& e) {
    std::cerr << e.what() << std::endl;
    return -1;
    // ToDo: should throw exception
  }

  return 0;
}

static void geonlp_service_dealloc(GeonlpService *self)
// Destruct the object
{
  Py_TYPE(self)->tp_free((PyObject*)self);
}

static PyObject * geonlp_service_version(GeonlpService *self)
// Get the version string
{
  std::string version = (self->_ptrObj)->version();
  
  return Py_BuildValue("s", version.c_str(), 1);
}

static PyObject * geonlp_service_proc(GeonlpService *self, PyObject *args)
// Process the geonlp request
{
  char* str;

  if (!PyArg_ParseTuple(args, "s", &str)) {
    return NULL;
  }

  picojson::ext p(str);
  picojson::value response = (self->_ptrObj)->proc(picojson::value(p));
  std::string json_response_str = response.serialize();

  return Py_BuildValue("s", json_response_str.c_str(), 1);
    
}

// Define methods
static PyMethodDef GeonlpMethods[] = {
  {"version", (PyCFunction)geonlp_service_version, METH_NOARGS, "Show the version"},
  {"proc", (PyCFunction)geonlp_service_proc, METH_VARARGS, "Proc the GeoNLP request"},
  {NULL, NULL, 0, NULL} // Sentinel
};

// Define module
static struct PyModuleDef GeonlpModule = {
  PyModuleDef_HEAD_INIT,
  "geonlp", // m_name
  NULL,     // m_doc
  -1,       // m_size
  NULL,     // m_methods
  NULL,     // m_slots
  NULL,     // m_traverse
  NULL,     // m_clear
  NULL      // m_free
};

static PyTypeObject GeonlpServiceType = {
  PyVarObject_HEAD_INIT(NULL, 0)
};

PyMODINIT_FUNC
PyInit_geonlp (void)
// Create the module
{
  PyObject* m;

  GeonlpServiceType.tp_name = "geonlp.service";
  GeonlpServiceType.tp_new = PyType_GenericNew;
  GeonlpServiceType.tp_basicsize = sizeof(GeonlpService);
  GeonlpServiceType.tp_dealloc = (destructor) geonlp_service_dealloc;
  GeonlpServiceType.tp_flags = Py_TPFLAGS_DEFAULT;
  GeonlpServiceType.tp_doc = "GeoNLP Service objects";
  GeonlpServiceType.tp_methods = GeonlpMethods;
  GeonlpServiceType.tp_init = (initproc) geonlp_service_init;

  if (PyType_Ready(&GeonlpServiceType) < 0)
    return NULL;

  m = PyModule_Create(&GeonlpModule);
  if (m == NULL)
    return NULL;

  Py_INCREF(&GeonlpServiceType);
  PyModule_AddObject(m, "GeonlpService", (PyObject *)&GeonlpServiceType);
  return m;
  // return PyModule_Create(&GeonlpModule);
}
