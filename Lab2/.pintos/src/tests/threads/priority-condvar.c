/* Tests that condvar_signal() wakes up the highest-priority thread
   waiting in condvar_wait(). */

#include <stdio.h>
#include "tests/threads/tests.h"
#include "threads/init.h"
#include "threads/malloc.h"
#include "threads/condvar.h"
#include "threads/lock.h"
#include "threads/thread.h"
#include "devices/timer.h"

static struct lock lock;
static struct condvar condvar;

static void
priority_condvar_thread(void *aux UNUSED)
{
  msg("Thread %s starting.", thread_name());
  lock_acquire(&lock);
  condvar_wait(&condvar, &lock);
  msg("Thread %s woke up.", thread_name());
  lock_release(&lock);
}

void test_priority_condvar(void)
{
  lock_init(&lock);
  condvar_init(&condvar);

  thread_set_priority(PRI_MIN);
  for (int i = 0; i < 10; i++)
  {
    int priority = PRI_DEFAULT - (i + 7) % 10 - 1;
    char name[16];
    snprintf(name, sizeof name, "priority %d", priority);
    thread_create(name, priority, priority_condvar_thread, NULL);
  }

  for (int i = 0; i < 10; i++)
  {
    lock_acquire(&lock);
    msg("Signaling...");
    condvar_signal(&condvar, &lock);
    lock_release(&lock);
  }
}

