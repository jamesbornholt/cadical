#ifndef _cover_hpp_INCLUDED
#define _cover_hpp_INCLUDED

/*------------------------------------------------------------------------*/

// This header only provides the 'COVER' macro for testing.  It is unrelated
// to 'cover.cpp' which implements covered clause elimination, but we wanted
// to use the name base name in both cases.

/*------------------------------------------------------------------------*/

// Coverage goal, used similar to 'assert' (but with flipped condition) and
// also included even if 'NDEBUG' is defined (in optimizing compilation).
//
// This should in essence not be used in production code.
//
// There seems to be no problem overloading the name 'COVER' of this macro
// with the constant 'COVER' of 'Internal::Mode' (surprisingly).

#define COVER(COND) \
do { \
  if (!(COND)) break; \
  fprintf (stderr, \
    "%scadical%s: %s:%d: %s: Coverage goal %s`%s'%s reached.\n", \
    terr.bold_code (), terr.normal_code (), \
    __FUNCTION__, __LINE__, __FILE__, \
    terr.green_code (), # COND, terr.normal_code ()); \
  fflush (stderr); \
  abort (); \
} while (0)

#endif
