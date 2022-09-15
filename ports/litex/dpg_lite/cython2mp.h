#include <string.h>
extern "C"
{
#include "py/obj.h"
#include "py/objstr.h"
#include "py/gc.h"
#include "py/objtuple.h"
}

#define malloc(x) gc_alloc(x, false)
#define free(x) gc_free(x)
typedef void(*PyCFunction)(...);
typedef PyCFunction PyCFunctionWithKeywords;
typedef struct PyMethodDef { const char *name; PyCFunction f; int flags, dummy; };
#define METH_NOARGS 1
#define METH_VARARGS 2
#define METH_KEYWORDS 4
#define METH_O 8

#define CYTHON_UNUSED
#define CYTHON_INLINE inline
#define __PYX_ERR(...) {}
#define unlikely(x) (x)
#define likely(x) (x)
#define __Pyx_GOTREF(...)
#define __Pyx_GIVEREF(...)
#define __Pyx_XDECREF(...)
#define __Pyx_XGIVEREF(...)
#define __Pyx_INCREF(x)
#define __Pyx_DECREF(x)
#define __Pyx_AddTraceback(...)
#define __Pyx_RefNannySetupContext(fn, i)
#define __Pyx_RefNannyFinishContext()
#define __Pyx_RefNannyDeclarations
#define __Pyx_TraceDeclarations
#define __Pyx_TraceFrameInit(x) {}
#define __Pyx_TraceCall(...)
#define __Pyx_TraceLine(...) {}
#define __Pyx_TraceReturn(...)
#define __Pyx_CppExn2PyErr()
#define PyErr_Occurred() false
#define PyErr_SetString(...)
#define CYTHON_FALLTHROUGH
typedef size_t Py_ssize_t;
#define PyDict_Size(x) mp_obj_dict_len(x)
#define __Pyx_PyDict_GetItemStr(k, s)  mp_map_lookup((mp_map_t*)k, s, MP_MAP_LOOKUP)->value //mp_obj_dict_get(k, s) //map ok if used just to get kw args
#define PyTypeObject void
#define Py_buffer void
struct __Pyx_memviewslice {};

#define Py_None mp_const_none
#define Py_True mp_const_true
#define Py_False mp_const_false

#define try if(true)
#define catch(...) else

#define PyObject void //mp_obj_t is void *
#define __Pyx_ArgTypeTest(...) true
#warning implement__Pyx_ArgTypeTest

#define __Pyx_RaiseArgtupleInvalid(...)
#warning implement __Pyx_RaiseArgtupleInvalid

#define __Pyx_PyBool_FromLong(x) mp_obj_new_bool(x)
#define __Pyx_PyInt_From_int(x) MP_OBJ_NEW_SMALL_INT(x)

#define PyTuple_New(n) mp_obj_new_tuple(n, NULL)
#define PyTuple_SET_ITEM(t, i, v) ((mp_obj_tuple_t *)t)->items[i]=(v)
#define PyTuple_GET_ITEM(t, i) ((mp_obj_tuple_t *)t)->items[i]
#define PyTuple_GET_SIZE(t) ((mp_obj_tuple_t *)t)->len

static inline const char *__Pyx_PyBytes_AsString(mp_obj_t value)
{
  GET_STR_DATA_LEN(value, s_value, value_len);
  return (const char *) s_value;
}

static inline char *__Pyx_PyBytes_AsWritableString(mp_obj_t value)
{
  GET_STR_DATA_LEN(value, s_value, value_len);
  return (char *) s_value;
}

static inline mp_obj_t __Pyx_PyBytes_FromString(const char *str)
{
  return mp_obj_new_str(str, strlen(str));
}

#define PyObject_IsTrue(v) mp_obj_is_true(v)

