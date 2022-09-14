#include <stdio.h>
#include "cython2mp.h"
#include "imgui.h"

#define __pyx_f_5imgui_4core__from_bytes(x) (x)
#define __pyx_f_5imgui_4core__bytes(x) (x)


extern "C" PyObject *__pyx_pf_5imgui_4core_218input_text(CYTHON_UNUSED PyObject *__pyx_self, PyObject *__pyx_v_label, PyObject *__pyx_v_value, int __pyx_v_buffer_length, ImGuiInputTextFlags __pyx_v_flags) {
  char *__pyx_v_inout_text;
  bool __pyx_v_changed;
  PyObject *__pyx_v_output = NULL;
  PyObject *__pyx_r = NULL;
  __Pyx_TraceDeclarations
  __Pyx_RefNannyDeclarations
  PyObject *__pyx_t_1 = NULL;
  char const *__pyx_t_2;
  char const *__pyx_t_3;
  bool __pyx_t_4;
  PyObject *__pyx_t_5 = NULL;
  int __pyx_lineno = 0;
  const char *__pyx_filename = NULL;
  int __pyx_clineno = 0;
  __Pyx_TraceFrameInit(__pyx_codeobj__160)
  __Pyx_RefNannySetupContext("input_text", 0);
  __Pyx_TraceCall("input_text", __pyx_f[0], 4870, 0, __PYX_ERR(0, 4870, __pyx_L1_error));

  /* "imgui/core.pyx":4919
 *     """
 *     # todo: pymalloc
 *     cdef char* inout_text = <char*>malloc(buffer_length * sizeof(char))             # <<<<<<<<<<<<<<
 *     # todo: take special care of terminating char
 *     strncpy(inout_text, _bytes(value), buffer_length)
 */
  __Pyx_TraceLine(4919,0,__PYX_ERR(0, 4919, __pyx_L1_error))
  __pyx_v_inout_text = ((char *)malloc((__pyx_v_buffer_length * (sizeof(char)))));

  /* "imgui/core.pyx":4921
 *     cdef char* inout_text = <char*>malloc(buffer_length * sizeof(char))
 *     # todo: take special care of terminating char
 *     strncpy(inout_text, _bytes(value), buffer_length)             # <<<<<<<<<<<<<<
 * 
 *     changed = cimgui.InputText(
 */
  __Pyx_TraceLine(4921,0,__PYX_ERR(0, 4921, __pyx_L1_error))
  __pyx_t_1 = __pyx_f_5imgui_4core__bytes(__pyx_v_value); if (unlikely(!__pyx_t_1)) __PYX_ERR(0, 4921, __pyx_L1_error)
  __Pyx_GOTREF(__pyx_t_1);
  if (unlikely(__pyx_t_1 == Py_None)) {
    PyErr_SetString(PyExc_TypeError, "expected bytes, NoneType found");
    __PYX_ERR(0, 4921, __pyx_L1_error)
  }
  __pyx_t_2 = __Pyx_PyBytes_AsString(__pyx_t_1); if (unlikely((!__pyx_t_2) && PyErr_Occurred())) __PYX_ERR(0, 4921, __pyx_L1_error)
  (void)(strncpy(__pyx_v_inout_text, __pyx_t_2, __pyx_v_buffer_length));
  __Pyx_DECREF(__pyx_t_1); __pyx_t_1 = 0;

  /* "imgui/core.pyx":4924
 * 
 *     changed = cimgui.InputText(
 *         _bytes(label), inout_text, buffer_length, flags, NULL, NULL             # <<<<<<<<<<<<<<
 *     )
 *     output = _from_bytes(inout_text)
 */
  __Pyx_TraceLine(4924,0,__PYX_ERR(0, 4924, __pyx_L1_error))
  __pyx_t_1 = __pyx_f_5imgui_4core__bytes(__pyx_v_label); if (unlikely(!__pyx_t_1)) __PYX_ERR(0, 4924, __pyx_L1_error)
  __Pyx_GOTREF(__pyx_t_1);
  if (unlikely(__pyx_t_1 == Py_None)) {
    PyErr_SetString(PyExc_TypeError, "expected bytes, NoneType found");
    __PYX_ERR(0, 4924, __pyx_L1_error)
  }
  __pyx_t_3 = __Pyx_PyBytes_AsString(__pyx_t_1); if (unlikely((!__pyx_t_3) && PyErr_Occurred())) __PYX_ERR(0, 4924, __pyx_L1_error)

  /* "imgui/core.pyx":4923
 *     strncpy(inout_text, _bytes(value), buffer_length)
 * 
 *     changed = cimgui.InputText(             # <<<<<<<<<<<<<<
 *         _bytes(label), inout_text, buffer_length, flags, NULL, NULL
 *     )
 */
  __Pyx_TraceLine(4923,0,__PYX_ERR(0, 4923, __pyx_L1_error))
  /*try*/if(true) {
    __pyx_t_4 = ImGui::InputText(__pyx_t_3, __pyx_v_inout_text, __pyx_v_buffer_length, __pyx_v_flags, NULL, NULL);
  } /*catch(...)*/else {
    __Pyx_CppExn2PyErr();
    __PYX_ERR(0, 4923, __pyx_L1_error)
  }
  __Pyx_DECREF(__pyx_t_1); __pyx_t_1 = 0;
  __pyx_v_changed = __pyx_t_4;

  /* "imgui/core.pyx":4926
 *         _bytes(label), inout_text, buffer_length, flags, NULL, NULL
 *     )
 *     output = _from_bytes(inout_text)             # <<<<<<<<<<<<<<
 * 
 *     free(inout_text)
 */
  __Pyx_TraceLine(4926,0,__PYX_ERR(0, 4926, __pyx_L1_error))
  __pyx_t_1 = __Pyx_PyBytes_FromString(__pyx_v_inout_text); if (unlikely(!__pyx_t_1)) __PYX_ERR(0, 4926, __pyx_L1_error)
  __Pyx_GOTREF(__pyx_t_1);
  __pyx_t_5 = __pyx_f_5imgui_4core__from_bytes(((PyObject*)__pyx_t_1)); if (unlikely(!__pyx_t_5)) __PYX_ERR(0, 4926, __pyx_L1_error)
  __Pyx_GOTREF(__pyx_t_5);
  __Pyx_DECREF(__pyx_t_1); __pyx_t_1 = 0;
  __pyx_v_output = __pyx_t_5;
  __pyx_t_5 = 0;

  /* "imgui/core.pyx":4928
 *     output = _from_bytes(inout_text)
 * 
 *     free(inout_text)             # <<<<<<<<<<<<<<
 *     return changed, output
 * 
 */
  __Pyx_TraceLine(4928,0,__PYX_ERR(0, 4928, __pyx_L1_error))
  free(__pyx_v_inout_text);

  /* "imgui/core.pyx":4929
 * 
 *     free(inout_text)
 *     return changed, output             # <<<<<<<<<<<<<<
 * 
 * 
 */
  __Pyx_TraceLine(4929,0,__PYX_ERR(0, 4929, __pyx_L1_error))
  __Pyx_XDECREF(__pyx_r);
  __pyx_t_5 = __Pyx_PyBool_FromLong(__pyx_v_changed); if (unlikely(!__pyx_t_5)) __PYX_ERR(0, 4929, __pyx_L1_error)
  __Pyx_GOTREF(__pyx_t_5);
  __pyx_t_1 = PyTuple_New(2); if (unlikely(!__pyx_t_1)) __PYX_ERR(0, 4929, __pyx_L1_error)
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

  /* "imgui/core.pyx":4870
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
  __Pyx_AddTraceback("imgui.core.input_text", __pyx_clineno, __pyx_lineno, __pyx_filename);
  __pyx_r = NULL;
  __pyx_L0:;
  __Pyx_XDECREF(__pyx_v_output);
  __Pyx_XGIVEREF(__pyx_r);
  __Pyx_TraceReturn(__pyx_r, 0);
  __Pyx_RefNannyFinishContext();
  return __pyx_r;
}
