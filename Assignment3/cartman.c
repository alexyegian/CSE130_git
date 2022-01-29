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
pthread_mutex_t junc_muts[10000];
int num_tracks;

struct cart_move{
	unsigned int cart_num;
	enum track track_num;
	enum junction junc_num;} cart_move;
void arrive(unsigned int cart, enum track track, enum junction junction) 
{
  pthread_t new_cart;
  struct cart_move c_move;
  c_move.cart_num = cart;
  c_move.track_num = track;
  c_move.junc_num = junction;
  pthread_create(&new_cart, NULL, cart_start, &c_move);
  pthread_join(new_cart, NULL);
  printf("CART DONE");
}
void* cart_start(void * args){
  struct cart_move c_move = *((struct cart_move *)args);
  cross(c_move.cart_num, c_move.track_num, c_move.junc_num);
  return NULL;
}
void cross(unsigned int cart, enum track track, enum junction junction){
  if((int)track == (int)junction){
    if(junction+1 == num_tracks){
      reserve2(cart, junction, 0);
      release2(cart, junction, 0);
    }
    else{
      reserve2(cart, junction, junction+1);
      release2(cart, junction, junction+1);
    }
  }
  else if(junction == track-1 || (junction == 0 && track == num_tracks-1)){
    if(junction-1 == -1){
      reserve2(cart, junction, num_tracks-1);
      release2(cart, junction, num_tracks-1);}
    else{
      reserve2(cart, junction, junction-1);
      release2(cart, junction, junction-1);
    }
  }
  printf("CROSS HERE\n");
}
void reserve2(unsigned int cart, enum junction junc1, enum junction junc2){
  while(true){
    pthread_mutex_lock(&reserve_mut);
    int junc1_ret = pthread_mutex_trylock(&junc_muts[junc1]);
    int junc2_ret = pthread_mutex_trylock(&junc_muts[junc2]);
    if(junc1_ret == 0 && junc2_ret == 0){
      pthread_mutex_unlock(&reserve_mut);
      return;
    }
    else if(junc1_ret == 0){
      pthread_mutex_unlock(&junc_muts[junc1]);}
    else if(junc2_ret == 0){
      pthread_mutex_unlock(&junc_muts[junc2]);}
    pthread_mutex_unlock(&reserve_mut); }
}
void reverse2(unsigned int cart, enum junction junc1, enum junction junc2){
    pthread_mutex_unlock(&junc_muts[junc1]);
    pthread_mutex_unlock(&junc_muts[junc2]);
}
void reserve(unsigned int cart, enum junction junction){
  
}
void release(unsigned int cart, enum junction junction){}
/*
 * Initialise the CART Manager for TRACKS tracks.
 *
 * Some implementations will do nothing, most will initialise necessary
 * concurrency primitives.
 */
void cartman(unsigned int tracks) 
{
//  pthread_mutex_t junc_muts[tracks];
//  pthread_mutex_t track_muts[tracks];
  pthread_mutex_init(&reserve_mut, NULL);
  for(int i = 0; i<tracks; ++i){
    pthread_mutex_init(&junc_muts[i], NULL);}
}
