#include <Python.h>
#include <cstdio>
#include "GeonlpService.h"

// ref: https://docs.python.org/3/extending/newtypes_tutorial.html

/**
 * Define GeonlpService Object
 */

typedef struct {
  PyObject_HEAD
  geonlp::ServicePtr _ptrObj;
} GeonlpService;

static int geonlp_service_init(GeonlpService *self, PyObject *args, PyObject *kwds)
// Initialization function of the GeoNLP Service object
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


// GeonlpService object methods
static PyMethodDef GeonlpServiceMethods[] = {
  {"version", (PyCFunction)geonlp_service_version, METH_NOARGS, "Show the version"},
  {"proc", (PyCFunction)geonlp_service_proc, METH_VARARGS, "Proc the GeoNLP request"},
  {NULL, NULL, 0, NULL} // Sentinel
};

/**
 * Define GeonlpMA Object
 */

typedef struct {
  PyObject_HEAD
  geonlp::MAPtr _ptrObj;
} GeonlpMA;

static int geonlp_ma_init(GeonlpMA *self, PyObject *args, PyObject *kwds)
// Initializetion function of GeoNLP Morphological Analyser object
{
  char *profile = NULL;

  if (!PyArg_ParseTuple(args, "|s", &profile)) {
    return -1;
  }

  try {
    if (profile != NULL) {
      self->_ptrObj = geonlp::createMA(profile);
    }
    else {
      self->_ptrObj = geonlp::createMA();
    }
  }
  catch (geonlp::ServiceCreateFailedException& e) {
    std::cerr << e.what() << std::endl;
    return -1;
  }

  return 0;
}

static void geonlp_ma_dealloc(GeonlpMA *self)
// Destruct the object
{
  Py_TYPE(self)->tp_free((PyObject*)self);
}

static PyObject * geonlp_ma_parse(GeonlpMA *self, PyObject *args)
// Parse the string
{
  char* str;

  if (!PyArg_ParseTuple(args, "s", &str)) {
    return NULL;
  }

  std::string result = (self->_ptrObj)->parse(str);

  return Py_BuildValue("s", result.c_str(), 1);
}

// GeonlpMA object methods
static PyMethodDef GeonlpMAMethods[] = {
  {"parse", (PyCFunction)geonlp_ma_parse, METH_VARARGS, "Parse the string"},
  {NULL, NULL, 0, NULL} // Sentinel
};

/**
 * Define module
 */

static PyObject * geonlp_module_version(PyObject *self)
// Get the version string
{
  return Py_BuildValue("s", PACKAGE_VERSION, 1);
}

static PyMethodDef GeonlpModuleMethods[] = {
  {"version", (PyCFunction)geonlp_module_version, METH_NOARGS, "Show the version"},
  {NULL, NULL, 0, NULL} // Sentinel
};

static struct PyModuleDef GeonlpModule = {
  PyModuleDef_HEAD_INIT,
  "pygeonlp", // m_name
  NULL,     // m_doc
  -1,       // m_size
  GeonlpModuleMethods, // m_methods
  NULL,     // m_slots
  NULL,     // m_traverse
  NULL,     // m_clear
  NULL      // m_free
};

/**
 * Initialization
 */

PyMODINIT_FUNC
PyInit_pygeonlp (void)
// Create the module
{

  // GeonlpServiceType object
  static PyTypeObject GeonlpServiceType = {
    PyVarObject_HEAD_INIT(NULL, 0)
  };
  GeonlpServiceType.tp_name = "pygeonlp.service";
  GeonlpServiceType.tp_new = PyType_GenericNew;
  GeonlpServiceType.tp_basicsize = sizeof(GeonlpService);
  GeonlpServiceType.tp_dealloc = (destructor) geonlp_service_dealloc;
  GeonlpServiceType.tp_flags = Py_TPFLAGS_DEFAULT;
  GeonlpServiceType.tp_doc = "GeoNLP Service objects";
  GeonlpServiceType.tp_methods = GeonlpServiceMethods;
  GeonlpServiceType.tp_init = (initproc) geonlp_service_init;

  if (PyType_Ready(&GeonlpServiceType) < 0)
    return NULL;

  // GeonlpMAType object
  static PyTypeObject GeonlpMAType = {
    PyVarObject_HEAD_INIT(NULL, 0)
  };
  GeonlpMAType.tp_name = "pygeonlp.ma";
  GeonlpMAType.tp_new = PyType_GenericNew;
  GeonlpMAType.tp_basicsize = sizeof(GeonlpMA);
  GeonlpMAType.tp_dealloc = (destructor) geonlp_ma_dealloc;
  GeonlpMAType.tp_flags = Py_TPFLAGS_DEFAULT;
  GeonlpMAType.tp_doc = "GeoNLP morphological analyzer objects";
  GeonlpMAType.tp_methods = GeonlpMAMethods;
  GeonlpMAType.tp_init = (initproc) geonlp_ma_init;

  if (PyType_Ready(&GeonlpMAType) < 0)
    return NULL;
  
  PyObject* m;  // the module object

  // Create module
  m = PyModule_Create(&GeonlpModule);
  if (m == NULL)
    return NULL;

  // Add GeonlpService Object to the module
  Py_INCREF(&GeonlpServiceType);
  PyModule_AddObject(m, "Service", (PyObject *)&GeonlpServiceType);

  // Add GeonlpMA Object to the module
  Py_INCREF(&GeonlpMAType);
  PyModule_AddObject(m, "MA", (PyObject *)&GeonlpMAType);
  
  return m;
}
