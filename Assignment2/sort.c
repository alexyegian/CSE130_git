/************************************************************************
 *
 * CSE130 Assignment 2
 *
 * Copyright (C) 2020-2022 David C. Harrison. All right reserved.
 *
 * You may not use, distribute, publish, or modify this code without 
 * the express written permission of the copyright holder.
 *
 ************************************************************************/
#include <pthread.h>
#include "merge.h"
#include <semaphore.h>
#include <stdio.h>
#include <string.h>
struct sort_struct{
  int left;
  int right;
  int* arr;
}sort_struct;
/* LEFT index and RIGHT index of the sub-array of ARR[] to be sorted */
void singleThreadedMergeSort(int arr[], int left, int right) 
{
  if (left < right) {
    int middle = (left+right)/2;
    singleThreadedMergeSort(arr, left, middle); 
    singleThreadedMergeSort(arr, middle+1, right); 
    merge(arr, left, middle, right); 
  } 
}

/* 
 * This function stub needs to be completed
 */
void* Test2(void* arg){
  struct sort_struct srt = *((struct sort_struct *) arg);
  int*nums = srt.arr;
  merge(nums, srt.left, (srt.left+srt.right)/2, srt.right);
  return NULL;}

void* Test(void* arg){
  struct sort_struct srt = *((struct sort_struct *) arg);
  int* nums = srt.arr;
  singleThreadedMergeSort(nums, srt.left, srt.right);
  return NULL;
}

void multiThreadedMergeSort(int arr[], int left, int right) 
{
  if(right-left <= 4){
    singleThreadedMergeSort(arr,left,right);
    return;
  }
  pthread_t threads[3];
  int base_num = (right-left)/4;
  struct sort_struct structs[3];
  for(int i = 0; i<3;++i){
    structs[i].arr = arr;
    structs[i].left = left + i*base_num+i;
    structs[i].right = left + (i+1)*base_num+i;
    pthread_create(&threads[i],NULL, Test, &structs[i]);
  }
  singleThreadedMergeSort(arr, left+3*base_num+3, right);
  for(int i = 0; i<3; ++i){
    pthread_join(threads[i], NULL);
  }
  structs[0].left = left;
  structs[0].right = left+2*base_num+1;
  pthread_create(&threads[0],NULL, Test2, &structs[0]);
  merge(arr, left+base_num*2+2, (left+base_num*3+2), right);
  pthread_join(threads[0], NULL);
  merge(arr, left, left+2*base_num+1, right);
}
