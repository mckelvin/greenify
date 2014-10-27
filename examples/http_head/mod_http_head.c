#include <Python.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>


int c_http_head(const char *host)
{
    struct addrinfo *p, hints, *res;
    char request[64];
    char response[2049];
    int socket_fd;

    sprintf(request, "HEAD / HTTP/1.1\nhost: %s\n\n", host);

    memset(&hints, 0, sizeof(hints));

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    getaddrinfo(host, "80", &hints, &res);
    for(p = res; p; p = p->ai_next)
    {
        socket_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (connect(socket_fd, p->ai_addr, p->ai_addrlen) == 0) {
            // fprintf(stderr, "[%s] connected %s\n", __FUNCTION__, host);
            break;
        }
    }

    send(socket_fd, request, strlen(request), 0);
    recv(socket_fd, response, 2048, 0);
    close(socket_fd);
    // fprintf(stderr, "[%s] closed %s\n", __FUNCTION__, host);
    // fprintf(stderr, "%s\n", response);
    if (strstr(response, "HTTP/1.1") != NULL) {
        return 1;
    }
    return 0;
}


static PyObject*
http_head(PyObject* self, PyObject* args)
{
    const char* host;
    if (!PyArg_ParseTuple(args, "s", &host))
    {
        return NULL;
    }
    long ret = c_http_head(host);
    return PyInt_FromLong(ret);
}


static PyMethodDef ModHttpHeadMethods[] =
{
    {"http_head", http_head, METH_VARARGS, "A naive http head test."},
    {NULL, NULL, 0, NULL}
};


PyMODINIT_FUNC
initmod_http_head(void)
{
    (void) Py_InitModule("mod_http_head", ModHttpHeadMethods);
}
