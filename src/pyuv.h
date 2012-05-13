
#ifndef PYUV_H
#define PYUV_H

/* python */
#include "Python.h"
#include "structmember.h"
#include "structseq.h"

/* Python3 */
#if PY_MAJOR_VERSION >= 3
    #define PYUV_PYTHON3
    #define PyInt_FromSsize_t PyLong_FromSsize_t
    #define PyInt_FromLong PyLong_FromLong
    #define PyString_FromString PyBytes_FromString
    #define PyString_AsString PyBytes_AsString
    #define PyString_FromStringAndSize PyBytes_FromStringAndSize
    #define PyString_Check PyBytes_Check
    #define PyString_Size PyBytes_Size
    #define PyString_AS_STRING PyBytes_AS_STRING
    #define PyString_GET_SIZE PyBytes_GET_SIZE
    /* helpers, to avoid too many ifdefs */
    #define PYUVString_FromString PyUnicode_FromString
    #define PYUVString_FromStringAndSize PyUnicode_FromStringAndSize
#else
    /* helpers, to avoid too many ifdefs */
    #define PYUVString_FromString PyString_FromString
    #define PYUVString_FromStringAndSize PyString_FromStringAndSize
#endif

/* libuv */
#include "uv.h"


/* Custom types */
typedef int Bool;
#define True  1
#define False 0


/* Utility macros */
#ifndef __STRING
    #define __STRING(x) #x
#endif
#define __MSTR(x) __STRING(x)

#define UNUSED_ARG(arg)  (void)arg

#define ASSERT(x)                                                           \
    do {                                                                    \
        if (!(x)) {                                                         \
            fprintf (stderr, "%s:%u: %s: Assertion `" #x "' failed.\n",     \
                     __FILE__, __LINE__, __func__);                         \
            abort();                                                        \
        }                                                                   \
    } while(0)                                                              \

#define UV_LOOP(x) x->loop->uv_loop

#if defined(__MINGW32__) || defined(_MSC_VER)
    #define PYUV_WINDOWS
#endif


/* Python types definitions */

/* Loop */
typedef struct {
    PyObject_HEAD
    PyObject *weakreflist;
    PyObject *data;
    uv_loop_t *uv_loop;
    int is_default;
} Loop;

static PyTypeObject LoopType;

/* Async */
typedef struct {
    PyObject_HEAD
    PyObject *weakreflist;
    Loop *loop;
    PyObject *on_close_cb;
    PyObject *callback;
    PyObject *data;
    uv_async_t *uv_handle;
} Async;

static PyTypeObject AsyncType;

/* Timer */
typedef struct {
    PyObject_HEAD
    PyObject *weakreflist;
    Loop *loop;
    PyObject *on_close_cb;
    PyObject *callback;
    PyObject *data;
    uv_timer_t *uv_handle;
} Timer;

static PyTypeObject TimerType;

/* Prepare */
typedef struct {
    PyObject_HEAD
    PyObject *weakreflist;
    Loop *loop;
    PyObject *on_close_cb;
    PyObject *callback;
    PyObject *data;
    uv_prepare_t *uv_handle;
} Prepare;

static PyTypeObject PrepareType;

/* Idle */
typedef struct {
    PyObject_HEAD
    PyObject *weakreflist;
    Loop *loop;
    PyObject *on_close_cb;
    PyObject *callback;
    PyObject *data;
    uv_idle_t *uv_handle;
} Idle;

static PyTypeObject IdleType;

/* Check */
typedef struct {
    PyObject_HEAD
    PyObject *weakreflist;
    Loop *loop;
    PyObject *on_close_cb;
    PyObject *callback;
    PyObject *data;
    uv_check_t *uv_handle;
} Check;

static PyTypeObject CheckType;

/* Signal */
typedef struct {
    PyObject_HEAD
    PyObject *weakreflist;
    Loop *loop;
    PyObject *on_close_cb;
    PyObject *data;
    uv_prepare_t *uv_handle;
} Signal;

static PyTypeObject SignalType;

/* IOStream */
typedef struct {
    PyObject_HEAD
    PyObject *weakreflist;
    Loop *loop;
    PyObject *on_read_cb;
    PyObject *on_close_cb;
    PyObject *data;
    uv_stream_t *uv_handle;
} IOStream;

static PyTypeObject IOStreamType;

/* TCP */
typedef struct {
    IOStream iostream;
    PyObject *on_new_connection_cb;
} TCP;

static PyTypeObject TCPType;

/* Pipe */
typedef struct {
    IOStream iostream;
    PyObject *on_new_connection_cb;
} Pipe;

static PyTypeObject PipeType;

/* TTY */
typedef struct {
    IOStream iostream;
} TTY;

static PyTypeObject TTYType;

/* UDP */
typedef struct {
    PyObject_HEAD
    PyObject *weakreflist;
    Loop *loop;
    PyObject *on_read_cb;
    PyObject *on_close_cb;
    PyObject *data;
    uv_udp_t *uv_handle;
} UDP;

static PyTypeObject UDPType;

/* Poll */
typedef struct {
    PyObject_HEAD
    PyObject *weakreflist;
    Loop *loop;
    PyObject *callback;
    PyObject *on_close_cb;
    PyObject *data;
    uv_poll_t *uv_handle;
    long fd;
} Poll;

static PyTypeObject PollType;

/* DNSResolver */
typedef struct {
    PyObject_HEAD
    Loop *loop;
    PyObject *data;
    ares_channel channel;
} DNSResolver;

static PyTypeObject DNSResolverType;

/* ThreadPool */
typedef struct {
    PyObject_HEAD
    Loop *loop;
} ThreadPool;

static PyTypeObject ThreadPoolType;

/* Process */
typedef struct {
    PyObject_HEAD
    PyObject *weakreflist;
    Loop *loop;
    PyObject *on_close_cb;
    PyObject *on_exit_cb;
    PyObject *stdin_pipe;
    PyObject *stdout_pipe;
    PyObject *stderr_pipe;
    PyObject *data;
    uv_process_t *uv_handle;
} Process;

static PyTypeObject ProcessType;

/* FSEvent */
typedef struct {
    PyObject_HEAD
    PyObject *weakreflist;
    Loop *loop;
    PyObject *on_fsevent_cb;
    PyObject *on_close_cb;
    PyObject *data;
    uv_fs_event_t *uv_handle;
} FSEvent;

static PyTypeObject FSEventType;


/* Some helper stuff */

/* Temporary hack: libuv should provide uv_inet_pton and uv_inet_ntop. */
#ifdef PYUV_WINDOWS
    #include <inet_net_pton.h>
    #include <inet_ntop.h>
    #define uv_inet_pton ares_inet_pton
    #define uv_inet_ntop ares_inet_ntop
#else /* __POSIX__ */
    #include <arpa/inet.h>
    #define uv_inet_pton inet_pton
    #define uv_inet_ntop inet_ntop
#endif


/* Add a type to a module */
static int
PyUVModule_AddType(PyObject *module, const char *name, PyTypeObject *type)
{
    if (PyType_Ready(type)) {
        return -1;
    }
    Py_INCREF(type);
    if (PyModule_AddObject(module, name, (PyObject *)type)) {
        Py_DECREF(type);
        return -1;
    }
    return 0;
}


/* Add a type to a module */
static int
PyUVModule_AddObject(PyObject *module, const char *name, PyObject *value)
{
    Py_INCREF(value);
    if (PyModule_AddObject(module, name, value)) {
        Py_DECREF(value);
        return -1;
    }
    return 0;
}


#if defined(__GNUC__) && !defined(__STRICT_ANSI__)
    #define INLINE inline
#else
    #define INLINE
#endif

/* Raise appropriate exception when an error is produced inside libuv */
static INLINE void
raise_uv_exception(Loop *loop, PyObject *exc_type)
{
    uv_err_t err = uv_last_error(loop->uv_loop);
    PyObject *exc_data = Py_BuildValue("(is)", err.code, uv_strerror(err));
    if (exc_data != NULL) {
        PyErr_SetObject(exc_type, exc_data);
        Py_DECREF(exc_data);
    }
}

/* borrowed from pyev */
#ifdef PYUV_WINDOWS
/* avoid including socketmodule.h (not available anyway) */
typedef struct {
    PyTypeObject *Sock_Type;
    PyObject *error;
#if PY_MAJOR_VERSION >= 3 && PY_MINOR_VERSION >= 2
    PyObject *timeout_error;
#endif
} PySocketModule_APIObject;

static PySocketModule_APIObject PySocketModule;

#define PYUV_MAXSTDIO 2048

/* Convert a Windows handle into a usable file descriptor */
#define PYUV_WIN32_HANDLE_TO_FD(handle) _open_osfhandle (handle, 0)
#define PYUV_FD_TO_WIN32_HANDLE(fd) _get_osfhandle (fd)

#endif

#endif


