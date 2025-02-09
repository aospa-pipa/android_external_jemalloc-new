#ifndef JEMALLOC_TEST_H
#define JEMALLOC_TEST_H

#ifdef __cplusplus
extern "C" {
#endif

#include <limits.h>
#ifndef SIZE_T_MAX
#  define SIZE_T_MAX	SIZE_MAX
#endif
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <errno.h>
#include <math.h>
#include <string.h>
#ifdef _WIN32
#  include "msvc_compat/strings.h"
#endif

#ifdef _WIN32
#  include <windows.h>
#  include "msvc_compat/windows_extra.h"
#else
#  include <pthread.h>
#endif

#include "test/jemalloc_test_defs.h"

#if defined(JEMALLOC_OSATOMIC)
#  include <libkern/OSAtomic.h>
#endif

#if defined(HAVE_ALTIVEC) && !defined(__APPLE__)
#  include <altivec.h>
#endif
#ifdef HAVE_SSE2
#  include <emmintrin.h>
#endif

/******************************************************************************/
/*
 * For unit tests and analytics tests, expose all public and private interfaces.
 */
#if defined(JEMALLOC_UNIT_TEST) || defined (JEMALLOC_ANALYZE_TEST)
#  define JEMALLOC_JET
#  define JEMALLOC_MANGLE
#  include "jemalloc/internal/jemalloc_preamble.h"
#  include "jemalloc/internal/jemalloc_internal_includes.h"

/******************************************************************************/
/*
 * For integration tests, expose the public jemalloc interfaces, but only
 * expose the minimum necessary internal utility code (to avoid re-implementing
 * essentially identical code within the test infrastructure).
 */
#elif defined(JEMALLOC_INTEGRATION_TEST) || \
    defined(JEMALLOC_INTEGRATION_CPP_TEST)
#  define JEMALLOC_MANGLE
#  include "jemalloc/jemalloc.h"
#  include "jemalloc/internal/jemalloc_internal_defs.h"
#  include "jemalloc/internal/jemalloc_internal_macros.h"
#  include "jemalloc/internal/jemalloc_preamble.h"

#  define JEMALLOC_N(n) je_##n
#  include "jemalloc/internal/private_namespace.h"
#  include "jemalloc/internal/test_hooks.h"

/* Hermetic headers. */
#  include "jemalloc/internal/assert.h"
#  include "jemalloc/internal/malloc_io.h"
#  include "jemalloc/internal/nstime.h"
#  include "jemalloc/internal/util.h"

/* Non-hermetic headers. */
#  include "jemalloc/internal/qr.h"
#  include "jemalloc/internal/ql.h"

/******************************************************************************/
/*
 * For stress tests, expose the public jemalloc interfaces with name mangling
 * so that they can be tested as e.g. malloc() and free().  Also expose the
 * public jemalloc interfaces with jet_ prefixes, so that stress tests can use
 * a separate allocator for their internal data structures.
 */
#elif defined(JEMALLOC_STRESS_TEST) || \
    defined(JEMALLOC_STRESS_CPP_TEST)
#  include "jemalloc/jemalloc.h"

#  include "jemalloc/jemalloc_protos_jet.h"

#  define JEMALLOC_JET
#  include "jemalloc/internal/jemalloc_preamble.h"
#  include "jemalloc/internal/jemalloc_internal_includes.h"
#  include "jemalloc/internal/public_unnamespace.h"
#  undef JEMALLOC_JET

#  include "jemalloc/jemalloc_rename.h"
#  define JEMALLOC_MANGLE
#  ifdef JEMALLOC_STRESS_TESTLIB
#    include "jemalloc/jemalloc_mangle_jet.h"
#  else
#    include "jemalloc/jemalloc_mangle.h"
#  endif

/******************************************************************************/
/*
 * This header does dangerous things, the effects of which only test code
 * should be subject to.
 */
#else
#  error "This header cannot be included outside a testing context"
#endif

/******************************************************************************/
/*
 * Common test utilities.
 */
#include "test/btalloc.h"
#include "test/math.h"
#include "test/mtx.h"
#include "test/mq.h"
#include "test/sleep.h"
#include "test/test.h"
#include "test/timer.h"
#include "test/thd.h"
#include "test/bgthd.h"
#define MEXP 19937
#include "test/SFMT.h"

#ifndef JEMALLOC_HAVE_MALLOC_SIZE
#define TEST_MALLOC_SIZE malloc_usable_size
#else
#define TEST_MALLOC_SIZE malloc_size
#endif
/******************************************************************************/
/*
 * Define always-enabled assertion macros, so that test assertions execute even
 * if assertions are disabled in the library code.
 */
#undef assert
#undef not_reached
#undef not_implemented
#undef expect_not_implemented

#define assert(e) do {							\
	if (!(e)) {							\
		malloc_printf(						\
		    "<jemalloc>: %s:%d: Failed assertion: \"%s\"\n",	\
		    __FILE__, __LINE__, #e);				\
		abort();						\
	}								\
} while (0)

#define not_reached() do {						\
	malloc_printf(							\
	    "<jemalloc>: %s:%d: Unreachable code reached\n",		\
	    __FILE__, __LINE__);					\
	abort();							\
} while (0)

#define not_implemented() do {						\
	malloc_printf("<jemalloc>: %s:%d: Not implemented\n",		\
	    __FILE__, __LINE__);					\
	abort();							\
} while (0)

#define expect_not_implemented(e) do {					\
	if (!(e)) {							\
		not_implemented();					\
	}								\
} while (0)

#ifdef __cplusplus
}
#endif

#endif
