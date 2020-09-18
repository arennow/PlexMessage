#ifndef asprintf_h
#define asprintf_h

#if defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
#include <sys/param.h>
#if defined(BSD)

// This is defined in GNU and BSD versions of libc, but there are weird include guards around it on OS X (and perhaps other BSDs), so this.
int asprintf(char ** __restrict, const char * __restrict, ...) __printflike(2, 3);

#endif
#endif

#endif