/*
 * Used only for upstream/src/microham.c, which scans /dev with glob() to find microHAM
 * keyers. Android's C library has glob() only from Android 9 (API 28), and Android apps
 * cannot list /dev anyway, so below API 28 glob() finds nothing.
 */
#pragma once
#include <glob.h>
#if defined(__ANDROID_API__) && __ANDROID_API__ < 28
static inline int android_glob_none(const char *pattern, int flags,
                                    int (*errfunc)(const char *, int), glob_t *g)
{
    (void) pattern; (void) flags; (void) errfunc;
    g->gl_pathc = 0;
    g->gl_pathv = 0;
    return GLOB_NOMATCH;
}
static inline void android_globfree_none(glob_t *g) { (void) g; }
#define glob android_glob_none
#define globfree android_globfree_none
#endif
