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
pthread_mutex_t junc_muts[10000];
int locks[10000];
int num_tracks;
struct test_struct{
	unsigned int num;}test_struct;
struct cart_move{
	unsigned int cart_num;
	enum track track_num;
	enum junction junc_num;} cart_move;
struct cart_move c_moves[10000];
void arrive(unsigned int cart, enum track track, enum junction junction) 
{
  printf("CART %d ARRIVED\n", cart);
//  printf("CART ARRIVED: %d TRACK: %d JUNC: %d\n", cart, (int)track, (int)junction);
  pthread_t new_cart;
  c_moves[cart].cart_num = cart;
  c_moves[cart].track_num = track;
  c_moves[cart].junc_num = junction;
 // tsts[cart].num = cart;
//  printf("CMOVE: CART: %d TRACK: %d, JUNC: %d\n", c_moves[cart].cart_num, c_moves[cart].track_num, c_moves[cart].junc_num);
//  pthread_create(&new_cart, NULL, test_start, &tsts[cart]);
  pthread_create(&new_cart, NULL, cart_start, &c_moves[cart]);
//  pthread_join(new_cart, NULL);
//  printf("CART DONE\n");
}
void* test_start(void* args){
	 // struct sort_struct srt = *((struct sort_struct *) arg);
  struct test_struct x = *((struct test_struct *)args);
  printf("CART NUM: %d\n", x.num);
  return NULL;}

void* cart_start(void * args){
  struct cart_move c_move = *((struct cart_move *)args);
//  printf("CMOVE 2: CART: %d TRACK: %d, JUNC: %d\n", c_move.cart_num, c_move.track_num, c_move.junc_num);
  enum track track = c_move.track_num;
  enum junction junction = c_move.junc_num;
  unsigned int cart = c_move.cart_num;
//  printf("CART START: CART: %d JUNC: %d TRACK: %d\n", cart, junction, track);
  if((int)track == (int)junction){
    if(junction+1 == num_tracks){
      reserve2(cart, junction, 0);
      printf("CART %d RESERVED JUNCTIONS %d and %d\n", cart, junction, 0);
      cross(cart, track, junction);
      release2(cart, junction, 0);
 //     printf("CART %d RELEASED JUNCTIONS %d and %d\n", cart, junction, 0);
    }
    else{
      reserve2(cart, junction, junction+1);
      printf("CART %d RESERVED JUNCTIONS %d and %d\n", cart, junction, junction+1);
      cross(cart, track, junction);
      release2(cart, junction, junction+1);
//      printf("CART %d RELEASED JUNCTIONS %d and %d\n", cart, junction, junction+1);

    }
  }
  else if(junction-1 == track || (junction == 0 && track == num_tracks-1)){
    if(junction-1 == -1){
      reserve2(cart, junction, num_tracks-1);
      printf("CART %d RESERVED JUNCTIONS %d and %d\n", cart, junction, num_tracks-1);
      cross(cart, track, junction);
      release2(cart, junction, num_tracks-1);
//      printf("CART %d RELEASED JUNCTIONS %d and %d\n", cart, junction, num_tracks-1);
    }
    else{
      reserve2(cart, junction, junction-1);
      printf("CART %d RESERVED JUNCTIONS %d and %d\n", cart, junction, junction-1);
      cross(cart, track, junction);
      release2(cart, junction, junction-1);
 //     printf("CART %d RELEASED JUNCTIONS %d and %d\n", cart, junction, junction-1);
    }
  }
  else{
    printf("NOT RESERVED: CART: %d TRACK: %d JUNCTION: %d\n", cart, track, junction);}
  return NULL;
}
//void cross(unsigned int cart, enum track track, enum junction junction){}

/*
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
}
*/
void reserve2(unsigned int cart, enum junction junc1, enum junction junc2){
  pthread_mutex_lock(&reserve_mut);
  while(locks[junc1] != 0 || locks[junc2]!=0){
    printf("CHECK CART: %d:\n", cart);
    pthread_cond_wait(&reserve_cond, &reserve_mut);}
    printf("CHECK CART: %d:\n", cart);
//    int junc1_ret = pthread_mutex_trylock(&junc_muts[junc1]);
//    int junc2_ret = pthread_mutex_trylock(&junc_muts[junc2]);
//  if(locks[junc1] == 0 && locks[junc2] == 0){
    printf("RESERVE JUNCTIONS: %d and %d\n", junc1, junc2);
    locks[junc1] = 1;
    locks[junc2] = 1;
    pthread_mutex_lock(&junc_muts[junc1]);
    pthread_mutex_lock(&junc_muts[junc2]);
    reserve(cart, junc1);
    reserve(cart, junc2); 
    pthread_mutex_unlock(&reserve_mut);
//    pthread_cond_broadcast(&reserve_cond);
    return;
//    }
}
/*
void reserve2(unsigned int cart, enum junction junc1, enum junction junc2){
  while(true){
    pthread_mutex_lock(&reserve_mut);
    int junc1_ret = pthread_mutex_trylock(&junc_muts[junc1]);
    int junc2_ret = pthread_mutex_trylock(&junc_muts[junc2]);
    if(junc1_ret == 0 && junc2_ret == 0){
      printf("RESERVE JUNCTIONS: %d and %d\n", junc1, junc2);
      reserve(cart, junc1);
      reserve(cart, junc2);
      pthread_mutex_unlock(&reserve_mut);
      return;
    }
    else if(junc1_ret == 0){
      pthread_mutex_unlock(&junc_muts[junc1]);}
    else if(junc2_ret == 0){
      pthread_mutex_unlock(&junc_muts[junc2]);}
    pthread_mutex_unlock(&reserve_mut); }
}
*/
void release2(unsigned int cart, enum junction junc1, enum junction junc2){
    pthread_mutex_lock(&reserve_mut);
    locks[junc1] = 0;
    locks[junc2] = 0;
    release(cart, junc1);
    release(cart, junc2);
    pthread_mutex_unlock(&junc_muts[junc1]);
    pthread_mutex_unlock(&junc_muts[junc2]);
//    printf("RELEASE JUNCTIONS: %d and %d\n", junc1, junc2);
    pthread_mutex_unlock(&reserve_mut);
    pthread_cond_broadcast(&reserve_cond);
    printf("BROADCAST RESERVE COND\n");
    printf("CART %d FINISHED\n", cart);
//    release(cart, junc1);
//    release(cart, junc2);
}
//void reserve(unsigned int cart, enum junction junction){
  
//}
//void release(unsigned int cart, enum junction junction){}

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
  printf("TRACKS: %d\n", tracks);
  num_tracks = tracks;
  pthread_mutex_init(&reserve_mut, NULL);
  pthread_cond_init(&reserve_cond, NULL);
  for(int i = 0; i<tracks; ++i){
    pthread_mutex_init(&junc_muts[i], NULL);}
}
