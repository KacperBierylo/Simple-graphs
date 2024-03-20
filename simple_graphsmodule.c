#include <Python.h>
#define SIZE 16
typedef struct {
    PyObject_HEAD
    uint16_t vertices;
    uint16_t edges[16];
    int num_vertices;
    int num_edges;
} AdjacencyMatrix;

static void AdjacencyMatrix_dealloc(AdjacencyMatrix* self) {
    Py_TYPE(self)->tp_free((PyObject*)self);
}

static PyObject* AdjacencyMatrix_new(PyTypeObject* type, PyObject* args, PyObject* kwds) {
    AdjacencyMatrix* self;
    self = (AdjacencyMatrix*)type->tp_alloc(type, 0);
    if (self != NULL) {
    }
    return (PyObject*)self;
}

int AdjacencyMatrix_init(AdjacencyMatrix* self, PyObject* args) {
    char *text = "?";
    self->vertices = 0;
    for(int i = 0; i < SIZE; i++){
        self->edges[i] = 0;
    }
    if (!PyArg_ParseTuple(args, "|s", &text)){
        return -1;
}
    int num_vertices = (int)text[0] - 63;
    self->num_vertices = num_vertices;
    self->num_edges = 0;
    int k = 0;
    int i = 1;
    for (int v = 0; v < num_vertices; v++) {
        self->vertices = self->vertices | (1 << v);
        for (int u = 0; u < v; u++) {
            if (k == 0) {
                i++;
                k = 6;
            }
            k--;
            int bit = (int)((text[i - 1] - 63) & (1 << k));
            if (bit != 0) {
                self->edges[u] = self->edges[u] | (1 << v);
                self->edges[v] = self->edges[v] | (1 << u);
                self->num_edges++;
            }
        }
    }
     return 1;
}

static PyObject* AdjacencyMatrix_number_of_vertices(AdjacencyMatrix* self, PyObject* args) {
    return PyLong_FromLong(self->num_vertices);
}

static PyObject* AdjacencyMatrix_vertices(AdjacencyMatrix* self, PyObject* args) {
    PyObject* set = PySet_New(NULL);
    for (int i = 0; i < SIZE; i++) {
        if(self->vertices & 1 << i){
           PyObject * value = Py_BuildValue("i", i);
           PySet_Add(set, value);
           Py_DECREF(value);
        }
    }
    return set;
}

static PyObject* AdjacencyMatrix_vertex_degree(AdjacencyMatrix* self, PyObject* args) {
    int vertex;
    if (!PyArg_ParseTuple(args, "i", &vertex)){
        Py_RETURN_NONE;
    }
    int degree = 0;
    int liczba = self->edges[vertex];

    while (liczba) {
        degree += liczba & 1;
        liczba >>= 1;
    }
    return PyLong_FromLong(degree);
}

static PyObject* AdjacencyMatrix_vertex_neighbors(AdjacencyMatrix* self, PyObject* args) {
    int vertex;
    if (!PyArg_ParseTuple(args, "i", &vertex)){
        Py_RETURN_NONE;
    }
    PyObject* set = PySet_New(NULL);

    int liczba = self->edges[vertex];
    int sasiad = 0;
    int numer = 0;
    while (liczba) {
        sasiad = liczba & 1;
        if(sasiad){
         PyObject* value = Py_BuildValue("i", numer);
         PySet_Add(set,value);
         Py_DECREF(value);
         }
        numer++;
        liczba >>= 1;
    }
     return set;
}

static PyObject* AdjacencyMatrix_add_vertex(AdjacencyMatrix* self, PyObject* args) {
    int vertex;
    if (!PyArg_ParseTuple(args, "i", &vertex)){
        Py_RETURN_NONE;
    }
    self->vertices = self->vertices | (1 << vertex);
    return PyLong_FromLong(1);
}

static PyObject* AdjacencyMatrix_delete_vertex(AdjacencyMatrix* self, PyObject* args) {
    int vertex;
    if (!PyArg_ParseTuple(args, "i", &vertex)){
        Py_RETURN_NONE;
    }
    self->vertices = self->vertices & ~(1 << vertex);
    self->edges[vertex] = 0;

    for(int i=0;i<SIZE;i++){
        self->edges[i] = self->edges[i] & ~(1 << vertex);
    }
    return PyLong_FromLong(1);
}

static PyObject* AdjacencyMatrix_number_of_edges(AdjacencyMatrix* self, PyObject* args) {
    return PyLong_FromLong(self->num_edges);
}

static PyObject* AdjacencyMatrix_edges(AdjacencyMatrix* self, PyObject* args) {
    PyObject* set = PySet_New(NULL);
    for (int i = 0; i < SIZE; i++) {
        for (int j = i + 1; j < SIZE; j++) {
            if (self->edges[i] & (1 << j)) {
                PyObject* packed = PyTuple_Pack(2, PyLong_FromLong(i), PyLong_FromLong(j));
                PySet_Add(set, packed);
                Py_DECREF(packed);
            }
        }
    }
    return set;
}

static PyObject* AdjacencyMatrix_is_edge(AdjacencyMatrix* self, PyObject* args) {
    int u, v;
    if (!PyArg_ParseTuple(args, "ii", &u, &v)){
        Py_RETURN_NONE;
    }
    if((self->edges[u] & (1 << v) ) /*|| (self->edges[v] & (1 << u))*/){
        Py_RETURN_TRUE;
      }
    Py_RETURN_FALSE;
}

static PyObject* AdjacencyMatrix_add_edge(AdjacencyMatrix* self, PyObject* args) {
    int u, v;
    if (!PyArg_ParseTuple(args, "ii", &u, &v)){
        Py_RETURN_NONE;
    }

    self->edges[u] = self->edges[u] | (1 << v);
    self->edges[v] = self->edges[v] | (1 << u);

     self->num_edges++;
     return PyLong_FromLong(1);
}

static PyObject* AdjacencyMatrix_delete_edge(AdjacencyMatrix* self, PyObject* args) {
    int u, v;
    if (!PyArg_ParseTuple(args, "ii", &u, &v)){
        return NULL;
    }
    self->edges[u] = self->edges[u] & ~(1 << v);
    self->edges[v] = self->edges[v] & ~(1 << u);
    self->num_edges--;
     return PyLong_FromLong(1);
}

static PyObject* AdjacencyMatrix_eq(AdjacencyMatrix *self, PyObject *args) {

    AdjacencyMatrix *other;
    if (!PyArg_ParseTuple(args, "O", &other)) {
        Py_RETURN_NONE;
    }

    if(self->num_edges != other->num_edges) Py_RETURN_FALSE;
    if(self->num_vertices != other->num_vertices) Py_RETURN_FALSE;

    if(self->vertices!=other->vertices) Py_RETURN_FALSE;

    for(int i=0; i<SIZE;i++){
        if(self->edges[i]!=other->edges[i]) Py_RETURN_FALSE;
    }
    Py_RETURN_TRUE;
}

static PyObject* AdjacencyMatrix_ne(AdjacencyMatrix *self, PyObject *args) {

    AdjacencyMatrix *other;
    if (!PyArg_ParseTuple(args, "O", &other)) {
        Py_RETURN_NONE;
    }

    if(self->num_edges != other->num_edges) Py_RETURN_TRUE;
    if(self->num_vertices != other->num_vertices) Py_RETURN_TRUE;

    if(self->vertices!=other->vertices) Py_RETURN_TRUE;

    for(int i=0; i<SIZE;i++){
        if(self->edges[i]!=other->edges[i]) Py_RETURN_TRUE;
    }
    Py_RETURN_FALSE;
}

int color_component(AdjacencyMatrix* self, int vertex, int color, int *cs){
 cs[vertex] = color;
    for (int v = 0; v < SIZE; v++) {
        if (self->edges[v] & (1 << vertex)) {
            if (cs[v] == color) {
                return 0;
            }
            if (cs[v] == 0) {
                if (!color_component(self, v, 3 - color, cs)) {
                    return 0;
                }
            }
        }
    }
    return 1;
}

static PyObject* AdjacencyMatrix_is_bipartite(AdjacencyMatrix* self, PyObject* args) {
  int cs[SIZE] = {0};
    for (int v = 0; v < SIZE; v++) {
        if (cs[v] == 0) {
            if (!color_component(self, v, 1, cs)) {
                Py_RETURN_FALSE;
            }
        }
    }
    Py_RETURN_TRUE;
}
static PyMethodDef AdjacencyMatrix_methods[] = {
    {"number_of_vertices", (PyCFunction)AdjacencyMatrix_number_of_vertices, METH_NOARGS, "Return number of vertices of the graph."},
    {"vertices", (PyCFunction)AdjacencyMatrix_vertices, METH_NOARGS, "Return set of vertices of the graph."},
    {"vertex_degree", (PyCFunction)AdjacencyMatrix_vertex_degree, METH_VARARGS, "Return vertex degree."},
    {"vertex_neighbors", (PyCFunction)AdjacencyMatrix_vertex_neighbors, METH_VARARGS, "Return given vertex's neighbours."},
    {"add_vertex", (PyCFunction)AdjacencyMatrix_add_vertex, METH_VARARGS, "Adds given vertex to graph"},
    {"delete_vertex", (PyCFunction)AdjacencyMatrix_delete_vertex, METH_VARARGS, "Deletes given vertex from graph"},
    {"number_of_edges", (PyCFunction)AdjacencyMatrix_number_of_edges, METH_NOARGS, "Return number of edges of the graph."},
    {"edges", (PyCFunction)AdjacencyMatrix_edges, METH_NOARGS, "Return edges of the graph"},
    {"is_edge", (PyCFunction)AdjacencyMatrix_is_edge, METH_VARARGS, "Checks if given vertex makes an edge."},
    {"add_edge", (PyCFunction)AdjacencyMatrix_add_edge, METH_VARARGS, "Add edge to graph."},
    {"delete_edge", (PyCFunction)AdjacencyMatrix_delete_edge, METH_VARARGS, "Delete edge from graph."},
    {"__eq__", (PyCFunction) AdjacencyMatrix_eq, METH_VARARGS, "Compare two Graph objects (if same)"},
    {"__ne__", (PyCFunction) AdjacencyMatrix_ne, METH_VARARGS, "Compare two Graph objects (if different)"},
    {"is_bipartite", (PyCFunction)AdjacencyMatrix_is_bipartite, METH_NOARGS, "Return True if graph is bipartite, False otherwise."},
    {NULL}
};

static PyTypeObject AdjacencyMatrixType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "simple_graphs.AdjacencyMatrix",              /* tp_name */
    sizeof(AdjacencyMatrix),                /* tp_basicsize */
    0,                              /* tp_itemsize */
    (destructor)AdjacencyMatrix_dealloc,    /* tp_dealloc */
    0,                              /* tp_print */
    0,                              /* tp_getattr */
    0,                              /* tp_setattr */
    0,                              /* tp_reserved */
    0,                              /* tp_repr */
    0,                              /* tp_as_number */
    0,                              /* tp_as_sequence */
    0,                              /* tp_as_mapping */
    0,                              /* tp_hash  */
    0,                              /* tp_call */
    0,                              /* tp_str */
    0,                              /* tp_getattro */
    0,                              /* tp_setattro */
    0,                              /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT,             /* tp_flags */
    "AdjacencyMatrix object",               /* tp_doc */
    0,                              /* tp_traverse */
    0,                              /* tp_clear */
    0,                              /* tp_richcompare */
    0,                              /* tp_weaklistoffset */
    0,                              /* tp_iter */
    0,                              /* tp_iternext */
    AdjacencyMatrix_methods,                /* tp_methods */
    0,                              /* tp_members */
    0,                              /* tp_getset */
    0,                              /* tp_base */
    0,                              /* tp_dict */
    0,                              /* tp_descr_get */
    0,                              /* tp_descr_set */
    0,                              /* tp_dictoffset */
    (initproc)AdjacencyMatrix_init,         /* tp_init */
    0,                              /* tp_alloc */
    AdjacencyMatrix_new,                    /* tp_new */
};

static PyModuleDef simple_graphs_module = {
    PyModuleDef_HEAD_INIT,
    "simple_graphs",
    "Example module that creates a Graph type.",
    -1,
    NULL, NULL, NULL, NULL, NULL
};

PyMODINIT_FUNC PyInit_simple_graphs(void) {
    PyObject* m;

    if (PyType_Ready(&AdjacencyMatrixType) < 0)
        Py_RETURN_NONE;

    m = PyModule_Create(&simple_graphs_module);
    if (m == NULL)
    Py_RETURN_NONE;

Py_INCREF(&AdjacencyMatrixType);
PyModule_AddObject(m, "AdjacencyMatrix", (PyObject*)&AdjacencyMatrixType);
return m;
}