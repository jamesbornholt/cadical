#ifndef _util_hpp_INCLUDED
#define _util_hpp_INCLUDED

#include <vector>

namespace CaDiCaL {

using namespace std;

// Common simple utility functions independent from 'Internal'.

/*------------------------------------------------------------------------*/

inline double relative (double a, double b) { return b ? a / b : 0; }
inline double percent (double a, double b) { return relative (100 * a, b); }
inline int sign (int lit) { return (lit > 0) - (lit < 0); }
inline unsigned bign (int lit) { return 1 + (lit < 0); }

/*------------------------------------------------------------------------*/

bool parse_int_str (const char * str, int &);
bool has_suffix (const char * str, const char * suffix);

/*------------------------------------------------------------------------*/

inline bool is_power_of_two (unsigned n) { return n && !(n & (n-1)); }

inline bool contained (long c, long l, long u) { return l <= c && c <= u; }

/*------------------------------------------------------------------------*/

inline bool parity (unsigned a) {
  assert (sizeof a == 4);
  unsigned tmp = a;
  tmp ^= (tmp >> 16);
  tmp ^= (tmp >> 8);
  tmp ^= (tmp >> 4);
  tmp ^= (tmp >> 2);
  tmp ^= (tmp >> 1);
  return tmp & 1;
}

/*------------------------------------------------------------------------*/

// The standard 'Effective STL' way (though not guaranteed) to clear a
// vector and reduce its capacity to zero, thus deallocating all its
// internal memory.  This is quite important for keeping the actual
// allocated size of watched and occurrence lists small particularly during
// bounded variable elimination where many clauses are added and removed.

template<class T> void erase_vector (vector<T> & v) {
  if (v.capacity ()) { std::vector<T>().swap (v); }
  assert (!v.capacity ());                          // not guaranteed though
}

// The standard 'Effective STL' way (though not guaranteed) to shrink the
// capacity of a vector to its size thus kind of releasing all the internal
// excess memory not needed at the moment any more.

template<class T> void shrink_vector (vector<T> & v) {
  if (v.capacity () > v.size ()) { vector<T>(v).swap (v); }
  assert (v.capacity () == v.size ());              // not guaranteed though
}

/*------------------------------------------------------------------------*/

// These are options both to 'cadical' and 'mobical'.  After wasting some
// on not remembering the spelling (British vs American), nor singular vs
// plural and then wanted to use '--color=false', and '--colours=0' too, I
// just factored this out into these two utility functions.

bool is_color_option (const char * arg);    // --color, --colour, ...
bool is_no_color_option (const char * arg); // --no-color, --no-colour, ...

/*------------------------------------------------------------------------*/

}

#endif
