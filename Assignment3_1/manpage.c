/*********************************************************************
 *
* Copyright (C) 2020-2022 David C. Harrison. All right reserved.
 *
 * You may not use, distribute, publish, or modify this code without 
 * the express written permission of the copyright holder.
 *
 ***********************************************************************/
#include <stdio.h>
#include "manpage.h"
#include <pthread.h>
/*
 * Create a thread for each of seven manpage paragraphs and have them synchronize
 * their invocations of showParagraph() so the manual page displays as follows:
 *
 --------- 8< ---------------------------------------------------
 
A semaphore S is an unsigned-integer-valued variable.
Two operations are of primary interest:
 
P(S): If processes have been blocked waiting on this semaphore,
 wake one of them, else S <- S + 1.
 
V(S): If S > 0 then S <- S - 1, else suspend execution of the calling process.
 The calling process is said to be blocked on the semaphore S.
 
A semaphore S has the following properties:

1. P(S) and V(S) are atomic instructions. Specifically, no
 instructions can be interleaved between the test that S > 0 and the
 decrement of S or the suspension of the calling process.
 
2. A semaphore must be given an non-negative initial value.
 
3. The V(S) operation must wake one of the suspended processes. The
 definition does not specify which process will be awakened.

 --------- 8< ---------------------------------------------------
 *
 * As supplied, shows random single messages.
 */
int cur_page;
void * print_page(void * a);
pthread_mutex_t page_mut;
pthread_cond_t cond_page;
void manpage() 
{
  pthread_mutex_init(&page_mut, NULL);
  pthread_cond_init(&cond_page, NULL);
  pthread_t threads[7]; 
  int max_page_ind = 6;
  for(int i = 0; i<=max_page_ind; ++i){
    pthread_create(&threads[i], NULL, print_page, NULL);}
  for(int i = 0; i<=max_page_ind; ++i){
    pthread_join(threads[i], NULL);}
}

void *print_page(void * args){
  int pid = getParagraphId();
//  printf("PID: %d\n",pid);
  pthread_mutex_lock(&page_mut);
  while(pid != cur_page){
    pthread_cond_wait(&cond_page, &page_mut);}
//  printf("CUR PAGE: %d PAGE ID: %d\n",cur_page, pid);
  showParagraph(); 
  cur_page += 1;
  pthread_mutex_unlock(&page_mut);
  pthread_cond_broadcast(&cond_page);
  return NULL; 
}
