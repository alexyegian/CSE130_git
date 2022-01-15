/************************************************************************
 * 
 * CSE130 Assignment 1
 *  
 * POSIX Shared Memory Multi-Process Merge Sort
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
#include <fcntl.h>
#include<sys/stat.h>
#include <sys/mman.h>
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
void multiProcessMergeSort(int arr[], int left, int right) 
{
  FILE * f = fopen("psortmem","w+");
  fclose(f);
  int shid = shm_open("psortmem", O_RDWR|O_CREAT, 0);
  ftruncate(shid, (sizeof(int)*(1+(right-left)))/2);
  int* shmem =(int *) mmap(0, (sizeof(int)*(1+(right-left))/2), PROT_READ|PROT_WRITE, MAP_SHARED,shid,0);
  int rel_left = 0;
  int rel_mid = ((left+right)/2)-left+1;
  int rel_right = 1+(right-left);
  for(int i = 0; i<rel_mid; ++i){
    shmem[i] = arr[i+left];
  }
  int pid = fork();
  if(pid == 0){
    singleProcessMergeSort(shmem, rel_left, rel_mid-1);
    munmap(shmem, (sizeof(int)*(1+(right-left)))/2);
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
  }
  munmap(shmem, (sizeof(int)*(1+(right-left)))/2);
  shm_unlink("psortmem");
  close(shid);
  remove("psortmem");
}
