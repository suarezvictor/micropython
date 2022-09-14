#include <stdio.h>
#include "cython2mp.h"
#include "imgui.h"

#define __pyx_f_4core__from_bytes(x) (x)
#define __pyx_f_4core__bytes(x) (x)

static CYTHON_INLINE int __Pyx_PyObject_IsTrue(PyObject* x) {
   int is_true = x == Py_True;
   if (is_true | (x == Py_False) | (x == Py_None)) return is_true;
   else return PyObject_IsTrue(x);
}

extern "C" PyObject *__pyx_pf_4core_32begin(CYTHON_UNUSED PyObject *__pyx_self, PyObject *__pyx_v_label, PyObject *__pyx_v_closable, ImGuiWindowFlags __pyx_v_flags) {
  bool __pyx_v_opened;
  PyObject *__pyx_r = NULL;
  __Pyx_RefNannyDeclarations
  PyObject *__pyx_t_1 = NULL;
  char const *__pyx_t_2;
  bool *__pyx_t_3;
  int __pyx_t_4;
  bool __pyx_t_5;
  PyObject *__pyx_t_6 = NULL;
  PyObject *__pyx_t_7 = NULL;
  int __pyx_lineno = 0;
  const char *__pyx_filename = NULL;
  int __pyx_clineno = 0;
  __Pyx_RefNannySetupContext("begin", 0);

  /* "core.pyx":2131
 *         )
 *     """
 *     cdef cimgui.bool opened = True             # <<<<<<<<<<<<<<
 * 
 *     return cimgui.Begin(_bytes(label), &opened if closable else NULL, flags), opened
 */
  __pyx_v_opened = 1;

  /* "core.pyx":2133
 *     cdef cimgui.bool opened = True
 * 
 *     return cimgui.Begin(_bytes(label), &opened if closable else NULL, flags), opened             # <<<<<<<<<<<<<<
 * 
 * 
 */
  __Pyx_XDECREF(__pyx_r);
  __pyx_t_1 = __pyx_f_4core__bytes(__pyx_v_label); if (unlikely(!__pyx_t_1)) __PYX_ERR(0, 2133, __pyx_L1_error)
  __Pyx_GOTREF(__pyx_t_1);
  if (unlikely(__pyx_t_1 == Py_None)) {
    PyErr_SetString(PyExc_TypeError, "expected bytes, NoneType found");
    __PYX_ERR(0, 2133, __pyx_L1_error)
  }
  __pyx_t_2 = __Pyx_PyBytes_AsString(__pyx_t_1); if (unlikely((!__pyx_t_2) && PyErr_Occurred())) __PYX_ERR(0, 2133, __pyx_L1_error)
  __pyx_t_4 = __Pyx_PyObject_IsTrue(__pyx_v_closable); if (unlikely(__pyx_t_4 < 0)) __PYX_ERR(0, 2133, __pyx_L1_error)
  if (__pyx_t_4) {
    __pyx_t_3 = (&__pyx_v_opened);
  } else {
    __pyx_t_3 = NULL;
  }
  try {
    __pyx_t_5 = ImGui::Begin(__pyx_t_2, __pyx_t_3, __pyx_v_flags);
  } catch(...) {
    __Pyx_CppExn2PyErr();
    __PYX_ERR(0, 2133, __pyx_L1_error)
  }
  __Pyx_DECREF(__pyx_t_1); __pyx_t_1 = 0;
  __pyx_t_1 = __Pyx_PyBool_FromLong(__pyx_t_5); if (unlikely(!__pyx_t_1)) __PYX_ERR(0, 2133, __pyx_L1_error)
  __Pyx_GOTREF(__pyx_t_1);
  __pyx_t_6 = __Pyx_PyBool_FromLong(__pyx_v_opened); if (unlikely(!__pyx_t_6)) __PYX_ERR(0, 2133, __pyx_L1_error)
  __Pyx_GOTREF(__pyx_t_6);
  __pyx_t_7 = PyTuple_New(2); if (unlikely(!__pyx_t_7)) __PYX_ERR(0, 2133, __pyx_L1_error)
  __Pyx_GOTREF(__pyx_t_7);
  __Pyx_GIVEREF(__pyx_t_1);
  PyTuple_SET_ITEM(__pyx_t_7, 0, __pyx_t_1);
  __Pyx_GIVEREF(__pyx_t_6);
  PyTuple_SET_ITEM(__pyx_t_7, 1, __pyx_t_6);
  __pyx_t_1 = 0;
  __pyx_t_6 = 0;
  __pyx_r = __pyx_t_7;
  __pyx_t_7 = 0;
  goto __pyx_L0;

  /* "core.pyx":2103
 * 
 * 
 * def begin(str label, closable=False, cimgui.ImGuiWindowFlags flags=0):             # <<<<<<<<<<<<<<
 *     """Begin a window.
 * 
 */

  /* function exit code */
  __pyx_L1_error:;
  __Pyx_XDECREF(__pyx_t_1);
  __Pyx_XDECREF(__pyx_t_6);
  __Pyx_XDECREF(__pyx_t_7);
  __Pyx_AddTraceback("core.begin", __pyx_clineno, __pyx_lineno, __pyx_filename);
  __pyx_r = NULL;
  __pyx_L0:;
  __Pyx_XGIVEREF(__pyx_r);
  __Pyx_RefNannyFinishContext();
  return __pyx_r;
}

extern "C" PyObject *__pyx_pf_4core_36end(CYTHON_UNUSED PyObject *__pyx_self) {
  PyObject *__pyx_r = NULL;
  __Pyx_RefNannyDeclarations
  int __pyx_lineno = 0;
  const char *__pyx_filename = NULL;
  int __pyx_clineno = 0;
  __Pyx_RefNannySetupContext("end", 0);

  /* "core.pyx":2160
 *         void End()
 *     """
 *     cimgui.End()             # <<<<<<<<<<<<<<
 * 
 * 
 */
  try {
    ImGui::End();
  } catch(...) {
    __Pyx_CppExn2PyErr();
    __PYX_ERR(0, 2160, __pyx_L1_error)
  }

  /* "core.pyx":2151
 * 
 * 
 * def end():             # <<<<<<<<<<<<<<
 *     """End a window.
 * 
 */

  /* function exit code */
  __pyx_r = Py_None; __Pyx_INCREF(Py_None);
  goto __pyx_L0;
  __pyx_L1_error:;
  __Pyx_AddTraceback("core.end", __pyx_clineno, __pyx_lineno, __pyx_filename);
  __pyx_r = NULL;
  __pyx_L0:;
  __Pyx_XGIVEREF(__pyx_r);
  __Pyx_RefNannyFinishContext();
  return __pyx_r;
}
extern "C" PyObject *__pyx_pf_4core_218input_text(CYTHON_UNUSED PyObject *__pyx_self, PyObject *__pyx_v_label, PyObject *__pyx_v_value, int __pyx_v_buffer_length, ImGuiInputTextFlags __pyx_v_flags) {
  char *__pyx_v_inout_text;
  bool __pyx_v_changed;
  PyObject *__pyx_v_output = NULL;
  PyObject *__pyx_r = NULL;
  __Pyx_RefNannyDeclarations
  PyObject *__pyx_t_1 = NULL;
  char const *__pyx_t_2;
  char const *__pyx_t_3;
  bool __pyx_t_4;
  PyObject *__pyx_t_5 = NULL;
  int __pyx_lineno = 0;
  const char *__pyx_filename = NULL;
  int __pyx_clineno = 0;
  __Pyx_RefNannySetupContext("input_text", 0);

  /* "core.pyx":4949
 *     """
 *     # todo: pymalloc
 *     cdef char* inout_text = <char*>malloc(buffer_length * sizeof(char))             # <<<<<<<<<<<<<<
 *     # todo: take special care of terminating char
 *     strncpy(inout_text, _bytes(value), buffer_length)
 */
  __pyx_v_inout_text = ((char *)malloc((__pyx_v_buffer_length * (sizeof(char)))));

  /* "core.pyx":4951
 *     cdef char* inout_text = <char*>malloc(buffer_length * sizeof(char))
 *     # todo: take special care of terminating char
 *     strncpy(inout_text, _bytes(value), buffer_length)             # <<<<<<<<<<<<<<
 * 
 *     changed = cimgui.InputText(
 */
  __pyx_t_1 = __pyx_f_4core__bytes(__pyx_v_value); if (unlikely(!__pyx_t_1)) __PYX_ERR(0, 4951, __pyx_L1_error)
  __Pyx_GOTREF(__pyx_t_1);
  if (unlikely(__pyx_t_1 == Py_None)) {
    PyErr_SetString(PyExc_TypeError, "expected bytes, NoneType found");
    __PYX_ERR(0, 4951, __pyx_L1_error)
  }
  __pyx_t_2 = __Pyx_PyBytes_AsString(__pyx_t_1); if (unlikely((!__pyx_t_2) && PyErr_Occurred())) __PYX_ERR(0, 4951, __pyx_L1_error)
  (void)(strncpy(__pyx_v_inout_text, __pyx_t_2, __pyx_v_buffer_length));
  __Pyx_DECREF(__pyx_t_1); __pyx_t_1 = 0;

  /* "core.pyx":4954
 * 
 *     changed = cimgui.InputText(
 *         _bytes(label), inout_text, buffer_length, flags, NULL, NULL             # <<<<<<<<<<<<<<
 *     )
 *     output = _from_bytes(inout_text)
 */
  __pyx_t_1 = __pyx_f_4core__bytes(__pyx_v_label); if (unlikely(!__pyx_t_1)) __PYX_ERR(0, 4954, __pyx_L1_error)
  __Pyx_GOTREF(__pyx_t_1);
  if (unlikely(__pyx_t_1 == Py_None)) {
    PyErr_SetString(PyExc_TypeError, "expected bytes, NoneType found");
    __PYX_ERR(0, 4954, __pyx_L1_error)
  }
  __pyx_t_3 = __Pyx_PyBytes_AsString(__pyx_t_1); if (unlikely((!__pyx_t_3) && PyErr_Occurred())) __PYX_ERR(0, 4954, __pyx_L1_error)

  /* "core.pyx":4953
 *     strncpy(inout_text, _bytes(value), buffer_length)
 * 
 *     changed = cimgui.InputText(             # <<<<<<<<<<<<<<
 *         _bytes(label), inout_text, buffer_length, flags, NULL, NULL
 *     )
 */
  try {
    __pyx_t_4 = ImGui::InputText(__pyx_t_3, __pyx_v_inout_text, __pyx_v_buffer_length, __pyx_v_flags, NULL, NULL);
  } catch(...) {
    __Pyx_CppExn2PyErr();
    __PYX_ERR(0, 4953, __pyx_L1_error)
  }
  __Pyx_DECREF(__pyx_t_1); __pyx_t_1 = 0;
  __pyx_v_changed = __pyx_t_4;

  /* "core.pyx":4956
 *         _bytes(label), inout_text, buffer_length, flags, NULL, NULL
 *     )
 *     output = _from_bytes(inout_text)             # <<<<<<<<<<<<<<
 * 
 *     free(inout_text)
 */
  __pyx_t_1 = __Pyx_PyBytes_FromString(__pyx_v_inout_text); if (unlikely(!__pyx_t_1)) __PYX_ERR(0, 4956, __pyx_L1_error)
  __Pyx_GOTREF(__pyx_t_1);
  __pyx_t_5 = __pyx_f_4core__from_bytes(((PyObject*)__pyx_t_1)); if (unlikely(!__pyx_t_5)) __PYX_ERR(0, 4956, __pyx_L1_error)
  __Pyx_GOTREF(__pyx_t_5);
  __Pyx_DECREF(__pyx_t_1); __pyx_t_1 = 0;
  __pyx_v_output = __pyx_t_5;
  __pyx_t_5 = 0;

  /* "core.pyx":4958
 *     output = _from_bytes(inout_text)
 * 
 *     free(inout_text)             # <<<<<<<<<<<<<<
 *     return changed, output
 * 
 */
  free(__pyx_v_inout_text);

  /* "core.pyx":4959
 * 
 *     free(inout_text)
 *     return changed, output             # <<<<<<<<<<<<<<
 * 
 * 
 */
  __Pyx_XDECREF(__pyx_r);
  __pyx_t_5 = __Pyx_PyBool_FromLong(__pyx_v_changed); if (unlikely(!__pyx_t_5)) __PYX_ERR(0, 4959, __pyx_L1_error)
  __Pyx_GOTREF(__pyx_t_5);
  __pyx_t_1 = PyTuple_New(2); if (unlikely(!__pyx_t_1)) __PYX_ERR(0, 4959, __pyx_L1_error)
  __Pyx_GOTREF(__pyx_t_1);
  __Pyx_GIVEREF(__pyx_t_5);
  PyTuple_SET_ITEM(__pyx_t_1, 0, __pyx_t_5);
  __Pyx_INCREF(__pyx_v_output);
  __Pyx_GIVEREF(__pyx_v_output);
  PyTuple_SET_ITEM(__pyx_t_1, 1, __pyx_v_output);
  __pyx_t_5 = 0;
  __pyx_r = __pyx_t_1;
  __pyx_t_1 = 0;
  goto __pyx_L0;

  /* "core.pyx":4900
 * 
 * 
 * def input_text(             # <<<<<<<<<<<<<<
 *     str label,
 *     str value,
 */

  /* function exit code */
  __pyx_L1_error:;
  __Pyx_XDECREF(__pyx_t_1);
  __Pyx_XDECREF(__pyx_t_5);
  __Pyx_AddTraceback("core.input_text", __pyx_clineno, __pyx_lineno, __pyx_filename);
  __pyx_r = NULL;
  __pyx_L0:;
  __Pyx_XDECREF(__pyx_v_output);
  __Pyx_XGIVEREF(__pyx_r);
  __Pyx_RefNannyFinishContext();
  return __pyx_r;
}



extern "C" PyObject *__pyx_pf_4core_160text(CYTHON_UNUSED PyObject *__pyx_self, PyObject *__pyx_v_text) {
  PyObject *__pyx_r = NULL;
  __Pyx_RefNannyDeclarations
  PyObject *__pyx_t_1 = NULL;
  char *__pyx_t_2;
  int __pyx_lineno = 0;
  const char *__pyx_filename = NULL;
  int __pyx_clineno = 0;
  __Pyx_RefNannySetupContext("text", 0);

  /* "core.pyx":3733
 *     """
 *     # note: "%s" required for safety and to favor of Python string formating
 *     cimgui.Text("%s", _bytes(text))             # <<<<<<<<<<<<<<
 * 
 * 
 */
  __pyx_t_1 = __pyx_f_4core__bytes(__pyx_v_text); if (unlikely(!__pyx_t_1)) __PYX_ERR(0, 3733, __pyx_L1_error)
  __Pyx_GOTREF(__pyx_t_1);
  if (unlikely(__pyx_t_1 == Py_None)) {
    PyErr_SetString(PyExc_TypeError, "expected bytes, NoneType found");
    __PYX_ERR(0, 3733, __pyx_L1_error)
  }
  __pyx_t_2 = __Pyx_PyBytes_AsWritableString(__pyx_t_1); if (unlikely((!__pyx_t_2) && PyErr_Occurred())) __PYX_ERR(0, 3733, __pyx_L1_error)
  try {
    ImGui::Text(((char const *)"%s"), __pyx_t_2);
  } catch(...) {
    __Pyx_CppExn2PyErr();
    __PYX_ERR(0, 3733, __pyx_L1_error)
  }
  __Pyx_DECREF(__pyx_t_1); __pyx_t_1 = 0;

  /* "core.pyx":3714
 * 
 * 
 * def text(str text):             # <<<<<<<<<<<<<<
 *     """Add text to current widget stack.
 * 
 */

  /* function exit code */
  __pyx_r = Py_None; __Pyx_INCREF(Py_None);
  goto __pyx_L0;
  __pyx_L1_error:;
  __Pyx_XDECREF(__pyx_t_1);
  __Pyx_AddTraceback("core.text", __pyx_clineno, __pyx_lineno, __pyx_filename);
  __pyx_r = NULL;
  __pyx_L0:;
  __Pyx_XGIVEREF(__pyx_r);
  __Pyx_RefNannyFinishContext();
  return __pyx_r;
}


