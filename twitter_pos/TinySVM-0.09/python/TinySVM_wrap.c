/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 1.3.6u-20010826-1259
 * 
 * This file is not intended to be easily readable and contains a number of 
 * coding conventions designed to improve portability and efficiency. Do not make
 * changes to this file unless you know what you are doing--modify the SWIG 
 * interface file instead. 
 * ----------------------------------------------------------------------------- */

#define SWIGPYTHON
/***********************************************************************
 * common.swg
 *
 *     This file contains generic SWIG runtime support for pointer
 *     type checking as well as a few commonly used macros to control
 *     external linkage.
 *
 * Author : David Beazley (beazley@cs.uchicago.edu)
 *
 * Copyright (c) 1999-2000, The University of Chicago
 * 
 * This file may be freely redistributed without license or fee provided
 * this copyright message remains intact.
 ************************************************************************/

#include <string.h>

#if defined(_WIN32) || defined(__WIN32__)
#       if defined(_MSC_VER)
#               if defined(STATIC_LINKED)
#                       define SWIGEXPORT(a) a
#               else
#                       define SWIGEXPORT(a) __declspec(dllexport) a
#               endif
#       else
#               if defined(__BORLANDC__)
#                       define SWIGEXPORT(a) a _export
#               else
#                       define SWIGEXPORT(a) a
#       endif
#endif
#else
#       define SWIGEXPORT(a) a
#endif

#ifdef SWIG_GLOBAL
#define SWIGRUNTIME(a) SWIGEXPORT(a)
#else
#define SWIGRUNTIME(a) static a
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct swig_type_info {
  const char  *name;                 
  void *(*converter)(void *);
  const char  *str;
  struct swig_type_info  *next;
  struct swig_type_info  *prev;
} swig_type_info;

#ifdef SWIG_NOINCLUDE
SWIGEXPORT(swig_type_info *) SWIG_TypeRegister(swig_type_info *);
SWIGEXPORT(swig_type_info *) SWIG_TypeCheck(char *c, swig_type_info *);
SWIGEXPORT(void *) SWIG_TypeCast(swig_type_info *, void *);
SWIGEXPORT(swig_type_info *) SWIG_TypeQuery(const char *);
#else

static swig_type_info *swig_type_list = 0;

/* Register a type mapping with the type-checking */
SWIGRUNTIME(swig_type_info *)
SWIG_TypeRegister(swig_type_info *ti)
{
  swig_type_info *tc, *head, *ret, *next;
  /* Check to see if this type has already been registered */
  tc = swig_type_list;
  while (tc) {
    if (strcmp(tc->name, ti->name) == 0) {
      /* Already exists in the table.  Just add additional types to the list */
      head = tc;
      next = tc->next;
      goto l1;
    }
    tc = tc->prev;
  }
  head = ti;
  next = 0;

  /* Place in list */
  ti->prev = swig_type_list;
  swig_type_list = ti;

  /* Build linked lists */
 l1:
  ret = head;
  tc = ti + 1;
  /* Patch up the rest of the links */
  while (tc->name) {
    head->next = tc;
    tc->prev = head;
    head = tc;
    tc++;
  }
  head->next = next;
  return ret;
}

/* Check the typename */
SWIGRUNTIME(swig_type_info *) 
SWIG_TypeCheck(char *c, swig_type_info *ty)
{
  swig_type_info *s;
  if (!ty) return 0;        /* Void pointer */
  s = ty->next;             /* First element always just a name */
  while (s) {
    if (strcmp(s->name,c) == 0) {
      if (s == ty->next) return s;
      /* Move s to the top of the linked list */
      s->prev->next = s->next;
      if (s->next) {
	s->next->prev = s->prev;
      }
      /* Insert s as second element in the list */
      s->next = ty->next;
      if (ty->next) ty->next->prev = s;
      ty->next = s;
      return s;
    }
    s = s->next;
  }
  return 0;
}

/* Cast a pointer (needed for C++ inheritance */
SWIGRUNTIME(void *) 
SWIG_TypeCast(swig_type_info *ty, void *ptr) 
{
  if ((!ty) || (!ty->converter)) return ptr;
  return (*ty->converter)(ptr);
}

/* Search for a swig_type_info structure */
SWIGRUNTIME(swig_type_info *)
SWIG_TypeQuery(const char *name) {
  swig_type_info *ty = swig_type_list;
  while (ty) {
    if (ty->str && (strcmp(name,ty->str) == 0)) return ty;
    if (ty->name && (strcmp(name,ty->name) == 0)) return ty;
    ty = ty->prev;
  }
  return 0;
}

#endif

#ifdef __cplusplus
}
#endif



/***********************************************************************
 * python.swg
 *
 *     This file contains the runtime support for Python modules
 *     and includes code for managing global variables and pointer
 *     type checking.
 *
 * Author : David Beazley (beazley@cs.uchicago.edu)
 ************************************************************************/

#include <stdlib.h>
#include "Python.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SWIG_PY_INT     1
#define SWIG_PY_FLOAT   2
#define SWIG_PY_STRING  3
#define SWIG_PY_POINTER 4

/* Constant information structure */
typedef struct swig_const_info {
    int type;
    char *name;
    long lvalue;
    double dvalue;
    void   *pvalue;
    swig_type_info **ptype;
} swig_const_info;

#ifdef SWIG_NOINCLUDE

SWIGEXPORT(PyObject *)        SWIG_newvarlink();
SWIGEXPORT(void)              SWIG_addvarlink(PyObject *, char *, PyObject *(*)(void), int (*)(PyObject *));
SWIGEXPORT(int)               SWIG_ConvertPtr(PyObject *, void **, swig_type_info *, int);
SWIGEXPORT(void)              SWIG_MakePtr(char *c, void *, swig_type_info *);
SWIGEXPORT(PyObject *)        SWIG_NewPointerObj(void *, swig_type_info *);
SWIGEXPORT(void)              SWIG_InstallConstants(PyObject *d, swig_const_info constants[]);

#else

/* -----------------------------------------------------------------------------
 * global variable support code.
 * ----------------------------------------------------------------------------- */

typedef struct swig_globalvar {   
  char       *name;                  /* Name of global variable */
  PyObject *(*get_attr)(void);       /* Return the current value */
  int       (*set_attr)(PyObject *); /* Set the value */
  struct swig_globalvar *next;
} swig_globalvar;

typedef struct swig_varlinkobject {
  PyObject_HEAD
  swig_globalvar *vars;
} swig_varlinkobject;

static PyObject *
swig_varlink_repr(swig_varlinkobject *v) {
  v = v;
  return PyString_FromString("<Global variables>");
}

static int
swig_varlink_print(swig_varlinkobject *v, FILE *fp, int flags) {
  swig_globalvar  *var;
  flags = flags;
  fprintf(fp,"Global variables { ");
  for (var = v->vars; var; var=var->next) {
    fprintf(fp,"%s", var->name);
    if (var->next) fprintf(fp,", ");
  }
  fprintf(fp," }\n");
  return 0;
}

static PyObject *
swig_varlink_getattr(swig_varlinkobject *v, char *n) {
  swig_globalvar *var = v->vars;
  while (var) {
    if (strcmp(var->name,n) == 0) {
      return (*var->get_attr)();
    }
    var = var->next;
  }
  PyErr_SetString(PyExc_NameError,"Unknown C global variable");
  return NULL;
}

static int
swig_varlink_setattr(swig_varlinkobject *v, char *n, PyObject *p) {
  swig_globalvar *var = v->vars;
  while (var) {
    if (strcmp(var->name,n) == 0) {
      return (*var->set_attr)(p);
    }
    var = var->next;
  }
  PyErr_SetString(PyExc_NameError,"Unknown C global variable");
  return 1;
}

statichere PyTypeObject varlinktype = {
  PyObject_HEAD_INIT(0)              
  0,
  "swigvarlink",                      /* Type name    */
  sizeof(swig_varlinkobject),         /* Basic size   */
  0,                                  /* Itemsize     */
  0,                                  /* Deallocator  */ 
  (printfunc) swig_varlink_print,     /* Print        */
  (getattrfunc) swig_varlink_getattr, /* get attr     */
  (setattrfunc) swig_varlink_setattr, /* Set attr     */
  0,                                  /* tp_compare   */
  (reprfunc) swig_varlink_repr,       /* tp_repr      */    
  0,                                  /* tp_as_number */
  0,                                  /* tp_as_mapping*/
  0,                                  /* tp_hash      */
};

/* Create a variable linking object for use later */
SWIGRUNTIME(PyObject *)
SWIG_newvarlink(void) {
  swig_varlinkobject *result = 0;
  result = PyMem_NEW(swig_varlinkobject,1);
  varlinktype.ob_type = &PyType_Type;    /* Patch varlinktype into a PyType */
  result->ob_type = &varlinktype;
  result->vars = 0;
  result->ob_refcnt = 0;
  Py_XINCREF((PyObject *) result);
  return ((PyObject*) result);
}

SWIGRUNTIME(void)
SWIG_addvarlink(PyObject *p, char *name,
	   PyObject *(*get_attr)(void), int (*set_attr)(PyObject *p)) {
  swig_varlinkobject *v;
  swig_globalvar *gv;
  v= (swig_varlinkobject *) p;
  gv = (swig_globalvar *) malloc(sizeof(swig_globalvar));
  gv->name = (char *) malloc(strlen(name)+1);
  strcpy(gv->name,name);
  gv->get_attr = get_attr;
  gv->set_attr = set_attr;
  gv->next = v->vars;
  v->vars = gv;
}
/* Convert a pointer value */
SWIGRUNTIME(int)
SWIG_ConvertPtr(PyObject *obj, void **ptr, swig_type_info *ty, int flags) {
  unsigned long p;
  register int d;
  swig_type_info *tc;
  char  *c;
  static PyObject *SWIG_this = 0;
  int    newref = 0;

  if (!obj || (obj == Py_None)) {
    *ptr = 0;
    return 0;
  }
#ifdef SWIG_COBJECT_TYPES
  if (!(PyCObject_Check(obj))) {
    if (!SWIG_this)
      SWIG_this = PyString_InternFromString("this");
    obj = PyObject_GetAttr(obj,SWIG_this);
    newref = 1;
    if (!obj) goto type_error;
    if (!PyCObject_Check(obj)) {
      Py_DECREF(obj);
      goto type_error;
    }
  } 
  *ptr = PyCObject_AsVoidPtr(obj);
  c = (char *) PyCObject_GetDesc(obj);
  if (newref) Py_DECREF(obj);
  goto cobject;
#else
  if (!(PyString_Check(obj))) {
    if (!SWIG_this)
      SWIG_this = PyString_InternFromString("this");
    obj = PyObject_GetAttr(obj,SWIG_this);
    newref = 1;
    if (!obj) goto type_error;
    if (!PyString_Check(obj)) {
      Py_DECREF(obj);
      goto type_error;
    }
  } 
  c = PyString_AsString(obj);
  p = 0;
  /* Pointer values must start with leading underscore */
  if (*c != '_') {
    *ptr = (void *) 0;
    if (strcmp(c,"NULL") == 0) {
      if (newref) Py_DECREF(obj);
      return 0;
    } else {
      if (newref) Py_DECREF(obj);
      goto type_error;
    }
  }
  c++;
  /* Extract hex value from pointer */
  while ((d = *c)) {
    if ((d >= '0') && (d <= '9'))
      p = (p << 4) + (d - '0');
    else if ((d >= 'a') && (d <= 'f'))
      p = (p << 4) + (d - ('a'-10));
    else
      break; 
    c++;
  }
  *ptr = (void *) p;
  if (newref) Py_DECREF(obj);
#endif

#ifdef SWIG_COBJECT_TYPES
cobject:
#endif

  if (ty) {
    tc = SWIG_TypeCheck(c,ty);
    if (!tc) goto type_error;
    *ptr = SWIG_TypeCast(tc,(void*)p);
  }
  return 0;

type_error:

  if (flags) {
    if (ty) {
      char *temp = (char *) malloc(64+strlen(ty->name));
      sprintf(temp,"Type error. Expected %s", ty->name);
      PyErr_SetString(PyExc_TypeError, temp);
      free((char *) temp);
    } else {
      PyErr_SetString(PyExc_TypeError,"Expected a pointer");
    }
  }
  return -1;
}

/* Take a pointer and convert it to a string */
SWIGRUNTIME(void) 
SWIG_MakePtr(char *c, void *ptr, swig_type_info *ty) {
  static char hex[17] = "0123456789abcdef";
  unsigned long p, s;
  char result[32], *r; 
  r = result;
  p = (unsigned long) ptr;
  if (p > 0) {
    while (p > 0) {
      s = p & 0xf;
      *(r++) = hex[s];
      p = p >> 4;
    }
    *r = '_';
    while (r >= result)
      *(c++) = *(r--);
    strcpy (c, ty->name);
  } else {
    strcpy (c, "NULL");
  }
}

/* Create a new pointer object */
SWIGRUNTIME(PyObject *)
SWIG_NewPointerObj(void *ptr, swig_type_info *type) {
  char result[512];
  PyObject *robj;
  if (!ptr) {
    Py_INCREF(Py_None);
    return Py_None;
  }
#ifdef SWIG_COBJECT_TYPES
  robj = PyCObject_FromVoidPtrAndDesc((void *) ptr, type->name, NULL);
#else
  SWIG_MakePtr(result,ptr,type);
  robj = PyString_FromString(result);
#endif
  return robj;
}

/* Install Constants */
SWIGRUNTIME(void)
SWIG_InstallConstants(PyObject *d, swig_const_info constants[]) {
  int i;
  PyObject *obj;
  for (i = 0; constants[i].type; i++) {
    switch(constants[i].type) {
    case SWIG_PY_INT:
      obj = PyInt_FromLong(constants[i].lvalue);
      break;
    case SWIG_PY_FLOAT:
      obj = PyFloat_FromDouble(constants[i].dvalue);
      break;
    case SWIG_PY_STRING:
      obj = PyString_FromString((char *) constants[i].pvalue);
      break;
    case SWIG_PY_POINTER:
      obj = SWIG_NewPointerObj(constants[i].pvalue, *(constants[i]).ptype);
      break;
    default:
      obj = 0;
      break;
    }
    if (obj) {
      PyDict_SetItemString(d,constants[i].name,obj);
      Py_DECREF(obj);
    }
  }
}

#endif

#ifdef __cplusplus
}
#endif



/* -------- TYPES TABLE (BEGIN) -------- */

#define  SWIGTYPE_p_BaseExample swig_types[0] 
#define  SWIGTYPE_p_Example swig_types[1] 
#define  SWIGTYPE_p_Model swig_types[2] 
static swig_type_info *swig_types[4];

/* -------- TYPES TABLE (END) -------- */


/*-----------------------------------------------
              @(target):= TinySVMc.so
  ------------------------------------------------*/
#define SWIG_init    initTinySVMc

#define SWIG_name    "TinySVMc"

#include <tinysvm.h>
#include <ctype.h>
#include <string.h>
#define BaseExample TinySVM::BaseExample
#define Example TinySVM::Example 
#define Model TinySVM::Model
#undef VALUEFUNC
#define VALUEFUNC(f) ((VALUE (*)())f)
#ifdef __cplusplus
extern "C" {
#endif
static PyObject *_wrap_BaseExample_add(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    BaseExample *arg0 ;
    char *arg1 ;
    PyObject * argo0 =0 ;
    int result ;
    
    if(!PyArg_ParseTuple(args,"Os:BaseExample_add",&argo0,&arg1)) return NULL;
    if ((SWIG_ConvertPtr(argo0,(void **) &arg0,SWIGTYPE_p_BaseExample,1)) == -1) return NULL;
    result = (int )arg0->add((char const *)arg1);
    resultobj = PyInt_FromLong((long)result);
    return resultobj;
}


static PyObject *_wrap_BaseExample_set(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    BaseExample *arg0 ;
    int arg1 ;
    char *arg2 ;
    PyObject * argo0 =0 ;
    int result ;
    
    if(!PyArg_ParseTuple(args,"Ois:BaseExample_set",&argo0,&arg1,&arg2)) return NULL;
    if ((SWIG_ConvertPtr(argo0,(void **) &arg0,SWIGTYPE_p_BaseExample,1)) == -1) return NULL;
    result = (int )arg0->set(arg1,(char const *)arg2);
    resultobj = PyInt_FromLong((long)result);
    return resultobj;
}


static PyObject *_wrap_BaseExample_get(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    BaseExample *arg0 ;
    int arg1 ;
    PyObject * argo0 =0 ;
    char *result ;
    
    if(!PyArg_ParseTuple(args,"Oi:BaseExample_get",&argo0,&arg1)) return NULL;
    if ((SWIG_ConvertPtr(argo0,(void **) &arg0,SWIGTYPE_p_BaseExample,1)) == -1) return NULL;
    result = (char *)arg0->get(arg1);
    resultobj = PyString_FromString(result);
    return resultobj;
}


static PyObject *_wrap_BaseExample_remove(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    BaseExample *arg0 ;
    int arg1 ;
    PyObject * argo0 =0 ;
    int result ;
    
    if(!PyArg_ParseTuple(args,"Oi:BaseExample_remove",&argo0,&arg1)) return NULL;
    if ((SWIG_ConvertPtr(argo0,(void **) &arg0,SWIGTYPE_p_BaseExample,1)) == -1) return NULL;
    result = (int )arg0->remove(arg1);
    resultobj = PyInt_FromLong((long)result);
    return resultobj;
}


static PyObject *_wrap_BaseExample_clear(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    BaseExample *arg0 ;
    PyObject * argo0 =0 ;
    int result ;
    
    if(!PyArg_ParseTuple(args,"O:BaseExample_clear",&argo0)) return NULL;
    if ((SWIG_ConvertPtr(argo0,(void **) &arg0,SWIGTYPE_p_BaseExample,1)) == -1) return NULL;
    result = (int )arg0->clear();
    resultobj = PyInt_FromLong((long)result);
    return resultobj;
}


static PyObject *_wrap_BaseExample_size(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    BaseExample *arg0 ;
    PyObject * argo0 =0 ;
    int result ;
    
    if(!PyArg_ParseTuple(args,"O:BaseExample_size",&argo0)) return NULL;
    if ((SWIG_ConvertPtr(argo0,(void **) &arg0,SWIGTYPE_p_BaseExample,1)) == -1) return NULL;
    result = (int )arg0->size();
    resultobj = PyInt_FromLong((long)result);
    return resultobj;
}


static PyObject *_wrap_BaseExample_read(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    BaseExample *arg0 ;
    char *arg1 ;
    char *arg2 = "r" ;
    int arg3 = 0 ;
    PyObject * argo0 =0 ;
    int result ;
    
    if(!PyArg_ParseTuple(args,"Os|si:BaseExample_read",&argo0,&arg1,&arg2,&arg3)) return NULL;
    if ((SWIG_ConvertPtr(argo0,(void **) &arg0,SWIGTYPE_p_BaseExample,1)) == -1) return NULL;
    result = (int )arg0->read((char const *)arg1,(char const *)arg2,arg3);
    resultobj = PyInt_FromLong((long)result);
    return resultobj;
}


static PyObject *_wrap_BaseExample_write(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    BaseExample *arg0 ;
    char *arg1 ;
    char *arg2 = "w" ;
    int arg3 = 0 ;
    PyObject * argo0 =0 ;
    int result ;
    
    if(!PyArg_ParseTuple(args,"Os|si:BaseExample_write",&argo0,&arg1,&arg2,&arg3)) return NULL;
    if ((SWIG_ConvertPtr(argo0,(void **) &arg0,SWIGTYPE_p_BaseExample,1)) == -1) return NULL;
    result = (int )arg0->write((char const *)arg1,(char const *)arg2,arg3);
    resultobj = PyInt_FromLong((long)result);
    return resultobj;
}


static PyObject *_wrap_BaseExample_readSVindex(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    BaseExample *arg0 ;
    char *arg1 ;
    char *arg2 = "r" ;
    int arg3 = 0 ;
    PyObject * argo0 =0 ;
    int result ;
    
    if(!PyArg_ParseTuple(args,"Os|si:BaseExample_readSVindex",&argo0,&arg1,&arg2,&arg3)) return NULL;
    if ((SWIG_ConvertPtr(argo0,(void **) &arg0,SWIGTYPE_p_BaseExample,1)) == -1) return NULL;
    result = (int )arg0->readSVindex((char const *)arg1,(char const *)arg2,arg3);
    resultobj = PyInt_FromLong((long)result);
    return resultobj;
}


static PyObject *_wrap_BaseExample_writeSVindex(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    BaseExample *arg0 ;
    char *arg1 ;
    char *arg2 = "w" ;
    int arg3 = 0 ;
    PyObject * argo0 =0 ;
    int result ;
    
    if(!PyArg_ParseTuple(args,"Os|si:BaseExample_writeSVindex",&argo0,&arg1,&arg2,&arg3)) return NULL;
    if ((SWIG_ConvertPtr(argo0,(void **) &arg0,SWIGTYPE_p_BaseExample,1)) == -1) return NULL;
    result = (int )arg0->writeSVindex((char const *)arg1,(char const *)arg2,arg3);
    resultobj = PyInt_FromLong((long)result);
    return resultobj;
}


static PyObject *_wrap_delete_BaseExample(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    BaseExample *arg0 ;
    PyObject * argo0 =0 ;
    
    if(!PyArg_ParseTuple(args,"O:delete_BaseExample",&argo0)) return NULL;
    if ((SWIG_ConvertPtr(argo0,(void **) &arg0,SWIGTYPE_p_BaseExample,1)) == -1) return NULL;
    delete arg0;
    Py_INCREF(Py_None);
    resultobj = Py_None;
    return resultobj;
}


int  BaseExample_append(BaseExample *self,char *f) {
    {
        return self->write(f,"a"); 
    }
}


static PyObject *_wrap_BaseExample_append(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    BaseExample *arg0 ;
    char *arg1 ;
    PyObject * argo0 =0 ;
    int result ;
    
    if(!PyArg_ParseTuple(args,"Os:BaseExample_append",&argo0,&arg1)) return NULL;
    if ((SWIG_ConvertPtr(argo0,(void **) &arg0,SWIGTYPE_p_BaseExample,1)) == -1) return NULL;
    result = (int )BaseExample_append(arg0,arg1);
    resultobj = PyInt_FromLong((long)result);
    return resultobj;
}


int  BaseExample_appendSVindex(BaseExample *self,char *f) {
    {
        return self->writeSVindex(f,"a"); 
    }
}


static PyObject *_wrap_BaseExample_appendSVindex(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    BaseExample *arg0 ;
    char *arg1 ;
    PyObject * argo0 =0 ;
    int result ;
    
    if(!PyArg_ParseTuple(args,"Os:BaseExample_appendSVindex",&argo0,&arg1)) return NULL;
    if ((SWIG_ConvertPtr(argo0,(void **) &arg0,SWIGTYPE_p_BaseExample,1)) == -1) return NULL;
    result = (int )BaseExample_appendSVindex(arg0,arg1);
    resultobj = PyInt_FromLong((long)result);
    return resultobj;
}


int  BaseExample_getDimension(BaseExample *self) {
    {
        return self->d; 
    }
}


static PyObject *_wrap_BaseExample_getDimension(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    BaseExample *arg0 ;
    PyObject * argo0 =0 ;
    int result ;
    
    if(!PyArg_ParseTuple(args,"O:BaseExample_getDimension",&argo0)) return NULL;
    if ((SWIG_ConvertPtr(argo0,(void **) &arg0,SWIGTYPE_p_BaseExample,1)) == -1) return NULL;
    result = (int )BaseExample_getDimension(arg0);
    resultobj = PyInt_FromLong((long)result);
    return resultobj;
}


int  BaseExample_getNonzeroDimension(BaseExample *self) {
    {
        return self->pack_d; 
    }
}


static PyObject *_wrap_BaseExample_getNonzeroDimension(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    BaseExample *arg0 ;
    PyObject * argo0 =0 ;
    int result ;
    
    if(!PyArg_ParseTuple(args,"O:BaseExample_getNonzeroDimension",&argo0)) return NULL;
    if ((SWIG_ConvertPtr(argo0,(void **) &arg0,SWIGTYPE_p_BaseExample,1)) == -1) return NULL;
    result = (int )BaseExample_getNonzeroDimension(arg0);
    resultobj = PyInt_FromLong((long)result);
    return resultobj;
}


double  BaseExample_getY(BaseExample *self,int i) {
    {
        if (i >= 0 && i < self->l) return self->y[i];
        fprintf(stderr, "BaseExample::getY() -- index is out of range\n");
        return 0.0;
    }
}


static PyObject *_wrap_BaseExample_getY(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    BaseExample *arg0 ;
    int arg1 ;
    PyObject * argo0 =0 ;
    double result ;
    
    if(!PyArg_ParseTuple(args,"Oi:BaseExample_getY",&argo0,&arg1)) return NULL;
    if ((SWIG_ConvertPtr(argo0,(void **) &arg0,SWIGTYPE_p_BaseExample,1)) == -1) return NULL;
    result = (double )BaseExample_getY(arg0,arg1);
    resultobj = PyFloat_FromDouble(result);
    return resultobj;
}


char const * BaseExample_getX(BaseExample *self,int i) {
    {
        const char *s = self->get(i);
        if (!s) return 0;
        
        int len = strlen (s);
        int i;
        for (i = 0; i < len;) {
            while (isspace (s[i])) i++;
            while (i < len && !isspace (s[i])) i++;
            while (i < len && isspace (s[i]))  i++;
            break;
        }
        
        return (const char*)(s + i);
    }
}


static PyObject *_wrap_BaseExample_getX(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    BaseExample *arg0 ;
    int arg1 ;
    PyObject * argo0 =0 ;
    char *result ;
    
    if(!PyArg_ParseTuple(args,"Oi:BaseExample_getX",&argo0,&arg1)) return NULL;
    if ((SWIG_ConvertPtr(argo0,(void **) &arg0,SWIGTYPE_p_BaseExample,1)) == -1) return NULL;
    result = (char *)BaseExample_getX(arg0,arg1);
    resultobj = PyString_FromString(result);
    return resultobj;
}


double  BaseExample_getAlpha(BaseExample *self,int i) {
    {
        if (self->alpha && i >= 0 && i < self->svindex_size) return self->alpha[i];
        fprintf(stderr, "BaseExample::getAlpha() -- \nalpha is not available or index is out of range\n");
        return 0.0;
    }
}


static PyObject *_wrap_BaseExample_getAlpha(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    BaseExample *arg0 ;
    int arg1 ;
    PyObject * argo0 =0 ;
    double result ;
    
    if(!PyArg_ParseTuple(args,"Oi:BaseExample_getAlpha",&argo0,&arg1)) return NULL;
    if ((SWIG_ConvertPtr(argo0,(void **) &arg0,SWIGTYPE_p_BaseExample,1)) == -1) return NULL;
    result = (double )BaseExample_getAlpha(arg0,arg1);
    resultobj = PyFloat_FromDouble(result);
    return resultobj;
}


double  BaseExample_getGradient(BaseExample *self,int i) {
    {
        if (self->G && i >= 0 && i < self->svindex_size) return self->G[i];
        fprintf(stderr, "BaseExample::getGradient() -- \ngradient is not available or index is out of range\n");
        return 0.0;
    }
}


static PyObject *_wrap_BaseExample_getGradient(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    BaseExample *arg0 ;
    int arg1 ;
    PyObject * argo0 =0 ;
    double result ;
    
    if(!PyArg_ParseTuple(args,"Oi:BaseExample_getGradient",&argo0,&arg1)) return NULL;
    if ((SWIG_ConvertPtr(argo0,(void **) &arg0,SWIGTYPE_p_BaseExample,1)) == -1) return NULL;
    result = (double )BaseExample_getGradient(arg0,arg1);
    resultobj = PyFloat_FromDouble(result);
    return resultobj;
}


double  BaseExample_getG(BaseExample *self,int i) {
    {
        if (self->G && i >= 0 && i < self->svindex_size) return self->G[i];
        fprintf(stderr, "BaseExample::getG() -- index is out of range\n");
        return 0.0;
    }
}


static PyObject *_wrap_BaseExample_getG(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    BaseExample *arg0 ;
    int arg1 ;
    PyObject * argo0 =0 ;
    double result ;
    
    if(!PyArg_ParseTuple(args,"Oi:BaseExample_getG",&argo0,&arg1)) return NULL;
    if ((SWIG_ConvertPtr(argo0,(void **) &arg0,SWIGTYPE_p_BaseExample,1)) == -1) return NULL;
    result = (double )BaseExample_getG(arg0,arg1);
    resultobj = PyFloat_FromDouble(result);
    return resultobj;
}


static PyObject *_wrap_Model_read(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    Model *arg0 ;
    char *arg1 ;
    char *arg2 = "r" ;
    int arg3 = 0 ;
    PyObject * argo0 =0 ;
    int result ;
    
    if(!PyArg_ParseTuple(args,"Os|si:Model_read",&argo0,&arg1,&arg2,&arg3)) return NULL;
    if ((SWIG_ConvertPtr(argo0,(void **) &arg0,SWIGTYPE_p_Model,1)) == -1) return NULL;
    result = (int )arg0->read((char const *)arg1,(char const *)arg2,arg3);
    resultobj = PyInt_FromLong((long)result);
    return resultobj;
}


static PyObject *_wrap_Model_write(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    Model *arg0 ;
    char *arg1 ;
    char *arg2 = "w" ;
    int arg3 = 0 ;
    PyObject * argo0 =0 ;
    int result ;
    
    if(!PyArg_ParseTuple(args,"Os|si:Model_write",&argo0,&arg1,&arg2,&arg3)) return NULL;
    if ((SWIG_ConvertPtr(argo0,(void **) &arg0,SWIGTYPE_p_Model,1)) == -1) return NULL;
    result = (int )arg0->write((char const *)arg1,(char const *)arg2,arg3);
    resultobj = PyInt_FromLong((long)result);
    return resultobj;
}


static PyObject *_wrap_Model_clear(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    Model *arg0 ;
    PyObject * argo0 =0 ;
    int result ;
    
    if(!PyArg_ParseTuple(args,"O:Model_clear",&argo0)) return NULL;
    if ((SWIG_ConvertPtr(argo0,(void **) &arg0,SWIGTYPE_p_Model,1)) == -1) return NULL;
    result = (int )arg0->clear();
    resultobj = PyInt_FromLong((long)result);
    return resultobj;
}


static PyObject *_wrap_Model_classify(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    Model *arg0 ;
    char *arg1 ;
    PyObject * argo0 =0 ;
    double result ;
    
    if(!PyArg_ParseTuple(args,"Os:Model_classify",&argo0,&arg1)) return NULL;
    if ((SWIG_ConvertPtr(argo0,(void **) &arg0,SWIGTYPE_p_Model,1)) == -1) return NULL;
    result = (double )arg0->classify((char const *)arg1);
    resultobj = PyFloat_FromDouble(result);
    return resultobj;
}


static PyObject *_wrap_Model_estimateMargin(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    Model *arg0 ;
    PyObject * argo0 =0 ;
    double result ;
    
    if(!PyArg_ParseTuple(args,"O:Model_estimateMargin",&argo0)) return NULL;
    if ((SWIG_ConvertPtr(argo0,(void **) &arg0,SWIGTYPE_p_Model,1)) == -1) return NULL;
    result = (double )arg0->estimateMargin();
    resultobj = PyFloat_FromDouble(result);
    return resultobj;
}


static PyObject *_wrap_Model_estimateSphere(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    Model *arg0 ;
    PyObject * argo0 =0 ;
    double result ;
    
    if(!PyArg_ParseTuple(args,"O:Model_estimateSphere",&argo0)) return NULL;
    if ((SWIG_ConvertPtr(argo0,(void **) &arg0,SWIGTYPE_p_Model,1)) == -1) return NULL;
    result = (double )arg0->estimateSphere();
    resultobj = PyFloat_FromDouble(result);
    return resultobj;
}


static PyObject *_wrap_Model_estimateVC(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    Model *arg0 ;
    PyObject * argo0 =0 ;
    double result ;
    
    if(!PyArg_ParseTuple(args,"O:Model_estimateVC",&argo0)) return NULL;
    if ((SWIG_ConvertPtr(argo0,(void **) &arg0,SWIGTYPE_p_Model,1)) == -1) return NULL;
    result = (double )arg0->estimateVC();
    resultobj = PyFloat_FromDouble(result);
    return resultobj;
}


static PyObject *_wrap_Model_estimateXA(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    Model *arg0 ;
    double arg1 = 2.0 ;
    PyObject * argo0 =0 ;
    double result ;
    
    if(!PyArg_ParseTuple(args,"O|d:Model_estimateXA",&argo0,&arg1)) return NULL;
    if ((SWIG_ConvertPtr(argo0,(void **) &arg0,SWIGTYPE_p_Model,1)) == -1) return NULL;
    result = (double )arg0->estimateXA(arg1);
    resultobj = PyFloat_FromDouble(result);
    return resultobj;
}


static PyObject *_wrap_Model_compress(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    Model *arg0 ;
    PyObject * argo0 =0 ;
    int result ;
    
    if(!PyArg_ParseTuple(args,"O:Model_compress",&argo0)) return NULL;
    if ((SWIG_ConvertPtr(argo0,(void **) &arg0,SWIGTYPE_p_Model,1)) == -1) return NULL;
    result = (int )arg0->compress();
    resultobj = PyInt_FromLong((long)result);
    return resultobj;
}


static PyObject *_wrap_Model_getSVnum(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    Model *arg0 ;
    PyObject * argo0 =0 ;
    int result ;
    
    if(!PyArg_ParseTuple(args,"O:Model_getSVnum",&argo0)) return NULL;
    if ((SWIG_ConvertPtr(argo0,(void **) &arg0,SWIGTYPE_p_Model,1)) == -1) return NULL;
    result = (int )arg0->getSVnum();
    resultobj = PyInt_FromLong((long)result);
    return resultobj;
}


static PyObject *_wrap_Model_getBSVnum(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    Model *arg0 ;
    PyObject * argo0 =0 ;
    int result ;
    
    if(!PyArg_ParseTuple(args,"O:Model_getBSVnum",&argo0)) return NULL;
    if ((SWIG_ConvertPtr(argo0,(void **) &arg0,SWIGTYPE_p_Model,1)) == -1) return NULL;
    result = (int )arg0->getBSVnum();
    resultobj = PyInt_FromLong((long)result);
    return resultobj;
}


static PyObject *_wrap_Model_getTrainingDataSize(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    Model *arg0 ;
    PyObject * argo0 =0 ;
    int result ;
    
    if(!PyArg_ParseTuple(args,"O:Model_getTrainingDataSize",&argo0)) return NULL;
    if ((SWIG_ConvertPtr(argo0,(void **) &arg0,SWIGTYPE_p_Model,1)) == -1) return NULL;
    result = (int )arg0->getTrainingDataSize();
    resultobj = PyInt_FromLong((long)result);
    return resultobj;
}


static PyObject *_wrap_Model_getLoss(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    Model *arg0 ;
    PyObject * argo0 =0 ;
    double result ;
    
    if(!PyArg_ParseTuple(args,"O:Model_getLoss",&argo0)) return NULL;
    if ((SWIG_ConvertPtr(argo0,(void **) &arg0,SWIGTYPE_p_Model,1)) == -1) return NULL;
    result = (double )arg0->getLoss();
    resultobj = PyFloat_FromDouble(result);
    return resultobj;
}


static PyObject *_wrap_new_Model(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    Model *result ;
    
    if(!PyArg_ParseTuple(args,":new_Model")) return NULL;
    result = (Model *)new Model();
    resultobj = SWIG_NewPointerObj((void *) result, SWIGTYPE_p_Model);
    return resultobj;
}


static PyObject *_wrap_delete_Model(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    Model *arg0 ;
    PyObject * argo0 =0 ;
    
    if(!PyArg_ParseTuple(args,"O:delete_Model",&argo0)) return NULL;
    if ((SWIG_ConvertPtr(argo0,(void **) &arg0,SWIGTYPE_p_Model,1)) == -1) return NULL;
    delete arg0;
    Py_INCREF(Py_None);
    resultobj = Py_None;
    return resultobj;
}


static PyObject *_wrap_Example_read(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    Example *arg0 ;
    char *arg1 ;
    char *arg2 = "r" ;
    int arg3 = 0 ;
    PyObject * argo0 =0 ;
    int result ;
    
    if(!PyArg_ParseTuple(args,"Os|si:Example_read",&argo0,&arg1,&arg2,&arg3)) return NULL;
    if ((SWIG_ConvertPtr(argo0,(void **) &arg0,SWIGTYPE_p_Example,1)) == -1) return NULL;
    result = (int )arg0->read((char const *)arg1,(char const *)arg2,arg3);
    resultobj = PyInt_FromLong((long)result);
    return resultobj;
}


static PyObject *_wrap_Example_write(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    Example *arg0 ;
    char *arg1 ;
    char *arg2 = "w" ;
    int arg3 = 0 ;
    PyObject * argo0 =0 ;
    int result ;
    
    if(!PyArg_ParseTuple(args,"Os|si:Example_write",&argo0,&arg1,&arg2,&arg3)) return NULL;
    if ((SWIG_ConvertPtr(argo0,(void **) &arg0,SWIGTYPE_p_Example,1)) == -1) return NULL;
    result = (int )arg0->write((char const *)arg1,(char const *)arg2,arg3);
    resultobj = PyInt_FromLong((long)result);
    return resultobj;
}


static PyObject *_wrap_Example_rebuildSVindex(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    Example *arg0 ;
    Model *arg1 ;
    PyObject * argo0 =0 ;
    PyObject * argo1 =0 ;
    int result ;
    
    if(!PyArg_ParseTuple(args,"OO:Example_rebuildSVindex",&argo0,&argo1)) return NULL;
    if ((SWIG_ConvertPtr(argo0,(void **) &arg0,SWIGTYPE_p_Example,1)) == -1) return NULL;
    if ((SWIG_ConvertPtr(argo1,(void **) &arg1,SWIGTYPE_p_Model,1)) == -1) return NULL;
    result = (int )arg0->rebuildSVindex(arg1);
    resultobj = PyInt_FromLong((long)result);
    return resultobj;
}


static PyObject *_wrap_Example_learn(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    Example *arg0 ;
    char *arg1 = 0 ;
    PyObject * argo0 =0 ;
    Model *result ;
    
    if(!PyArg_ParseTuple(args,"O|s:Example_learn",&argo0,&arg1)) return NULL;
    if ((SWIG_ConvertPtr(argo0,(void **) &arg0,SWIGTYPE_p_Example,1)) == -1) return NULL;
    result = (Model *)arg0->learn((char const *)arg1);
    resultobj = SWIG_NewPointerObj((void *) result, SWIGTYPE_p_Model);
    return resultobj;
}


static PyObject *_wrap_new_Example(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    Example *result ;
    
    if(!PyArg_ParseTuple(args,":new_Example")) return NULL;
    result = (Example *)new Example();
    resultobj = SWIG_NewPointerObj((void *) result, SWIGTYPE_p_Example);
    return resultobj;
}


static PyObject *_wrap_delete_Example(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    Example *arg0 ;
    PyObject * argo0 =0 ;
    
    if(!PyArg_ParseTuple(args,"O:delete_Example",&argo0)) return NULL;
    if ((SWIG_ConvertPtr(argo0,(void **) &arg0,SWIGTYPE_p_Example,1)) == -1) return NULL;
    delete arg0;
    Py_INCREF(Py_None);
    resultobj = Py_None;
    return resultobj;
}


static PyMethodDef TinySVMcMethods[] = {
	 { "BaseExample_add", _wrap_BaseExample_add, METH_VARARGS },
	 { "BaseExample_set", _wrap_BaseExample_set, METH_VARARGS },
	 { "BaseExample_get", _wrap_BaseExample_get, METH_VARARGS },
	 { "BaseExample_remove", _wrap_BaseExample_remove, METH_VARARGS },
	 { "BaseExample_clear", _wrap_BaseExample_clear, METH_VARARGS },
	 { "BaseExample_size", _wrap_BaseExample_size, METH_VARARGS },
	 { "BaseExample_read", _wrap_BaseExample_read, METH_VARARGS },
	 { "BaseExample_write", _wrap_BaseExample_write, METH_VARARGS },
	 { "BaseExample_readSVindex", _wrap_BaseExample_readSVindex, METH_VARARGS },
	 { "BaseExample_writeSVindex", _wrap_BaseExample_writeSVindex, METH_VARARGS },
	 { "delete_BaseExample", _wrap_delete_BaseExample, METH_VARARGS },
	 { "BaseExample_append", _wrap_BaseExample_append, METH_VARARGS },
	 { "BaseExample_appendSVindex", _wrap_BaseExample_appendSVindex, METH_VARARGS },
	 { "BaseExample_getDimension", _wrap_BaseExample_getDimension, METH_VARARGS },
	 { "BaseExample_getNonzeroDimension", _wrap_BaseExample_getNonzeroDimension, METH_VARARGS },
	 { "BaseExample_getY", _wrap_BaseExample_getY, METH_VARARGS },
	 { "BaseExample_getX", _wrap_BaseExample_getX, METH_VARARGS },
	 { "BaseExample_getAlpha", _wrap_BaseExample_getAlpha, METH_VARARGS },
	 { "BaseExample_getGradient", _wrap_BaseExample_getGradient, METH_VARARGS },
	 { "BaseExample_getG", _wrap_BaseExample_getG, METH_VARARGS },
	 { "Model_read", _wrap_Model_read, METH_VARARGS },
	 { "Model_write", _wrap_Model_write, METH_VARARGS },
	 { "Model_clear", _wrap_Model_clear, METH_VARARGS },
	 { "Model_classify", _wrap_Model_classify, METH_VARARGS },
	 { "Model_estimateMargin", _wrap_Model_estimateMargin, METH_VARARGS },
	 { "Model_estimateSphere", _wrap_Model_estimateSphere, METH_VARARGS },
	 { "Model_estimateVC", _wrap_Model_estimateVC, METH_VARARGS },
	 { "Model_estimateXA", _wrap_Model_estimateXA, METH_VARARGS },
	 { "Model_compress", _wrap_Model_compress, METH_VARARGS },
	 { "Model_getSVnum", _wrap_Model_getSVnum, METH_VARARGS },
	 { "Model_getBSVnum", _wrap_Model_getBSVnum, METH_VARARGS },
	 { "Model_getTrainingDataSize", _wrap_Model_getTrainingDataSize, METH_VARARGS },
	 { "Model_getLoss", _wrap_Model_getLoss, METH_VARARGS },
	 { "new_Model", _wrap_new_Model, METH_VARARGS },
	 { "delete_Model", _wrap_delete_Model, METH_VARARGS },
	 { "Example_read", _wrap_Example_read, METH_VARARGS },
	 { "Example_write", _wrap_Example_write, METH_VARARGS },
	 { "Example_rebuildSVindex", _wrap_Example_rebuildSVindex, METH_VARARGS },
	 { "Example_learn", _wrap_Example_learn, METH_VARARGS },
	 { "new_Example", _wrap_new_Example, METH_VARARGS },
	 { "delete_Example", _wrap_delete_Example, METH_VARARGS },
	 { NULL, NULL }
};

#ifdef __cplusplus
}
#endif

/* -------- TYPE CONVERSION AND EQUIVALENCE RULES (BEGIN) -------- */

static void *_p_ExampleTo_p_BaseExample(void *x) {
    return (void *)((BaseExample *) ((Example *) x));
}
static void *_p_ModelTo_p_BaseExample(void *x) {
    return (void *)((BaseExample *) ((Model *) x));
}
static swig_type_info _swigt__p_BaseExample[] = {{"_p_BaseExample", 0, "BaseExample *"},{"_p_BaseExample"},{"_p_Example", _p_ExampleTo_p_BaseExample},{"_p_Model", _p_ModelTo_p_BaseExample},{0}};
static swig_type_info _swigt__p_Example[] = {{"_p_Example", 0, "Example *"},{"_p_Example"},{0}};
static swig_type_info _swigt__p_Model[] = {{"_p_Model", 0, "Model *"},{"_p_Model"},{0}};

static swig_type_info *swig_types_initial[] = {
_swigt__p_BaseExample, 
_swigt__p_Example, 
_swigt__p_Model, 
0
};


/* -------- TYPE CONVERSION AND EQUIVALENCE RULES (END) -------- */

static swig_const_info swig_const_table[] = {
{0}};

static PyObject *SWIG_globals;
#ifdef __cplusplus
extern "C" 
#endif
SWIGEXPORT(void) initTinySVMc(void) {
    PyObject *m, *d;
    int i;
    SWIG_globals = SWIG_newvarlink();
    m = Py_InitModule("TinySVMc", TinySVMcMethods);
    d = PyModule_GetDict(m);
    for (i = 0; swig_types_initial[i]; i++) {
        swig_types[i] = SWIG_TypeRegister(swig_types_initial[i]);
    }
    SWIG_InstallConstants(d,swig_const_table);
}

