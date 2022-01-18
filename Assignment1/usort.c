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
#include <errno.h>
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
void multiProcessMergeSort(int arr[], int left, int right){
//  FILE *f = fopen("usortmem","w+");
// fclose(f);
//  key_t key = ftok("usortmem", 111);
  if(right-left <=0){
    return;
  }
  int shid = shmget(IPC_PRIVATE, (sizeof(int)*(1+(right-left)))/2, 0666|IPC_CREAT);
  int * shmem= (int *)shmat(shid,(void *)0,0);
  int rel_left = 0;
  int rel_mid = ((left+right)/2)-left+1;
  int rel_right = 1+(right-left);
  for(int i = 0; i<rel_mid;++i){
    shmem[i]=arr[left+i];
  }

  int pid = fork();
  if(pid == 0){
    singleProcessMergeSort(shmem, rel_left, rel_mid-1);
    shmdt(shmem);
    exit(0);
  }
  else{
    singleProcessMergeSort(arr, left+rel_mid, left+rel_right-1);
    wait(NULL);
    for(int i = rel_left; i<rel_mid;++i){
      arr[i+left] = shmem[i];
    }
    int mid = ((left+right)/2);
    merge(arr, left, mid, right);
    shmdt(shmem);
    shmctl(shid,IPC_RMID,NULL);
//    remove("usortmem");
  }	
}
