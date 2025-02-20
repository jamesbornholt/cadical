// vim: set tw=300: set VIM text width to 300 characters for this file.

#include "internal.hpp"

namespace CaDiCaL {

/*------------------------------------------------------------------------*/

Stats::Stats () {
  memset (this, 0, sizeof *this);
  time.real = absolute_real_time ();
  time.process = absolute_process_time ();
  walk.minimum = LONG_MAX;
}

/*------------------------------------------------------------------------*/

#define PRT(FMT,...) \
do { \
  if (FMT[0] == ' ' && !all) break; \
  MSG (FMT, __VA_ARGS__); \
} while (0)

/*------------------------------------------------------------------------*/

void Stats::print (Internal * internal) {

#ifdef QUIET
  (void) internal;
#else

  Stats & stats = internal->stats;

  int all = internal->opts.verbose > 0;
#ifdef LOGGING
  if (internal->opts.log) all = true;
#endif // ifdef LOGGING

  if (internal->opts.profile) internal->print_profile ();

  double t = internal->solve_time ();

  long propagations = 0;
  propagations += stats.propagations.cover;
  propagations += stats.propagations.probe;
  propagations += stats.propagations.search;
  propagations += stats.propagations.transred;
  propagations += stats.propagations.vivify;
  propagations += stats.propagations.walk;

  long vivified = stats.vivifysubs + stats.vivifystrs;

  size_t extendbytes = internal->external->extension.size ();
  extendbytes *= sizeof (int);

  SECTION ("statistics");

  if (all || stats.blocked) {
  PRT ("blocked:         %15ld   %10.2f %%  of irredundant clauses", stats.blocked, percent (stats.blocked, stats.added.irredundant));
  PRT ("  blockings:     %15ld   %10.2f    internal", stats.blockings, relative (stats.conflicts, stats.blockings));
  PRT ("  candidates:    %15ld   %10.2f    per blocking ", stats.blockcands, relative (stats.blockcands, stats.blockings));
  PRT ("  blockres:      %15ld   %10.2f    per candidate", stats.blockres, relative (stats.blockres, stats.blockcands));
  PRT ("  pure:          %15ld   %10.2f %%  of all variables", stats.all.pure, percent (stats.all.pure, stats.vars));
  PRT ("  pureclauses:   %15ld   %10.2f    per pure literal", stats.blockpured, relative (stats.blockpured, stats.all.pure));
  }
  if (all || stats.backtracks)
  PRT ("backtracked:     %15ld   %10.2f %%  of conflicts", stats.backtracks, percent (stats.backtracks, stats.conflicts));
  if (all || stats.chrono)
  PRT ("chronological:   %15ld   %10.2f %%  of backtracks", stats.chrono, percent (stats.chrono, stats.backtracks));
  if (all)
  PRT ("compacts:        %15ld   %10.2f    interval", stats.compacts, relative (stats.conflicts, stats.compacts));
  if (all || stats.conflicts) {
  PRT ("conflicts:       %15ld   %10.2f    per second", stats.conflicts, relative (stats.conflicts, t));
  }
  if (all || stats.cover.total) {
  PRT ("covered:         %15ld   %10.2f %%  of irredundant clauses", stats.cover.total, percent (stats.cover.total, stats.added.irredundant));
  PRT ("  coverings:     %15ld   %10.2f    interval", stats.cover.count, relative (stats.conflicts, stats.cover.count));
  PRT ("  asymmetric:    %15ld   %10.2f %%  of covered clauses", stats.cover.asymmetric, percent (stats.cover.asymmetric, stats.cover.total));
  PRT ("  blocked:       %15ld   %10.2f %%  of covered clauses", stats.cover.blocked, percent (stats.cover.blocked, stats.cover.total));
  }
  if (all || stats.decisions) {
  PRT ("decisions:       %15ld   %10.2f    per second", stats.decisions, relative (stats.decisions, t));
  PRT ("  searched:      %15ld   %10.2f    per decision", stats.searched, relative (stats.searched, stats.decisions));
  }
  if (all || stats.all.eliminated) {
  PRT ("eliminated:      %15ld   %10.2f %%  of all variables", stats.all.eliminated, percent (stats.all.eliminated, stats.vars));
  PRT ("  elimphases:    %15ld   %10.2f    interval", stats.elimphases, relative (stats.conflicts, stats.elimphases));
  PRT ("  elimrounds:    %15ld   %10.2f    per phase", stats.elimrounds, relative (stats.elimrounds, stats.elimphases));
  PRT ("  elimtried:     %15ld   %10.2f %%  eliminated", stats.elimtried, percent (stats.all.eliminated, stats.elimtried));
  PRT ("  elimgates:     %15ld   %10.2f %%  gates per tried", stats.elimgates, percent (stats.elimgates, stats.elimtried));
  PRT ("  elimequivs:    %15ld   %10.2f %%  equivalence gates", stats.elimequivs, percent (stats.elimequivs, stats.elimgates));
  PRT ("  elimands:      %15ld   %10.2f %%  and gates", stats.elimands, percent (stats.elimands, stats.elimgates));
  PRT ("  elimites:      %15ld   %10.2f %%  if-then-else gates", stats.elimites, percent (stats.elimites, stats.elimgates));
  PRT ("  elimxors:      %15ld   %10.2f %%  xor gates", stats.elimxors, percent (stats.elimxors, stats.elimgates));
  PRT ("  elimsubst:     %15ld   %10.2f %%  substituted", stats.elimsubst, percent (stats.elimsubst, stats.all.eliminated));
  PRT ("  elimres:       %15ld   %10.2f    per eliminated", stats.elimres, relative (stats.elimres, stats.all.eliminated));
  PRT ("  elimrestried:  %15ld   %10.2f %%  per resolution", stats.elimrestried, percent (stats.elimrestried, stats.elimres));
  }
  if (all || stats.all.fixed) {
  PRT ("fixed:           %15ld   %10.2f %%  of all variables", stats.all.fixed, percent (stats.all.fixed, stats.vars));
  PRT ("  failed:        %15ld   %10.2f %%  of all variables", stats.failed, percent (stats.failed, stats.vars));
  PRT ("  probefailed:   %15ld   %10.2f %%  per failed", stats.probefailed, percent (stats.probefailed, stats.failed));
  PRT ("  transredunits: %15ld   %10.2f %%  per failed", stats.transredunits, percent (stats.transredunits, stats.failed));
  PRT ("  probingphases: %15ld   %10.2f    interval", stats.probingphases, relative (stats.conflicts, stats.probingphases));
  PRT ("  probingrounds: %15ld   %10.2f    per phase", stats.probingrounds, relative (stats.probingrounds, stats.probingphases));
  PRT ("  probed:        %15ld   %10.2f    per failed", stats.probed, relative (stats.probed, stats.failed));
  PRT ("  hbrs:          %15ld   %10.2f    per probed", stats.hbrs, relative (stats.hbrs, stats.probed));
  PRT ("  hbrsizes:      %15ld   %10.2f    per hbr", stats.hbrsizes, relative (stats.hbrsizes, stats.hbrs));
  PRT ("  hbreds:        %15ld   %10.2f %%  per hbr", stats.hbreds, percent (stats.hbreds, stats.hbrs));
  PRT ("  hbrsubs:       %15ld   %10.2f %%  per hbr", stats.hbrsubs, percent (stats.hbrsubs, stats.hbrs));
  }
  PRT ("  units:         %15ld   %10.2f    interval", stats.units, relative (stats.conflicts, stats.units));
  PRT ("  binaries:      %15ld   %10.2f    interval", stats.binaries, relative (stats.conflicts, stats.binaries));
  if (all || stats.flush.learned) {
  PRT ("flushed:         %15ld   %10.2f %%  per conflict", stats.flush.learned, percent (stats.flush.learned, stats.conflicts));
  PRT ("  hyper:         %15ld   %10.2f %%  per conflict", stats.flush.hyper, relative (stats.flush.hyper, stats.conflicts));
  PRT ("  flushings:     %15ld   %10.2f    interval", stats.flush.count, relative (stats.conflicts, stats.flush.count));
  }
  if (all || stats.conflicts) {
  PRT ("learned:         %15ld   %10.2f %%  per conflict", stats.learned.clauses, percent (stats.learned.clauses, stats.conflicts));
  PRT ("  bumped:        %15ld   %10.2f    per learned", stats.bumped, relative (stats.bumped, stats.learned.clauses));
  }
  if (all || stats.lucky.succeeded) {
  PRT ("lucky:           %15ld   %10.2f %%  of tried", stats.lucky.succeeded, percent (stats.lucky.succeeded, stats.lucky.tried));
  PRT ("  constantzero   %15ld   %10.2f %%  of tried", stats.lucky.constant.zero, percent (stats.lucky.constant.zero, stats.lucky.tried));
  PRT ("  constantone    %15ld   %10.2f %%  of tried", stats.lucky.constant.one, percent (stats.lucky.constant.one, stats.lucky.tried));
  PRT ("  backwardone    %15ld   %10.2f %%  of tried", stats.lucky.backward.one, percent (stats.lucky.backward.one, stats.lucky.tried));
  PRT ("  backwardzero   %15ld   %10.2f %%  of tried", stats.lucky.backward.zero, percent (stats.lucky.backward.zero, stats.lucky.tried));
  PRT ("  forwardone     %15ld   %10.2f %%  of tried", stats.lucky.forward.one, percent (stats.lucky.forward.one, stats.lucky.tried));
  PRT ("  forwardzero    %15ld   %10.2f %%  of tried", stats.lucky.forward.zero, percent (stats.lucky.forward.zero, stats.lucky.tried));
  PRT ("  positivehorn   %15ld   %10.2f %%  of tried", stats.lucky.horn.positive, percent (stats.lucky.horn.positive, stats.lucky.tried));
  PRT ("  negativehorn   %15ld   %10.2f %%  of tried", stats.lucky.horn.negative, percent (stats.lucky.horn.negative, stats.lucky.tried));
  }
  PRT ("  extendbytes:   %15ld   %10.2f    bytes and MB", extendbytes, extendbytes/(double)(1l<<20));
  if (all || stats.learned.clauses)
  PRT ("minimized:       %15ld   %10.2f %%  learned literals", stats.minimized, percent (stats.minimized, stats.learned.literals));
  PRT ("propagations:    %15ld   %10.2f M  per second", propagations, relative (propagations/1e6, t));
  PRT ("  coverprops:    %15ld   %10.2f %%  of propagations", stats.propagations.cover, percent (stats.propagations.cover, propagations));
  PRT ("  probeprops:    %15ld   %10.2f %%  of propagations", stats.propagations.probe, percent (stats.propagations.probe, propagations));
  PRT ("  searchprops:   %15ld   %10.2f %%  of propagations", stats.propagations.search, percent (stats.propagations.search, propagations));
  PRT ("  transredprops: %15ld   %10.2f %%  of propagations", stats.propagations.transred, percent (stats.propagations.transred, propagations));
  PRT ("  vivifyprops:   %15ld   %10.2f %%  of propagations", stats.propagations.vivify, percent (stats.propagations.vivify, propagations));
  PRT ("  walkprops:     %15ld   %10.2f %%  of propagations", stats.propagations.walk, percent (stats.propagations.walk, propagations));
  if (all || stats.reactivated) {
  PRT ("reactivated:     %15ld   %10.2f %%  of all variables", stats.reactivated, percent (stats.reactivated, stats.vars));
  }
  if (all || stats.reduced) {
  PRT ("reduced:         %15ld   %10.2f %%  per conflict", stats.reduced, percent (stats.reduced, stats.conflicts));
  PRT ("  reductions:    %15ld   %10.2f    interval", stats.reductions, relative (stats.conflicts, stats.reductions));
  PRT ("  collections:   %15ld   %10.2f    interval", stats.collections, relative (stats.conflicts, stats.collections));
  }
  if (all || stats.rephased.total) {
  PRT ("rephased:        %15ld   %10.2f    interval", stats.rephased.total, relative (stats.conflicts, stats.rephased.total));
  PRT ("  rephasedbest:  %15ld   %10.2f %%  rephased best", stats.rephased.best, percent (stats.rephased.best, stats.rephased.total));
  PRT ("  rephasedflip:  %15ld   %10.2f %%  rephased flipping", stats.rephased.flipped, percent (stats.rephased.flipped, stats.rephased.total));
  PRT ("  rephasedinv:   %15ld   %10.2f %%  rephased inverted", stats.rephased.inverted, percent (stats.rephased.inverted, stats.rephased.total));
  PRT ("  rephasedorig:  %15ld   %10.2f %%  rephased original", stats.rephased.original, percent (stats.rephased.original, stats.rephased.total));
  PRT ("  rephasedrand:  %15ld   %10.2f %%  rephased random", stats.rephased.random, percent (stats.rephased.random, stats.rephased.total));
  PRT ("  rephasedwalk:  %15ld   %10.2f %%  rephased walk", stats.rephased.walk, percent (stats.rephased.walk, stats.rephased.total));
  }
  if (all)
  PRT ("rescored:        %15ld   %10.2f    interval", stats.rescored, relative (stats.conflicts, stats.rescored));
  if (all || stats.restarts) {
  PRT ("restarts:        %15ld   %10.2f    interval", stats.restarts, relative (stats.conflicts, stats.restarts));
  PRT ("  reused:        %15ld   %10.2f %%  per restart", stats.reused, percent (stats.reused, stats.restarts));
  PRT ("  reusedlevels:  %15ld   %10.2f %%  per restart levels", stats.reusedlevels, percent (stats.reusedlevels, stats.restartlevels));
  }
  if (all || stats.restored) {
  PRT ("restored:        %15ld   %10.2f %%  per weakened", stats.restored, percent (stats.restored, stats.weakened));
  PRT ("  restorations:  %15ld   %10.2f %%  per extension", stats.restorations, percent (stats.restorations, stats.extensions));
  PRT ("  literals:      %15ld   %10.2f    per restored clause", stats.restoredlits, relative (stats.restoredlits, stats.restored));
  }
  if (all || stats.stabphases) {
  PRT ("stabilizing:     %15ld   %10.2f %%  of conflicts", stats.stabphases, percent (stats.stabconflicts, stats.conflicts));
  PRT ("  restartstab:   %15ld   %10.2f %%  of all restarts", stats.restartstable, percent (stats.restartstable, stats.restarts));
  PRT ("  reusedstab:    %15ld   %10.2f %%  per stable restarts", stats.reusedstable, percent (stats.reusedstable, stats.restartstable));
  }
  if (all || stats.all.substituted) {
  PRT ("substituted:     %15ld   %10.2f %%  of all variables", stats.all.substituted, percent (stats.all.substituted, stats.vars));
  PRT ("  decompositions:  %13ld   %10.2f    per phase", stats.decompositions, relative (stats.decompositions, stats.probingphases));
  }
  if (all || stats.subsumed) {
  PRT ("subsumed:        %15ld   %10.2f %%  of all clauses", stats.subsumed, percent (stats.subsumed, stats.added.total));
  PRT ("  subsumephases: %15ld   %10.2f    interval", stats.subsumephases, relative (stats.conflicts, stats.subsumephases));
  PRT ("  subsumerounds: %15ld   %10.2f    per phase", stats.subsumerounds, relative (stats.subsumerounds, stats.subsumephases));
  PRT ("  deduplicated:  %15ld   %10.2f %%  per subsumed", stats.deduplicated, percent (stats.deduplicated, stats.subsumed));
  PRT ("  transreductions:%14ld   %10.2f    interval", stats.transreds, relative (stats.conflicts, stats.transreds));
  PRT ("  transitive:    %15ld   %10.2f %%  per subsumed", stats.transitive, percent (stats.transitive, stats.subsumed));
  PRT ("  subirr:        %15ld   %10.2f %%  of subsumed", stats.subirr, percent (stats.subirr, stats.subsumed));
  PRT ("  subred:        %15ld   %10.2f %%  of subsumed", stats.subred, percent (stats.subred, stats.subsumed));
  PRT ("  subtried:      %15ld   %10.2f    tried per subsumed", stats.subtried, relative (stats.subtried, stats.subsumed));
  PRT ("  subchecks:     %15ld   %10.2f    per tried", stats.subchecks, relative (stats.subchecks, stats.subtried));
  PRT ("  subchecks2:    %15ld   %10.2f %%  per subcheck", stats.subchecks2, percent (stats.subchecks2, stats.subchecks));
  PRT ("  elimotfsub:    %15ld   %10.2f %%  of subsumed", stats.elimotfsub, percent (stats.elimotfsub, stats.subsumed));
  PRT ("  elimbwsub:     %15ld   %10.2f %%  of subsumed", stats.elimbwsub, percent (stats.elimbwsub, stats.subsumed));
  PRT ("  eagersub:      %15ld   %10.2f %%  of subsumed", stats.eagersub, percent (stats.eagersub, stats.subsumed));
  PRT ("  eagertried:    %15ld   %10.2f    tried per eagersub", stats.eagertried, relative (stats.eagertried, stats.eagersub));
  }
  if (all || stats.strengthened) {
  PRT ("strengthened:    %15ld   %10.2f %%  of all clauses", stats.strengthened, percent (stats.strengthened, stats.added.total));
  PRT ("  elimotfstr:    %15ld   %10.2f %%  of strengthened", stats.elimotfstr, percent (stats.elimotfstr, stats.strengthened));
  PRT ("  elimbwstr:     %15ld   %10.2f %%  of strengthened", stats.elimbwstr, percent (stats.elimbwstr, stats.strengthened));
  }
  if (all || stats.htrs) {
  PRT ("ternary:         %15ld   %10.2f %%  of resolved", stats.htrs, percent (stats.htrs, stats.ternres));
  PRT ("  phases:        %15ld   %10.2f    interval", stats.ternary, relative (stats.conflicts, stats.ternary));
  PRT ("  htr3:          %15ld   %10.2f %%  ternary hyper ternres", stats.htrs3, percent (stats.htrs3, stats.htrs));
  PRT ("  htr2:          %15ld   %10.2f %%  binary hyper ternres", stats.htrs2, percent (stats.htrs2, stats.htrs));
  }
  if (all || vivified) {
  PRT ("vivified:        %15ld   %10.2f %%  of all clauses", vivified, percent (vivified, stats.added.total));
  PRT ("  vivifications: %15ld   %10.2f    interval", stats.vivifications, relative (stats.conflicts, stats.vivifications));
  PRT ("  vivifychecks:  %15ld   %10.2f %%  per conflict", stats.vivifychecks, percent (stats.vivifychecks, stats.conflicts));
  PRT ("  vivifysched:   %15ld   %10.2f %%  checks per scheduled", stats.vivifysched, percent (stats.vivifychecks, stats.vivifysched));
  PRT ("  vivifyunits:   %15ld   %10.2f %%  per vivify check", stats.vivifyunits, percent (stats.vivifyunits, stats.vivifychecks));
  PRT ("  vivifysubs:    %15ld   %10.2f %%  per subsumed", stats.vivifysubs, percent (stats.vivifysubs, stats.subsumed));
  PRT ("  vivifystrs:    %15ld   %10.2f %%  per strengthened", stats.vivifystrs, percent (stats.vivifystrs, stats.strengthened));
  PRT ("  vivifystrirr:  %15ld   %10.2f %%  per vivifystrs", stats.vivifystrirr, percent (stats.vivifystrirr, stats.vivifystrs));
  PRT ("  vivifystred1:  %15ld   %10.2f %%  per vivifystrs", stats.vivifystred1, percent (stats.vivifystred1, stats.vivifystrs));
  PRT ("  vivifystred2:  %15ld   %10.2f %%  per vivifystrs", stats.vivifystred2, percent (stats.vivifystred2, stats.vivifystrs));
  PRT ("  vivifystred3:  %15ld   %10.2f %%  per vivifystrs", stats.vivifystred3, percent (stats.vivifystred3, stats.vivifystrs));
  PRT ("  vivifydecs:    %15ld   %10.2f    per checks", stats.vivifydecs, relative (stats.vivifydecs, stats.vivifychecks));
  PRT ("  vivifyreused:  %15ld   %10.2f %%  per decision", stats.vivifyreused, percent (stats.vivifyreused, stats.vivifydecs));
  }
  if (all || stats.walk.count) {
  PRT ("walked:          %15ld   %10.2f    interval", stats.walk.count, relative (stats.conflicts, stats.walk.count));
#ifndef QUIET
  if (internal->profiles.walk.value > 0)
  PRT ("  flips:         %15ld   %10.2f M  per second", stats.walk.flips, relative (1e-6*stats.walk.flips, internal->profiles.walk.value));
  else
#endif
  PRT ("  flips:         %15ld   %10.2f    per walk", stats.walk.flips, relative (stats.walk.flips, stats.walk.count));
  if (stats.walk.minimum < LONG_MAX)
  PRT ("  minimum:       %15ld   %10.2f %%  clauses", stats.walk.minimum, percent (stats.walk.minimum, stats.added.irredundant));
  PRT ("  broken:        %15ld   %10.2f    per flip", stats.walk.broken, relative (stats.walk.broken, stats.walk.flips));
  }
  if (all || stats.weakened) {
  PRT ("weakened:        %15ld   %10.2f    average size", stats.weakened, relative (stats.weakenedlen, stats.weakened));
  PRT ("  extensions:    %15ld   %10.2f    interval", stats.extensions, relative (stats.conflicts, stats.extensions));
  PRT ("  flipped:       %15ld   %10.2f    per weakened", stats.extended, relative (stats.extended, stats.weakened));
  }

  MSG ("");
  MSG ("%sseconds are measured in %s time for solving%s",
    tout.magenta_code (), internal->opts.realtime ? "real" : "process", tout.normal_code ());

  SECTION ("resources");

  size_t m = maximum_resident_set_size ();
  PRT ("total process time since initialization: %12.2f    seconds", internal->process_time ());
  PRT ("total real time since initialization:    %12.2f    seconds", internal->real_time ());
  PRT ("maximum resident set size of process:    %12.2f    MB", m/(double)(1l<<20));

#endif // ifndef QUIET

}

/*------------------------------------------------------------------------*/

void Checker::print_stats () {

  if (!stats.added && !stats.deleted) return;

  SECTION ("checker statistics");

  MSG ("checks:          %15ld", stats.checks);
  MSG ("assumptions:     %15ld   %10.2f    per check", stats.assumptions, relative (stats.assumptions, stats.checks));
  MSG ("propagations:    %15ld   %10.2f    per check", stats.propagations, relative (stats.propagations, stats.checks));
  MSG ("original:        %15ld   %10.2f %%  of all clauses", stats.original, percent (stats.original, stats.added));
  MSG ("derived:         %15ld   %10.2f %%  of all clauses", stats.derived, percent (stats.derived, stats.added));
  MSG ("deleted:         %15ld   %10.2f %%  of all clauses", stats.deleted, percent (stats.deleted, stats.added));
  MSG ("insertions:      %15ld   %10.2f %%  of all clauses", stats.insertions, percent (stats.insertions, stats.added));
  MSG ("collections:     %15ld   %10.2f    deleted per collection", stats.collections, relative (stats.collections, stats.deleted));
  MSG ("collisions:      %15ld   %10.2f    per search", stats.collisions, relative (stats.collisions, stats.searches));
  MSG ("searches:        %15ld", stats.searches);
  MSG ("units:           %15ld", stats.units);
}

}
