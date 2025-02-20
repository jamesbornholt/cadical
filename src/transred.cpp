#include "internal.hpp"

namespace CaDiCaL {

// Implement transitive reduction in the binary implication graph.  This is
// important for hyper binary resolution, which has the risk to produce too
// many hyper binary resolvents otherwise.  This algorithm only works on
// binary clauses and is usually pretty fast, so we even do not limit it.
// It will also find some failed literals (in the binary implication graph).

void Internal::transred () {

  if (unsat || terminating ()) return;
  if (!stats.current.redundant && !stats.current.irredundant) return;

  assert (opts.transred);
  assert (opts.simplify);
  assert (!level);

  START_SIMPLIFIER (transred, TRANSRED);
  stats.transreds++;

  // Transitive reduction can not be run to completion for larger formulas
  // with many binary clauses.  We bound it in the same way as 'probe_core'.
  //
  long limit = stats.propagations.search;
  limit -= last.transred.propagations;
  limit *= 1e-3 * opts.transredreleff;
  if (limit < opts.transredmineff) limit = opts.transredmineff;
  if (limit > opts.transredmaxeff) limit = opts.transredmaxeff;

  PHASE ("transred", stats.transreds,
    "transitive reduction limit of %ld propagations", limit);

  const auto end = clauses.end ();
  auto i = clauses.begin ();

  // Find first clause not checked for being transitive yet.
  //
  for (; i != end; i++) {
    Clause * c = *i;
    if (c->garbage) continue;
    if (c->size != 2) continue;
    if (c->redundant && c->hyper) continue;
    if (!c->transred) break;
  }

  // If all candidate clauses have been checked reschedule all.
  //
  if (i == end) {

    PHASE ("transred", stats.transreds,
      "rescheduling all clauses since no clauses to check left");
    for (i = clauses.begin (); i != end; i++) {
      Clause * c = *i;
      if (c->transred) c->transred = false;
    }
    i = clauses.begin ();
  }

  // Move watches of binary clauses to the front. Thus we can stop iterating
  // watches as soon a long clause is found during watch traversal.
  //
  sort_watches ();

  // This working stack plays the same role as the 'trail' during standard
  // propagation.
  //
  vector<int> work;

  long propagations = 0, units = 0, removed = 0;

  while (!unsat && i != end && !terminating () && propagations < limit) {
    Clause * c = *i++;

    // A clause is a candidate for being transitive if it is binary, and not
    // the result of hyper binary resolution.  The reason for excluding
    // those, is that they come in large numbers, most of them are reduced
    // away anyhow and further are non-transitive at the point they are
    // added (see the code in 'hyper_binary_resolve' in 'prope.cpp' and
    // also check out our CPAIOR paper on tree-based look ahead).
    //
    if (c->garbage) continue;
    if (c->size != 2) continue;
    if (c->redundant && c->hyper) continue;
    if (c->transred) continue;                  // checked before?
    c->transred = true;                         // marked as checked

    LOG (c, "checking transitive reduction of");

    // Find a different path from 'src' to 'dst' in the binary implication
    // graph, not using 'c'.  Since this is the same as checking whether
    // there is a path from '-dst' to '-src', we can do the reverse search
    // if the number of watches of '-dst' is larger than those of 'src'.
    //
    int src = -c->literals[0];
    int dst = c->literals[1];
    if (val (src) || val (dst)) continue;
    if (watches (-src).size () < watches (dst).size ()) {
      int tmp = dst;
      dst = -src; src = -tmp;
    }

    LOG ("searching path from %d to %d", src, dst);

    // If the candidate clause is irredundant then we can not use redundant
    // binary clauses in the implication graph.  See our inprocessing rules
    // paper, why this restriction is required.
    //
    const bool irredundant = !c->redundant;

    assert (work.empty ());
    mark (src);
    work.push_back (src);
    LOG ("transred assign %d", src);

    bool transitive = false;            // found path from 'src' to 'dst'?
    bool failed = false;                // 'src' failed literal?

    size_t j = 0;                       // 'propagated' in BFS

    while (!transitive && !failed && j < work.size ()) {
      const int lit = work[j++];
      assert (marked (lit) > 0);
      LOG ("transred propagating %d", lit);
      propagations++;
      const Watches & ws = watches (-lit);
      const const_watch_iterator eow = ws.end ();
      const_watch_iterator k;
      for (k = ws.begin (); !transitive && !failed && k != eow; k++) {
        const Watch & w = *k;
        if (!w.binary ()) break;        // since we sorted watches above
        Clause * d = w.clause;
        if (d == c) continue;
        if (irredundant && d->redundant) continue;
        if (d->garbage) continue;
        const int other = w.blit;
        if (other == dst) transitive = true;    // 'dst' reached
        else {
          const int tmp = marked (other);
          if (tmp > 0) continue;
          else if (tmp < 0) {
            LOG ("found both %d and %d reachable", -other, other);
            failed = true;
          } else {
            mark (other);
            work.push_back (other);
            LOG ("transred assign %d", other);
          }
        }
      }
    }

    // Unassign all assigned literals (same as '[bp]acktrack').
    //
    while (!work.empty ()) {
      const int lit = work.back ();
      work.pop_back ();
      unmark (lit);
    }

    if (transitive) {
      removed++;
      stats.transitive++;
      LOG (c, "transitive redundant");
      mark_garbage (c);
    } else if (failed) {
      units++;
      LOG ("found failed literal %d during transitive reduction", src);
      stats.failed++;
      stats.transredunits++;
      assign_unit (-src);
      if (!propagate ()) {
        VERBOSE (1, "propagating new unit results in conflict");
        learn_empty_clause ();
      }
    }
  }

  last.transred.propagations = stats.propagations.search;
  stats.propagations.transred += propagations;
  erase_vector (work);

  PHASE ("transred", stats.transreds,
    "removed %ld transitive clauses, found %d units",
    removed, units);

  STOP_SIMPLIFIER (transred, TRANSRED);
  report ('t', !opts.reportall && !(removed + units));
}

}
