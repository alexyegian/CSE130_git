/* Tests that the highest-priority thread waiting on a semaphore
   is the first to wake up. */

#include <stdio.h>
#include "tests/threads/tests.h"
#include "threads/init.h"
#include "threads/malloc.h"
#include "threads/semaphore.h"
#include "threads/thread.h"
#include "devices/timer.h"
#include "threads/interrupt.h"

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
//    printf("TEST MAKE THREAD\n");
    int priority = PRI_DEFAULT - (i + 3) % 10 - 1;
    char name[16];
    snprintf(name, sizeof name, "priority %d", priority);
    thread_create(name, priority, priority_sema_thread, NULL);
  }
//  struct list_elem* end = list_end(&(sema.waiters));
//  for(struct list_elem* beg = list_begin(&(sema.waiters)); beg != end; beg = list_next(beg)){
//    struct thread * t = list_entry(beg, struct thread, sharedelem);
//    printf("LIST: PRI: %d\n", t->priority);}
//  printf("EX UNBLOCK\n");
//  enum intr_level old_level;
//  struct thread * x = list_entry(list_pop_front(&(sema.waiters)), struct thread, sharedelem);

//  ASSERT (&(sema) != NULL);
//  old_level = intr_disable ();
//  printf("UB\n");
//  sema.value += 1;
//  thread_unblock(x);
//  printf("DUB\n");
//  intr_set_level (old_level);
//  printf("EX DONE\n");
  for (int i = 0; i < 10; i++)
  {
//    printf("I: %d SEMA VAL: %d\n", i, sema.value);
    semaphore_up(&sema);
    msg("Back in main thread.");
//    printf("AFTER I: %d SEMA VAL: %d\n", i, sema.value);
  }
}
