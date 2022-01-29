/*********************************************************************
 *
* Copyright (C) 2020-2022 David C. Harrison. All right reserved.
 *
 * You may not use, distribute, publish, or modify this code without 
 * the express written permission of the copyright holder.
 *
 ***********************************************************************/
#include <stdio.h>
#include "cartman.h"
#include <pthread.h>
#include <stdbool.h>
/*
 * Callback when CART on TRACK arrives at JUNCTION in preparation for
 * crossing a critical section of track.
*/

pthread_mutex_t reserve_mut;
pthread_cond_t reserve_cond;
int locks[10000];
int num_tracks;
struct cart_move{
	unsigned int cart_num;
	enum track track_num;
	enum junction junc_num;} cart_move;
struct cart_move c_moves[10000];
void arrive(unsigned int cart, enum track track, enum junction junction) 
{
  pthread_t new_cart;
  c_moves[cart].cart_num = cart;
  c_moves[cart].track_num = track;
  c_moves[cart].junc_num = junction;
  pthread_create(&new_cart, NULL, cart_start, &c_moves[cart]);
}

void* cart_start(void * args){
  struct cart_move c_move = *((struct cart_move *)args);
  enum track track = c_move.track_num;
  enum junction junction = c_move.junc_num;
  unsigned int cart = c_move.cart_num;
  if((int)track == (int)junction){
    if(junction+1 == num_tracks){
      reserve2(cart, junction, 0);
      cross(cart, track, junction);
      release2(cart, junction, 0);
    }
    else{
      reserve2(cart, junction, junction+1);
      cross(cart, track, junction);
      release2(cart, junction, junction+1);

    }
  }
  else if(junction-1 == track || (junction == 0 && track == num_tracks-1)){
    if(junction-1 == -1){
      reserve2(cart, junction, num_tracks-1);
      cross(cart, track, junction);
      release2(cart, junction, num_tracks-1);
    }
    else{
      reserve2(cart, junction, junction-1);
      cross(cart, track, junction);
      release2(cart, junction, junction-1);
    }
  }
  return NULL;
}
void reserve2(unsigned int cart, enum junction junc1, enum junction junc2){
  pthread_mutex_lock(&reserve_mut);
  while(locks[junc1] != 0 || locks[junc2]!=0){
    pthread_cond_wait(&reserve_cond, &reserve_mut);}
    locks[junc1] = 1;
    locks[junc2] = 1;
    reserve(cart, junc1);
    reserve(cart, junc2); 
    pthread_mutex_unlock(&reserve_mut);
    return;
}
void release2(unsigned int cart, enum junction junc1, enum junction junc2){
    pthread_mutex_lock(&reserve_mut);
    locks[junc1] = 0;
    locks[junc2] = 0;
    release(cart, junc1);
    release(cart, junc2);
    pthread_mutex_unlock(&reserve_mut);
    pthread_cond_broadcast(&reserve_cond);
    pthread_exit(0);
}

void cartman(unsigned int tracks) 
{
  num_tracks = tracks;
  pthread_mutex_init(&reserve_mut, NULL);
  pthread_cond_init(&reserve_cond, NULL);
}
