/*

  Copyright (c) 2017 Martin Sustrik

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"),
  to deal in the Software without restriction, including without limitation
  the rights to use, copy, modify, merge, publish, distribute, sublicense,
  and/or sell copies of the Software, and to permit persons to whom
  the Software is furnished to do so, subject to the following conditions:
  The above copyright notice and this permission notice shall be included
  in all copies or substantial portions of the Software.
  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
  IN THE SOFTWARE.

*/

#ifndef DILL_POLLER_INCLUDED
#define DILL_POLLER_INCLUDED

#include "apoll.h"
#include "list.h"

/* This is a super-optimized I/O poller. It keeps the state for each fd in
   the user-space. It flushes the diff in state to kernel-space only once in
   a while when actuall polling is performed. For example, if user unregisters
   an fd, then registers the same fd again, no call to underlying polling
   mechanism is done. */

struct dill_pollerfd {
    /* Current events. TODO: Maybe we can rely on hints instead of this? */
    int curr;
    /* Events as set in apoll. */
    int old;
    /* Hint for the coroutine waiting for in. */
    void *inhint;
    /* Hint for the coroutine waiting for in. */
    void *outhint;
    /* Member of diff list. */
    struct dill_list item;
};
 
struct dill_poller {
    struct dill_apoll apoll;
    struct dill_pollerfd *fds;
    int nfds;
    /* List of fds that were changed since the last call to dill_apoll_wait. */
    struct dill_list diff;
};

int dill_poller_init(struct dill_poller *self);
void dill_poller_term(struct dill_poller *self);

int dill_poller_in(struct dill_poller *self, int fd, void *hint);
int dill_poller_out(struct dill_poller *self, int fd, void *hint);

int dill_poller_clean(struct dill_poller *self, int fd);

int dill_poller_event(struct dill_poller *self, void **hint, int timeout);

#endif

