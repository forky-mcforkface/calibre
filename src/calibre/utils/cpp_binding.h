/*
 * Copyright (C) 2021 Kovid Goyal <kovid at kovidgoyal.net>
 *
 * Distributed under terms of the GPL3 license.
 */

#pragma once

#define PY_SSIZE_T_CLEAN
#define UNICODE
#define _UNICODE
#include <Python.h>
#include <wchar.h>

#define arraysz(x) (sizeof(x)/sizeof(x[0]))

template<typename T, void free_T(void*), T null=reinterpret_cast<T>(NULL)>
class generic_raii {
	private:
		T handle;
		generic_raii( const generic_raii & ) ;
		generic_raii & operator=( const generic_raii & ) ;

	public:
		explicit generic_raii(T h = null) : handle(h) {}
		~generic_raii() { release(); }

		void release() {
			if (handle != null) {
				free_T(handle);
				handle = null;
			}
		}

		T ptr() { return handle; }
		T detach() { T ans = handle; handle = null; return ans; }
		void attach(T val) { release(); handle = val; }
		T* address() { return &handle; }
		explicit operator bool() const { return handle != null; }
		T* operator &() { return &handle; }
};

typedef generic_raii<wchar_t*, PyMem_Free> wchar_raii;
static inline void python_object_destructor(void *p) { PyObject *x = reinterpret_cast<PyObject*>(p); Py_XDECREF(x); }
typedef generic_raii<PyObject*, python_object_destructor> pyobject_raii;


static inline int
py_to_wchar(PyObject *obj, wchar_t **output) {
	if (!PyUnicode_Check(obj)) {
		if (obj == Py_None) { return 1; }
		PyErr_SetString(PyExc_TypeError, "unicode object expected");
		return 0;
	}
    wchar_t *buf = PyUnicode_AsWideCharString(obj, NULL);
    if (!buf) { PyErr_NoMemory(); return 0; }
	*output = buf;
	return 1;
}

static inline int
py_to_wchar_no_none(PyObject *obj, wchar_t **output) {
	if (!PyUnicode_Check(obj)) {
		PyErr_SetString(PyExc_TypeError, "unicode object expected");
		return 0;
	}
    wchar_t *buf = PyUnicode_AsWideCharString(obj, NULL);
    if (!buf) { PyErr_NoMemory(); return 0; }
	*output = buf;
	return 1;
}
