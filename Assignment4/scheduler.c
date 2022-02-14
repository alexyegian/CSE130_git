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

typedef struct times{
        thread_t *t;
        int running_time;
        int waiting_time;
        int last_on;
        int beg_time;}times;


void* ready_queue;
void* io_queue;
void* done_queue;
times * cur_t = NULL;
times * io_t = NULL;
int ticks = 0;
int beg_tid = 0;

int last_on_total = 0;
int alg;
int quant;
/**
 * Initalise a scheduler implemeting the requested ALGORITHM. QUANTUM is only
 * meaningful for ROUND_ROBIN.
 */

static int sort_priority(void *a, void *b) {
    return ((times*)a)->t->priority - ((times*)b)->t->priority;
}


void scheduler(enum algorithm algorithm, unsigned int quantum) {
    alg = algorithm;
    ready_queue = queue_create(); 
    io_queue = queue_create();
    done_queue = queue_create();
    alg = algorithm;
    quant = quantum;
}


void priority_sort(void* queue){
    queue_sort(queue, sort_priority);
    if(alg == 3){
	    struct times* h = queue_head(queue);
	    if(cur_t != NULL && h!=NULL && h->t->priority < cur_t->t->priority){
              h = queue_dequeue(queue);
	      queue_enqueue(queue, cur_t);
	      cur_t->last_on = ticks;
	      cur_t = h;
	      cur_t->waiting_time += ticks-cur_t->last_on;
	      cur_t->last_on = ticks;
	      sim_dispatch(cur_t->t);
	    }}
}
/**
 * Thread T is ready to be scheduled for the first time.
 */
void sys_exec(thread_t *t) {
    
  if(queue_size(ready_queue)==0 && cur_t == NULL && alg != 2 && alg != 3){
      struct times* ti = malloc(sizeof(struct times));
      ti->last_on = ticks;
      ti->running_time = 0;
      ti->waiting_time = 0;
      ti->beg_time = ticks;
      ti->t = t;
    cur_t = ti;
    sim_dispatch(cur_t->t);
  }
  else{
      struct times* ti = malloc(sizeof(struct times));
      ti->last_on = ticks;
      ti->running_time = 0;
      ti->waiting_time = 0;
      ti->beg_time = ticks;
      ti->t = t;
      queue_enqueue(ready_queue, ti);
  }
}

/**
 * Programmable clock interrupt handler. Call sim_time() to find out
 * what tick this is. Called after all calls to sys_exec() for this
 * tick have been made.
 */
void tick() {
    if (alg == 3){
	priority_sort(ready_queue);}
    if(cur_t == NULL){
      if(alg == 2){
        priority_sort(ready_queue);}
      cur_t = queue_dequeue(ready_queue);
      if(cur_t != NULL){
	last_on_total = 0;
        cur_t->waiting_time += ticks-cur_t->last_on;
        cur_t->last_on = ticks;
	sim_dispatch(cur_t->t);}}
    else if(alg == 1 && last_on_total >= quant && queue_size(ready_queue)){
	cur_t->last_on = ticks;
	queue_enqueue(ready_queue, cur_t);
	cur_t = queue_dequeue(ready_queue);
	cur_t->waiting_time += ticks-cur_t->last_on;
	cur_t->last_on = ticks;
	sim_dispatch(cur_t->t);
	last_on_total = 0;
    }
    if(cur_t != NULL){
	   ++last_on_total;} 
    else if(last_on_total == quant){
	    last_on_total = 0;}
    ++ticks;
}

/**
 * Thread T has completed execution and should never again be scheduled.
 */
void sys_exit(thread_t *t) {
    cur_t->running_time = ticks-cur_t->beg_time;
    queue_enqueue(done_queue, cur_t);
    cur_t = NULL;
}

/**
 * Thread T has requested a read operation and is now in an I/O wait queue.
 * When the read operation starts, io_starting(T) will be called, when the
 * read operation completes, io_complete(T) will be called.
 */
void sys_read(thread_t *t) {
    if(queue_size(io_queue)==0 && io_t == NULL){
      io_t = cur_t;
      io_t->last_on = ticks;
    }
    else{
      cur_t->last_on = ticks;
      queue_enqueue(io_queue, cur_t);
    }
    cur_t = NULL;
}

/**
 * Thread T has requested a write operation and is now in an I/O wait queue.
 * When the write operation starts, io_starting(T) will be called, when the
 * write operation completes, io_complete(T) will be called.
 */
void sys_write(thread_t *t) {
    if(queue_size(io_queue)==0 && io_t == NULL){
      io_t = cur_t;
      io_t->last_on = ticks;
    }
    else{
      queue_enqueue(io_queue, cur_t);
      cur_t->last_on = ticks;
    }
    cur_t = NULL;
}

/**
 * An I/O operation requested by T has completed; T is now ready to be 
 * scheduled again.
 */
void io_complete(thread_t *t) {
  io_t->last_on = ticks;
  queue_enqueue(ready_queue, io_t);
  io_t = queue_dequeue(io_queue);
  if(cur_t == NULL){
    cur_t = queue_dequeue(ready_queue);
    if(cur_t != NULL){
        cur_t->waiting_time += ticks-cur_t->last_on;
        cur_t->last_on = ticks;
        sim_dispatch(cur_t->t);		  
    }
  }
}

/**
 * An I/O operation requested by T is starting; T will not be ready for
 * scheduling until the operation completes.
 */
void io_starting(thread_t *t) {
    io_t->waiting_time += (ticks-1)-io_t->last_on;
    io_t->last_on = ticks;
    io_t->waiting_time += (ticks)-io_t->last_on;
    io_t->last_on = ticks;
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
    struct times* ti = queue_dequeue(done_queue);
    stats->tstats[i].tid = ti->t->tid;
    stats->tstats[i].turnaround_time = ti->running_time;
    stats->tstats[i].waiting_time = ti->waiting_time;
    stats->tstats[i].thread_count = stats->thread_count;
    mean_turn += ti->running_time;
    mean_wait += ti->waiting_time;
    free(ti);
  }
  stats->turnaround_time = mean_turn/stats->thread_count;
  stats->waiting_time = mean_wait/stats->thread_count;
  queue_destroy(ready_queue);
  queue_destroy(io_queue);
  queue_destroy(done_queue);
  return stats; 
}
