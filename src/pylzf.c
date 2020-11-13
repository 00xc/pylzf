#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include <liblzf-3.6/lzf.h>

#define MAX_COMPRESS_OVERSIZE		1.2
#define MAX_COMPRESS_OVERSIZE_SAFE	2
#define MAX_DECOMPRESS_OVERSIZE 	1.4

static PyObject* pylzf_compress(PyObject* self, PyObject* args) {
	Py_buffer in_obj;
	PyObject* out_obj;
	unsigned char* out;
	unsigned int aux;

	if (!PyArg_ParseTuple(args, "y*", &in_obj)) {
		return NULL;
	}

	/* Estimate maximum compression output length */
	if (in_obj.len > 7) {
		aux = lround(in_obj.len * MAX_COMPRESS_OVERSIZE);
	} else {
		aux = lround(in_obj.len * MAX_COMPRESS_OVERSIZE_SAFE);
	}

	if ( (out = malloc(aux)) == NULL ) {
		PyBuffer_Release(&in_obj);
		return PyErr_NoMemory();
	}

	/* Data compression */
	if ((aux = lzf_compress(in_obj.buf, in_obj.len, out, aux)) == 0) {
		free(out);
		PyBuffer_Release(&in_obj);
		PyErr_SetNone(PyExc_IOError);
		return NULL;
	}
	PyBuffer_Release(&in_obj);

	/* Build output object */
	if ( (out_obj = Py_BuildValue("y#", out, aux)) == NULL ) {
		free(out);
		return NULL;
	}
	free(out);

	return out_obj;
}

static PyObject* pylzf_decompress(PyObject* self, PyObject* args) {
	Py_buffer in_obj;
	PyObject* out_obj;
	unsigned char* out;
	unsigned int ret;

	if (!PyArg_ParseTuple(args, "y*", &in_obj)) {
		return NULL;
	}

	if ( (out = malloc(in_obj.len * MAX_DECOMPRESS_OVERSIZE)) == NULL ) {
		PyBuffer_Release(&in_obj);
		return PyErr_NoMemory();
	}

	/* Data decompression */
	if ((ret = lzf_decompress(in_obj.buf, in_obj.len, out, in_obj.len * MAX_DECOMPRESS_OVERSIZE)) == 0) {
		free(out);
		PyBuffer_Release(&in_obj);
		PyErr_SetNone(PyExc_IOError);
		return NULL;
	}
	PyBuffer_Release(&in_obj);

	/* Build output object */
	if ( (out_obj = Py_BuildValue("y#", out, ret)) == NULL ) {
		free(out);
		return NULL;
	}
	free(out);

	return out_obj;
}

/* Module functions */
static PyMethodDef Methods[] = {
	{"compress", pylzf_compress, METH_VARARGS, "Compress arbitrary data."},
	{"decompress",  pylzf_decompress, METH_VARARGS, "Decompress arbitrary data."},
	{NULL, NULL, 0, NULL}	/* Sentinel */
};

/* Module definition */
static struct PyModuleDef module = {
	.m_base = PyModuleDef_HEAD_INIT,
	.m_name = "pylzf",
	.m_doc = NULL,
	.m_size = -1,
	.m_methods = Methods,
	.m_slots = NULL
};

PyMODINIT_FUNC PyInit_pylzf() {
	return PyModule_Create(&module);
}