/************************************************************************
 * 
 * CSE130 Assignment 1
 * 
 * UNIX Shared Memory Multi-Process Merge Sort
 * 
* Copyright (C) 2020-2022 David C. Harrison. All right reserved.
 *
 * You may not use, distribute, publish, or modify this code without 
 * the express written permission of the copyright holder.
 *
 ************************************************************************/
#include <sys/ipc.h>
#include "merge.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <string.h>

/* 
 * Merge Sort in the current process a sub-array of ARR[] defined by the 
 * LEFT and RIGHT indexes.
 */
void singleProcessMergeSort(int arr[], int left, int right) 
{
  if (left < right) {
    int middle = (left+right)/2;
    singleProcessMergeSort(arr, left, middle); 
    singleProcessMergeSort(arr, middle+1, right); 
    merge(arr, left, middle, right); 
  } 
}

/* 
 * Merge Sort in the current process and at least one child process a 
 * sub-array of ARR[] defined by the LEFT and RIGHT indexes.
 */
void multiProcessMergeSort(int arr[], int left, int right) 
{
  // Delete this line, it's only here to fail the code quality check
 // printf("START\n");
  key_t key = ftok("shared", 211);
  if(right-left <=0){
    return;
  }
//  for(int i = left; i<=right;++i){
//    printf("BEG ARR: %d\n", arr[i]);}
  int shid = shmget(key, sizeof(int)*(1+(right-left)), 0666|IPC_CREAT);
  int * shmem= (int *)shmat(shid,(void *)0,0);
//  printf("SIZE SHOULD BE: %ld LR: %d\n",sizeof(int)*(1+(right-left)), 1+(right-left));
//  printf("%p\n", shmem);
  for(int i = 0; i<=(right-left);++i){
//    printf("ARR I: %d\n", arr[left+i]);
//    printf("I: %d\n",i);
    shmem[i]=arr[left+i];
  }
//  printf("AFTER THING\n");
  int rel_left = 0;
  int rel_mid = ((left+right)/2)-left+1;
  int rel_right = 1+(right-left);
//  printf("L: %d M: %d R: %d\n",rel_left, rel_mid, rel_right);
  int pid = fork();
  if(pid == 0){
    key = ftok("shared", 211);
    shid = shmget(key, sizeof(int)*(1+(right-left)), IPC_CREAT);
    shmem = (int *) shmat(shid, (void *) NULL, 0);
//    for(int i = 0; i<rel_mid;++i){
//      printf("BEF: %d\n",shmem[i]);}
    singleProcessMergeSort(shmem, rel_left, rel_mid-1);
    printf("SORTED\n");
    for(int i = 0; i<rel_mid;++i){
      printf("AFT: %d\n",shmem[i]);}
    shmdt(shmem);
    exit(0);
  }
  else{
    singleProcessMergeSort(arr, left+rel_mid, left+rel_right-1);
    wait(NULL);
//    for(int i = left+rel_mid; i<left+rel_right;++i){
//      printf("START ARR: %d\n",arr[i]);}
//    singleProcessMergeSort(arr, left+rel_mid, left+rel_right-1);
    for(int i = left+rel_mid; i<left+rel_right;++i){
      printf("AFT ARR: %d\n",arr[i]);}

 //   printf("CHILD ENDED\n");
    for(int i = rel_left; i<rel_mid;++i){
      arr[i+left] = shmem[i];
    }
//    for(int i = 0; i<rel_right;++i){
//      printf("AFT 2: %d\n", arr[i+left]);}
    int mid = ((left+right)/2);
//    printf("LEFT: %d MID: %d RIGHT: %d\n",left,mid,right);
    merge(arr, left, mid, right);
//    for(int i = 0; i<(right-left);++i){
//      printf("ELEMENT %d IS: %d\n", i, shmem[i]);
//      arr[i+left] = shmem[i];
//    }
//    for(int i = left; i<=right; ++i){
//      printf("ARR: %d\n", arr[i]);}
//    printf("END\n");
    shmdt(shmem);
    shmctl(shid,IPC_RMID,NULL);
//DETACH AND DELETE MEMORY
  }


  // Your code goes here 
}
