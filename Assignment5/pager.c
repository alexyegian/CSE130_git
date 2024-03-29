/************************************************************************
 *
 * CSE130 Assignment 5
 *
 * Copyright (C) 2021-2022 David C. Harrison. All right reserved.
 *
 * You may not use, distribute, publish, or modify this code without 
 * the express written permission of the copyright holder.
 * 
 ************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "queue.h"
#include "simulator.h"
int max_size;
int alg;
void* queue;
int* freq_list;
int step = 0;
int sec_chance=-1;
int* bit_refs;
struct freq_page{
	unsigned int uses;
	unsigned int page;
	unsigned int last;};
/**
 * Initialise your ALGORITHM pager with FRAMES frames (physical pages).
 */
void pager_init(enum algorithm algorithm, unsigned int frames) {
	max_size = frames;
	alg = algorithm;
	queue = queue_create();
	if(alg == 4 || alg == 3){
		bit_refs = malloc(1000000*sizeof(int));}
	if(alg == 2){
		freq_list = malloc(1000000*sizeof(int));}
}

/**
 * Free any dynamically allocated resources.
 */
void pager_destroy() {
	if(alg == 4 || alg == 3){
		free(bit_refs);}
	if(alg == 2){
		while(queue_size(queue) > 0){
			struct freq_page* t = queue_dequeue(queue);
			free(t);}
		free(freq_list);}
	queue_destroy(queue);
}

/**
 * A request has been made for virtual page PAGE. If your pager does
 * not currently have PAGE in physical memory, request it from the 
 * simulator.
 */
//static bool find_page(unsigned int a, unsigned int b){
//	printf("A: %d B: %d\n", a, b);
//	return a == b;
//}
bool find_elem(void *q, unsigned int val){
  void* temp_queue = queue_create();
  int flag = 0;
  while(queue_size(q) > 0){
	  unsigned int v = (unsigned int)((uintptr_t)queue_dequeue(q));
	  if(v != val){
		  queue_enqueue(temp_queue,(char*) ((long)v));
	  }
	  else{
		  if(alg != 1){
			  queue_enqueue(temp_queue, (char *) ((long)v));
		  }
		  if(alg == 3){
			  bit_refs[val] = 1;}
		  if(alg == 4){
			  bit_refs[val] = 0;}
		  flag = 1;
	  }
  }
  while(queue_size(temp_queue)>0){
	  unsigned int v = (unsigned int) ((uintptr_t)queue_dequeue(temp_queue));
	  queue_enqueue(q, (char *) ((long)v));
  }
  queue_destroy(temp_queue);
  if(flag == 1){
	  return true;
  }
  return false;
}

bool find_elem_freq(void *q, unsigned int val){
  void* temp_queue = queue_create();
  int flag = 0;
  while(queue_size(q) > 0){
          struct freq_page* v;
	  v = queue_dequeue(q);
          if(v->page != val){
                queue_enqueue(temp_queue, v);
          }
          else{
		  v->uses += 1;
		  v->last = step;
                  queue_enqueue(temp_queue, v);
                  flag = 1;
          }
  }
  while(queue_size(temp_queue)>0){
         struct freq_page* v = queue_dequeue(temp_queue);
         queue_enqueue(q, v);
  }
  queue_destroy(temp_queue);
  if(flag == 1){
          return true;
  }
  return false;
}


static int sort_freq(void* a, void*b){
  	int x = ((struct freq_page*)a)->uses - ((struct freq_page*)b)->uses;
	if(x == 0){
		x = ((struct freq_page*)a)->last - ((struct freq_page*)b)->last;	}
	return x;
}

void print_sorted(void* q){
	void* temp = queue_create();
	while(queue_size(q)>0){
		struct freq_page* v = queue_dequeue(q);
		printf("PAGE: %d FREQ %d LAST: %d\n",v->page, v->uses, v->last);
		queue_enqueue(temp, v);
	}
	while(queue_size(temp)>0){
		struct freq_page* v = queue_dequeue(temp);
		queue_enqueue(q, v);
	}
}


void pager_request(unsigned int page) {
  if(alg == 4){
	  bit_refs[page] = 0;}
  if(alg != 2){
  bool found = find_elem(queue, page);
  if(found == true){
	  if(alg == 1){
		  queue_enqueue(queue, (char *) ((long)page));
	  }
	  }
  else{
	  sim_get_page(page);
	  if(queue_size(queue) == max_size){
		  if(alg == 3){
		    unsigned int temp = (unsigned int) ((uintptr_t)queue_dequeue(queue));
		    while(bit_refs[temp] == 1){
			    bit_refs[temp] = 0;
			    queue_enqueue(queue,(char *) ((long)temp));
			    temp = (unsigned int) ((uintptr_t)queue_dequeue(queue));}
		    queue_enqueue(queue, (char *) ((long)page));
		  }
		  else if(alg == 4){
		    unsigned int temp = (unsigned int) ((uintptr_t)queue_dequeue(queue));
		    while(bit_refs[temp] == 0){
                            bit_refs[temp] = 1;
                            queue_enqueue(queue, (char*) ((long)temp));
                            temp = (unsigned int) ((uintptr_t)queue_dequeue(queue));}
                    queue_enqueue(queue, (char*)((long)page));

		  }
		  else{ 
		  queue_dequeue(queue);
		  queue_enqueue(queue, (char *) ((long) page));}}
	  else{
		  queue_enqueue(queue, (char *) ((long)page));}}}
  else{
	  ++freq_list[page];
	  bool found = find_elem_freq(queue,page);
	  if(!found){
		  sim_get_page(page);
		  struct freq_page* p = malloc(sizeof(struct freq_page));
		  p->page = page;
		  p->uses = freq_list[page];
		  p->last = step;
		  if(queue_size(queue) == max_size){
		          struct freq_page *t = queue_head(queue);
				queue_dequeue(queue);
				freq_list[t->page] = 0;
		  	        free(t);
		        	queue_enqueue(queue, p);}
		  else{
			  queue_enqueue(queue, p);
			  }
			  }
	  queue_sort(queue, sort_freq);
	  print_sorted(queue);
		  }
  ++step;
}
