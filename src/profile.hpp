#ifndef _profiles_h_INCLUDED
#define _profiles_h_INCLUDED

/*------------------------------------------------------------------------*/
#ifndef QUIET
/*------------------------------------------------------------------------*/

namespace CaDiCaL {

struct Internal;

/*------------------------------------------------------------------------*/

// The solver contains some built in profiling (even for optimized code).
// The idea is that even without using external tools it is possible to get
// an overview of where time is spent.  This is enabled with the option
// 'profile', e.g., you might want to use '--profile=3', or even higher
// values for more detailed profiling information.  Currently the default is
// '--profile=2', which should only induce a tiny profiling overhead.
//
// Profiling has a Heisenberg effect, since we rely on calling 'getrusage'
// instead of using profile counters and sampling.  For functions which are
// executed many times, this overhead is substantial (say 10%-20%).  For
// functions which are not executed many times there is in essence no
// overhead in measuring time spent in them.  These get a smaller profiling
// level, which is the second argument in the 'PROFILE' macro below.  Thus
// using '--profile=1' for instance should not add any penalty to the
// run-time, while '--profile=3' and higher levels slow down the solver.
//
// To profile say 'foo', just add another line 'PROFILE(foo,LEVEL)' and wrap
// the code to be profiled within a 'START (foo)' / 'STOP (foo)' block.

/*------------------------------------------------------------------------*/

// Profile counters for functions which are not compiled in should be
// removed. This is achieved by adding a wrapper macro for them here.

/*------------------------------------------------------------------------*/

#define PROFILES \
PROFILE(analyze,3) \
PROFILE(backward,3) \
PROFILE(block,2) \
PROFILE(bump,4) \
PROFILE(checking,2) \
PROFILE(cdcl,1) \
PROFILE(collect,3) \
PROFILE(compact,3) \
PROFILE(connect,3) \
PROFILE(cover,2) \
PROFILE(decide,3) \
PROFILE(decompose,3) \
PROFILE(elim,2) \
PROFILE(extend,3) \
PROFILE(lucky,2) \
PROFILE(minimize,4) \
PROFILE(parse,3) \
PROFILE(probe,2) \
PROFILE(deduplicate,3) \
PROFILE(propagate,4) \
PROFILE(reduce,3) \
PROFILE(restart,3) \
PROFILE(restore,2) \
PROFILE(search,1) \
PROFILE(solve,0) \
PROFILE(stable,2) \
PROFILE(preprocess,2) \
PROFILE(simplify,1) \
PROFILE(subsume,2) \
PROFILE(ternary,2) \
PROFILE(transred,3) \
PROFILE(unstable,2) \
PROFILE(vivify,2) \
PROFILE(walk,2) \

/*------------------------------------------------------------------------*/

// See 'START' and 'STOP' in 'macros.hpp' too.

struct Profile {

  bool active;
  double value;      // accumulated time
  double started;    // started time if active
  const char * name; // name of the profiled function (or 'phase')
  const int level;   // allows to cheaply test if profiling is enabled

  Profile (const char * n, int l) :
    active (false), value (0), name (n), level (l) { }
};

struct Profiles {
  Internal * internal;
#define PROFILE(NAME, LEVEL) \
  Profile NAME;
  PROFILES
#undef PROFILE
  Profiles (Internal *);
};

}

/*------------------------------------------------------------------------*/

// Macros for Profiling support.

#define START(P) \
do { \
  if (internal->profiles.P.level > internal->opts.profile) break; \
  internal->start_profiling (internal->profiles.P, internal->time ()); \
} while (0)

#define STOP(P) \
do { \
  if (internal->profiles.P.level > internal->opts.profile) break; \
  internal->stop_profiling (internal->profiles.P, internal->time ()); \
} while (0)

/*------------------------------------------------------------------------*/

#define START_SIMPLIFIER(S,M) \
do { \
  const double N = time (); \
  const int L = internal->opts.profile; \
  if (!preprocessing) { \
    if (stable && internal->profiles.stable.level <= L) \
      internal->stop_profiling (internal->profiles.stable, N); \
    if (!stable && internal->profiles.unstable.level <= L) \
      internal->stop_profiling (internal->profiles.unstable, N); \
    if (internal->profiles.search.level <= L) \
      internal->stop_profiling (internal->profiles.search, N); \
    reset_mode (SEARCH); \
  } \
  if (internal->profiles.simplify.level <= L) \
    internal->start_profiling (internal->profiles.simplify, N); \
  if (internal->profiles.S.level <= L) \
    internal->start_profiling (internal->profiles.S, N); \
  set_mode (SIMPLIFY); \
  set_mode (M); \
} while (0)

/*------------------------------------------------------------------------*/

#define STOP_SIMPLIFIER(S,M) \
do { \
  const double N = time (); \
  const int L = internal->opts.profile; \
  if (internal->profiles.S.level <= L) \
    internal->stop_profiling (internal->profiles.S, N); \
  if (internal->profiles.simplify.level <= L) \
    internal->stop_profiling (internal->profiles.simplify, N); \
  reset_mode (M); \
  reset_mode (SIMPLIFY); \
  if (!preprocessing) { \
    if (internal->profiles.search.level <= L) \
      internal->start_profiling (internal->profiles.search, N); \
    if (stable && internal->profiles.stable.level <= L) \
      internal->start_profiling (internal->profiles.stable, N); \
    if (!stable && internal->profiles.unstable.level <= L) \
      internal->start_profiling (internal->profiles.unstable, N); \
    set_mode (SEARCH); \
  } \
} while (0)

/*------------------------------------------------------------------------*/
// Used in 'walk' before calling 'walk_round' within the CDCL loop.

#define START_INNER_WALK() \
do { \
  require_mode (SEARCH); \
  assert (!preprocessing); \
  const double N = time (); \
  const int L = internal->opts.profile; \
  if (stable && internal->profiles.stable.level <= L) \
    internal->stop_profiling (internal->profiles.stable, N); \
  if (!stable && internal->profiles.unstable.level <= L) \
    internal->stop_profiling (internal->profiles.unstable, N); \
  if (internal->profiles.walk.level <= L) \
    internal->start_profiling (internal->profiles.walk, N); \
  set_mode (WALK); \
} while (0)

/*------------------------------------------------------------------------*/
// Used in 'walk' after calling 'walk_round' within the CDCL loop.

#define STOP_INNER_WALK() \
do { \
  require_mode (SEARCH); \
  assert (!preprocessing); \
  reset_mode (WALK); \
  const double N = time (); \
  const int L = internal->opts.profile; \
  if (internal->profiles.walk.level <= L) \
    internal->stop_profiling (internal->profiles.walk, N); \
  if (stable && internal->profiles.stable.level <= L) \
    internal->start_profiling (internal->profiles.stable, N); \
  if (!stable && internal->profiles.unstable.level <= L) \
    internal->start_profiling (internal->profiles.unstable, N); \
} while (0)

/*------------------------------------------------------------------------*/
// Used in 'local_search' before calling 'walk_round'.

#define START_OUTER_WALK() \
do { \
  require_mode (SEARCH); \
  assert (!preprocessing); \
  START (walk); \
  set_mode (WALK); \
} while (0)

/*------------------------------------------------------------------------*/
// Used in 'local_search' after calling 'walk_round'.

#define STOP_OUTER_WALK() \
do { \
  require_mode (SEARCH); \
  assert (!preprocessing); \
  reset_mode (WALK); \
  STOP (walk); \
} while (0)

/*------------------------------------------------------------------------*/
#else // ifndef QUIET
/*------------------------------------------------------------------------*/

#define START(...) do { } while (0)
#define STOP(...) do { } while (0)

#define START_SIMPLIFIER(...) do { } while (0)
#define STOP_SIMPLIFIER(...) do { } while (0)

#define START_OUTER_WALK(...) do { } while (0)
#define STOP_OUTER_WALK(...) do { } while (0)

#define START_INNER_WALK(...) do { } while (0)
#define STOP_INNER_WALK(...) do { } while (0)

/*------------------------------------------------------------------------*/
#endif // ifndef QUIET
/*------------------------------------------------------------------------*/

#endif // ifndef _profiles_h_INCLUDED
