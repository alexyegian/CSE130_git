/*
 * See lecture handouts for details.
 */

#include <stdio.h>
#include "tests/threads/tests.h"
#include "threads/init.h"
#include "threads/lock.h"
#include "threads/thread.h"

static struct lock lock;

static void b_thread_func(void *aux UNUSED)
{
  lock_acquire(&lock);
  lock_release(&lock);
  msg("Thread B finished.");
}

static void c_thread_func(void *aux UNUSED)
{
  msg("Thread C finished.");
}

void test_priority_donate_single(void)
{
  thread_set_priority(20);

  lock_init(&lock);
  lock_acquire(&lock);

  thread_create("Thread B", 40, b_thread_func, NULL);
  thread_create("Thread C", 30, c_thread_func, NULL);

  lock_release(&lock);
  msg("Thread A finished.");
}

