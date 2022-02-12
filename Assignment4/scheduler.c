/************************************************************************
 *
 * CSE130 Assignment 4
 *
 * Copyright (C) 2021-2022 David C. Harrison. All right reserved.
 *
 * You may not use, distribute, publish, or modify this code without 
 * the express written permission of the copyright holder.
 * 
 ************************************************************************/

/**
 * See scheduler.h for function details. All are callbacks; i.e. the simulator 
 * calls you when something interesting happens.
 */
#include <stdlib.h>
#include <stdio.h>

#include "simulator.h"
#include "scheduler.h"
#include "queue.h"

typedef struct LL{
	struct LL *next;
	struct LL *prev;
	thread_t *cur;}LL;


struct times* cur_time = NULL;
void* ready_queue;
void* io_queue;
void* time_ready;
thread_t * cur_t = NULL;
thread_t * io_t = NULL;
int ticks = 0;
void* done_queue;
void* time_io;
void* done_time;
struct times* io_time = NULL;
int beg_tid = 0;

int alg;
int quant;
/**
 * Initalise a scheduler implemeting the requested ALGORITHM. QUANTUM is only
 * meaningful for ROUND_ROBIN.
 */
//  FIRST_COME_FIRST_SERVED,
//  ROUND_ROBIN,
//  NON_PREEMPTIVE_PRIORITY,
//  PREEMPTIVE_PRIORITY,
//  NON_PREEMPTIVE_SHORTEST_JOB_FIRST,
//  PREEMPTIVE_SHORTEST_JOB_FIRST,
//  NON_PREEMPTIVE_SHORTEST_REMAINING_TIME_FIRST,
//  PREEMPTIVE_SHORTEST_REMAINING_TIME_FIRST

void scheduler(enum algorithm algorithm, unsigned int quantum) {
//    cur_red = malloc(sizeof(LL));
    ready_queue = queue_create(); 
    io_queue = queue_create();
    done_queue = queue_create();
    time_ready = queue_create();
    time_io = queue_create();
    done_time = queue_create();
    alg = algorithm;
    quant = quantum;
    printf("ALG: %d\n", alg);
}

/**
 * Thread T is ready to be scheduled for the first time.
 */
void sys_exec(thread_t *t) {
    
//    printf("EXEC\n");
//    struct LL* newt = malloc(sizeof(LL));
//    newt->cur = t;
//    newt->prev = cur_red;
//    cur_red->next = newt;
//    newt->cur = t;
//    if(cur_red->cur == NULL){
//	sim_dispatch(t);
//	cur_red = newt;
//    }
   // sim_dispatch(t);
  printf("THREAD: %d BEG: %d\n", t->tid, ticks);
  if(queue_size(ready_queue)==0 && cur_t == NULL){
    printf("BEGIN\n");
//    if(cur_time == NULL){
      struct times* ti = malloc(sizeof(struct times));
      ti->last_on = ticks;
      ti->running_time = 0;
      ti->waiting_time = 0;
      ti->beg_time = ticks;
      cur_time = ti;
      beg_tid = t->tid;
  //  }
    cur_t = t;
    sim_dispatch(t);
  }
  else{
      cur_time->last_on = ticks;
      struct times* ti = malloc(sizeof(struct times));
      ti->last_on = ticks;
      ti->running_time = 0;
      ti->waiting_time = 0;
      ti->beg_time = ticks;
      queue_enqueue(ready_queue, t);
      printf("TIME ENQUEUE: %d\n", ti->beg_time);
      queue_enqueue(time_ready, ti);
  }
}

/**
 * Programmable clock interrupt handler. Call sim_time() to find out
 * what tick this is. Called after all calls to sys_exec() for this
 * tick have been made.
 */
void tick() {
    ++ticks;
}

/**
 * Thread T has completed execution and should never again be scheduled.
 */
void sys_exit(thread_t *t) {
//    if(cur_red->next != NULL){
//      cur_red = cur_red->next;
//      sim_dispatch(cur_red->cur);}
//    else{
//	    printf("NO NEXT\n"); }
    cur_time->running_time = ticks-cur_time->beg_time;
    printf("THREAD: %d EXIT 2: %d BEG: %d TICK: %d\n", t->tid, cur_time->running_time, cur_time->beg_time, ticks);
    queue_enqueue(done_queue, t);
    queue_enqueue(done_time, cur_time);
    thread_t *f = queue_dequeue(ready_queue);
    cur_t = f;
    if(cur_t != NULL){
      cur_time = queue_dequeue(time_ready);
      printf("CHANGE TIME: %d\n", cur_time->beg_time);
      cur_time->waiting_time += (ticks-cur_time->last_on);
//      cur_time->last_on = ticks;
      printf("\nTHREAD: %d WAIT: %d ADD TIME: %d\n\n", cur_t->tid, cur_time->waiting_time, ticks-cur_time->last_on);
      cur_time->last_on = ticks;
      sim_dispatch(cur_t);
    }
    else{
      printf("EXIT TO NULL\n");}
}

/**
 * Thread T has requested a read operation and is now in an I/O wait queue.
 * When the read operation starts, io_starting(T) will be called, when the
 * read operation completes, io_complete(T) will be called.
 */
void sys_read(thread_t *t) {

    printf("READ\n");
    if(queue_size(io_queue)==0 && io_t == NULL){
      printf("\nNO IO READ\n\n");
      io_t = t;
      io_time = cur_time;
      io_time->last_on = ticks;
    }
    else{
      cur_time->last_on = ticks;
      printf("\nLAST ON: %d IS: %d\n\n", cur_t->tid, ticks);
      queue_enqueue(io_queue, t);
      queue_enqueue(time_io, cur_time);
    }
    cur_t = queue_dequeue(ready_queue);
    if(cur_t != NULL){
        cur_time = queue_dequeue(time_ready);
        printf("CHANGE TIME READ: %d\n", cur_time->beg_time);

        cur_time->waiting_time += ticks-cur_time->last_on;
	cur_time->last_on = ticks;
        printf("\nTHREAD: %d WAIT: %d ADD TIME: %d\n\n", cur_t->tid, cur_time->waiting_time, ticks-cur_time->last_on);
 	sim_dispatch(cur_t); 
    }
    else{
      printf("CUR T READ NULL\n");}

}

/**
 * Thread T has requested a write operation and is now in an I/O wait queue.
 * When the write operation starts, io_starting(T) will be called, when the
 * write operation completes, io_complete(T) will be called.
 */
void sys_write(thread_t *t) {

    
    printf("WRITE\n");
    if(queue_size(io_queue)==0 && io_t == NULL){
      io_t = t;
      io_time = cur_time;
      io_time->last_on = ticks;
      printf("\nNO IO WRITE\n\n");
    }
    else{
      queue_enqueue(io_queue, t);
      cur_time->last_on = ticks;
      printf("\nLAST ON: %d IS %d\n\n", cur_t->tid, ticks);
      queue_enqueue(time_io, cur_time);

    }
    cur_t = queue_dequeue(ready_queue);
    if(cur_t != NULL){
      cur_time = queue_dequeue(time_ready);
            printf("CHANGE TIME WRITE: %d\n", cur_time->beg_time);

        cur_time->waiting_time += ticks-cur_time->last_on;

	cur_time->last_on = ticks;
      printf("\nTHREAD: %d WAIT: %d ADD TIME: %d\n\n", cur_t->tid, cur_time->waiting_time, ticks-cur_time->last_on);

      sim_dispatch(cur_t);
      printf("DISPATCH 2\n");
    }
    else{
	 printf("CURT WRITE NULL\n");}

}

/**
 * An I/O operation requested by T has completed; T is now ready to be 
 * scheduled again.
 */
void io_complete(thread_t *t) {
  printf("IO DONE\n");
  io_time->last_on = ticks;
  queue_enqueue(ready_queue, t);

  printf("TIME ENQUEUE: %d\n", io_time->beg_time);
  queue_enqueue(time_ready, io_time);
  io_t = queue_dequeue(io_queue);
  io_time = queue_dequeue(time_io);
  if(cur_t == NULL){
    
    printf("IO CURT NULL\n");
    cur_t = queue_dequeue(ready_queue);
    if(cur_t != NULL){
	cur_time = queue_dequeue(time_ready);
	        printf("CHANGE TIME IO DONE: %d\n", cur_time->beg_time);
        cur_time->waiting_time += ticks-cur_time->last_on;

        cur_time->last_on = ticks;

        printf("\nTHREAD: %d WAIT: %d ADD TIME: %d\n\n", cur_t->tid, cur_time->waiting_time, ticks-cur_time->last_on);

      printf("DISPATCH 3\n");
      sim_dispatch(cur_t);		  
    }
  }
  else{
    printf("IO CURT NOT NULL: %d\n", cur_t->tid);}
//  f = queue_dequeue(ready_queue);
//  if(f!=NULL){
//    sim_dispatch(f);
//  }
}

/**
 * An I/O operation requested by T is starting; T will not be ready for
 * scheduling until the operation completes.
 */
void io_starting(thread_t *t) {
//    if(cur_red->next != NULL){
//      cur_red = cur_red->next;
//      sim_dispatch(cur_red->cur);}

//    printf("IO START\n");
//    if(cur_red->next != NULL){
//      cur_red = cur_red->next;
//      sim_dispatch(cur_red->cur);}
//    else{
//            printf("NO NEXT 2\n");}
//    if(cur_t == NULL)
//    {
    io_time->waiting_time += (ticks-1)-io_time->last_on;
    io_time->last_on = ticks;
    //}
//    else{
    printf("\nIO START: %d WAITING: %d\n\n", io_t->tid, (ticks-io_time->last_on));
    io_time->waiting_time += (ticks)-io_time->last_on;
    

    io_time->last_on = ticks;
//  }
//    cur_t = queue_dequeue(ready_queue);
//    if(cur_t != NULL){
//      sim_dispatch(cur_t);
//      printf("DISPATCH 2\n");
//    }
//    if(queue_size(io_queue)==0 && io_t == NULL){
//      io_t = t;
//    }
//    else{
//      queue_enqueue(io_queue, t);
//    }
}


/**
 * Return dynamically allocated stats for the scheduler simulation, see 
 * scheduler.h for details. Memory allocated by your code will be free'd
 * by the similulator. Do NOT return a pointer to a stack variable.
 */
stats_t *stats() {
  stats_t *stats = malloc(sizeof(stats_t));
  stats->thread_count = queue_size(done_queue);
  stats->tstats = malloc(sizeof(stats_t)*stats->thread_count);
  int mean_turn = 0;
  int mean_wait = 0;
  for(int i = 0; i< stats->thread_count; ++i){
    thread_t *f = queue_dequeue(done_queue);
    struct times* ti = queue_dequeue(done_time);
    stats->tstats[i].tid = f->tid;
    stats->tstats[i].turnaround_time = ti->running_time;
    stats->tstats[i].waiting_time = ti->waiting_time;
    stats->tstats[i].thread_count = stats->thread_count;
    mean_turn += ti->running_time;
    mean_wait += ti->waiting_time;
    printf("TI TURNAROUND: %d WAITING: %d\n", ti->running_time, ti->waiting_time);
  }
  stats->turnaround_time = mean_turn/stats->thread_count;
  stats->waiting_time = mean_wait/stats->thread_count;
  printf("STATS HERE\n");
//  while(queue_size(done_time) != 0){
//    struct times* ti = queue_dequeue(done_time);
//    printf("TI TURNAROUND: %d WAITING: %d\n", ti->running_time, ti->waiting_time); 
//    stats->tid = 
//  }
  return stats; 
}
