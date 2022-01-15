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
//  printf("OPEN\n");
  FILE * f = fopen("psortmem","w+");
//  printf("CLOSE\n");
  fclose(f);
  // Delete this line, it's only here to fail the code quality chec
  int shid = shm_open("psortmem", O_RDWR|O_CREAT, 0);
  ftruncate(shid, (sizeof(int)*(1+(right-left)))/2);
  printf("AFTER OPEN NO: %d ERROR: %d\n", shid, errno);
  int* shmem =(int *) mmap(0, (sizeof(int)*(1+(right-left))/2), PROT_READ|PROT_WRITE, MAP_SHARED,shid,0);
  printf("BEF LOOP\n");
//  for(int i = 0; i<rel_mid; ++i){
//    shmem[i] = arr[i+left];
//    printf("%d: %d\n",i, shmem[i]);
//  }
  int rel_left = 0;
  int rel_mid = ((left+right)/2)-left+1;
  int rel_right = 1+(right-left);
  for(int i = 0; i<rel_mid; ++i){
    shmem[i] = arr[i+left];
    printf("%d: %d\n",i, shmem[i]);
  }
  printf("L: %d M: %d R: %d\n",rel_left, rel_mid, rel_right);
  int pid = fork();
  if(pid == 0){
    printf("CHILD\n");
    printf("AFTER OPEN NO: %d ERROR: %d\n", shid, errno);
    printf("HERE\n");
    for(int i = 0; i<rel_mid;++i){
      printf("I: %d\n",i);
      printf("BEF: %d\n",shmem[i]);
      }
    printf("HERE 2\n");
    singleProcessMergeSort(shmem, rel_left, rel_mid-1);
    printf("SORTED\n");
    for(int i = 0; i<rel_mid;++i){
      printf("AFT: %d\n",shmem[i]);}
    munmap(shmem, (sizeof(int)*(1+(right-left)))/2);
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
  }
  printf("REMOVE\n");
  munmap(shmem, (sizeof(int)*(1+(right-left)))/2);
  shm_unlink("psortmem");
  close(shid);
  remove("psortmem");
  // Your code goes here 
}
