/* Tests that the highest-priority thread waiting on a semaphore
   is the first to wake up. */

#include <stdio.h>
#include "tests/threads/tests.h"
#include "threads/init.h"
#include "threads/malloc.h"
#include "threads/semaphore.h"
#include "threads/thread.h"
#include "devices/timer.h"

static struct semaphore sema;

static void
priority_sema_thread(void *aux UNUSED)
{
  semaphore_down(&sema);
  msg("Thread %s woke up.", thread_name());
}

void test_priority_semaphore(void)
{
  semaphore_init(&sema, 0);
  thread_set_priority(PRI_MIN);

  for (int i = 0; i < 10; i++)
  {
    int priority = PRI_DEFAULT - (i + 3) % 10 - 1;
    char name[16];
    snprintf(name, sizeof name, "priority %d", priority);
    thread_create(name, priority, priority_sema_thread, NULL);
  }

  for (int i = 0; i < 10; i++)
  {
    semaphore_up(&sema);
    msg("Back in main thread.");
  }
}
