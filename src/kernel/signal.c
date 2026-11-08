/*
 * KFS-4
 *
 * Role: Kernel signal-callback system (register a callback for a signal) and an interface to schedule signals.
 * Depends on: nothing for the registry itself; idt/pit for asynchronous scheduling
 * Unlocks: process signals (KFS-5)
 */

#include "kernel/signal.h"
