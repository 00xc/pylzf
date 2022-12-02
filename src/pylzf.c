#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include <liblzf-3.6/lzf.h>

/*
 * These values define how big we expect the result to be as a function
 * of the size of the input data, i.e., given N bytes of uncompressed
 * data, we expect the compressed data to have a max. size of
 * N * MAX_COMPRESS_OVERSIZE. For exceptionally small sizes, we use
 * MAX_COMPRESS_OVERSIZE_SAFE. For decompression there is not a maximum
 * value, but an initial one that will be increased over several
 * iterations (MAX_DECOMPRESS_RETRIES at most).
 */
#define MAX_COMPRESS_OVERSIZE		1.2
#define MAX_COMPRESS_OVERSIZE_SAFE	2
#define INITIAL_DECOMPRESS_OVERSIZE 	1.5
#define MAX_DECOMPRESS_RETRIES		15

#define THREADSAFE_CLEANUP(ptr, buf) \
	if (ptr) { free(ptr); } \
	Py_BLOCK_THREADS \
	PyBuffer_Release(buf); \

static PyObject* pylzf_compress(PyObject* self, PyObject* args) {
	Py_buffer in_obj;
	PyObject* out_obj;
	unsigned char* out;
	unsigned int aux;

	if (!PyArg_ParseTuple(args, "y*", &in_obj)) {
		return NULL;
	}

	/* GIL-free section */
	Py_BEGIN_ALLOW_THREADS

		/* Estimate maximum compression output size */
		if (in_obj.len > 7) {
			aux = lround(in_obj.len * MAX_COMPRESS_OVERSIZE);
		} else {
			aux = lround(in_obj.len * MAX_COMPRESS_OVERSIZE_SAFE);
		}

		if ( (out = malloc(aux)) == NULL ) {
			THREADSAFE_CLEANUP(NULL, &in_obj)
			return PyErr_NoMemory();
		}

		/* Data compression */
		if ((aux = lzf_compress(in_obj.buf, in_obj.len, out, aux)) == 0) {
			THREADSAFE_CLEANUP(out, &in_obj)
			PyErr_SetNone(PyExc_IOError);
			return NULL;
		}

	Py_END_ALLOW_THREADS
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
	unsigned int aux = 0, i = 0;

	if (!PyArg_ParseTuple(args, "y*", &in_obj)) {
		return NULL;
	}

	/* GIL-free section */
	Py_BEGIN_ALLOW_THREADS

		/* 
		 * This loop keeps trying to allocate bigger and bigger
		 * buffers until one is big enough to decompress all the data.
		 * For example, for N bytes of compressed data, the tried
		 * values will be:
		 * i = 0    N * INITIAL_DECOMPRESS_OVERSIZE
		 * i = 1    N * (INITIAL_DECOMPRESS_OVERSIZE * 2)
		 * i = 2    N * (INITIAL_DECOMPRESS_OVERSIZE * 4)
		 * i = 3    N * (INITIAL_DECOMPRESS_OVERSIZE * 8)
		 * etc.
		 */
		while (aux == 0 && i++ < MAX_DECOMPRESS_RETRIES) {

			aux = in_obj.len * (INITIAL_DECOMPRESS_OVERSIZE * (1 << i));

			if ( (out = malloc(aux)) == NULL ) {
				THREADSAFE_CLEANUP(NULL, &in_obj)
				return PyErr_NoMemory();
			}

			if ( (aux = lzf_decompress(in_obj.buf, in_obj.len, out, aux)) == 0 ) {
				free(out);
			}
		}

		/* Max. retries reached */
		if (aux == 0) {
			THREADSAFE_CLEANUP(NULL, &in_obj)
			PyErr_SetNone(PyExc_IOError);
			return NULL;
		}

	Py_END_ALLOW_THREADS
	PyBuffer_Release(&in_obj);

	/* Build output object */
	if ( (out_obj = Py_BuildValue("y#", out, aux)) == NULL ) {
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
