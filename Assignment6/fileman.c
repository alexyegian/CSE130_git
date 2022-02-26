/*********************************************************************
 *
 * Copyright (C) 2020-2022 David C. Harrison. All right reserved.
 *
 * You may not use, distribute, publish, or modify this code without 
 * the express written permission of the copyright holder.
 *
 ***********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

/*
 * Extended ASCII box drawing characters:
 * 
 * The following code:
 * 
 * printf("CSE130\n");
 * printf("%s%s Assignments\n", TEE, HOR);
 * printf("%s  %s%s Assignment 1\n", VER, TEE, HOR);
 * printf("%s  %s%s Assignment 2\n", VER, TEE, HOR);
 * printf("%s  %s%s Assignment 3\n", VER, TEE, HOR);
 * printf("%s  %s%s Assignment 4\n", VER, TEE, HOR);
 * printf("%s  %s%s Assignment 5\n", VER, TEE, HOR);
 * printf("%s  %s%s Assignment 6\n", VER, ELB, HOR);
 * printf("%s%s Labs\n", ELB, HOR);
 * printf("   %s%s Lab 1\n", TEE, HOR);
 * printf("   %s%s Lab 2\n", TEE, HOR);
 * printf("   %s%s Lab 3\n", ELB, HOR);
 * printf();
 * 
 * Shows this tree:
 * 
 * CSE130
 * ├─ Assignments
 * │  ├─ Assignment 1
 * │  ├─ Assignment 2
 * │  ├─ Assignment 3
 * │  ├─ Assignment 4
 * │  ├─ Assignment 5
 * |  └─ Assignment 6
 * └─ Labs
 *    ├─ Lab 1
 *    ├─ Lab 2
 *    └─ Lab 3
 */
#define TEE "\u251C"  // ├ 
#define HOR "\u2500"  // ─ 
#define VER "\u2502"  // │
#define ELB "\u2514"  // └

/*
 * Read at most SIZE bytes from FNAME starting at FOFFSET into BUF starting 
 * at BOFFSET.
 *
 * RETURN number of bytes read from FNAME into BUF, -1 on error.
 */
size_t fileman_read(
  const char *const fname, 
  const size_t foffset, 
  char *const buf, 
  const size_t boffset, 
  const size_t size) 
{
	printf("FNAME: %s\n", fname);
	FILE *f = fopen(fname, "r");
	if(f == NULL){
		printf("NULL\n");
		return -1;}
	size_t ind = 0;
	char c = fgetc(f);
	while(ind < foffset && c != EOF){
		c = fgetc(f);
		++ind;
	}
	ind = 0;
	while(c != EOF && ind < size){
		buf[boffset+ind] = c;
		c = fgetc(f);
		++ind;}
	fclose(f);
	return ind;
}

/*
 * Create FNAME and write SIZE bytes from BUF starting at BOFFSET into FNAME
 * starting at FOFFSET.
 * 
 * RETURN number of bytes from BUF written to FNAME, -1 on error or if FNAME
 * already exists
 */
size_t fileman_write(
  const char *const fname, 
  const size_t foffset, 
  const char *const buf, 
  const size_t boffset, 
  const size_t size) 
{
        FILE *f = fopen(fname, "r");
        if(f != NULL){
                printf("NOT NULL\n");
		fclose(f);
                return -1;}
	f = fopen(fname, "w");
        size_t off = fseek(f, foffset, SEEK_SET);
	if(off != 0){
		return -1;}
        printf("WRITE: %ld\n", size);
	size_t w = fwrite(buf + boffset, sizeof(char), size, f);
	printf("WRITTEN: %ld\n", w);
	fclose(f);
        return w;
}

/*
 * Append SIZE bytes from BUF to existing FNAME.
 * 
 * RETURN number of bytes from BUF appended to FNAME, -1 on error or if FNAME
 * does not exist
 */
size_t fileman_append(const char *const fname, const char *const buf, const size_t size) 
{
        FILE *f = fopen(fname, "r+");
        if(f == NULL){
                printf("NOT NULL\n");
                return -1;}
	int e = fseek(f, 0, SEEK_END);
	if(e != 0){
		return -1;}
	size_t w = fwrite(buf, sizeof(char), size, f);
	fclose(f);
	return w;
}

/*
 * Copy existing file FSRC to new file FDEST.
 *
 * Do not assume anything about the size of FSRC. 
 * 
 * RETURN number of bytes from FSRC written to FDEST, -1 on error, or if FSRC 
 * does not exists, or if SDEST already exists.
 */
size_t fileman_copy(const char *const fsrc, const char *const fdest) 
{
  FILE* f1 = fopen(fsrc, "r");
  FILE* f2 = fopen(fdest, "r");
  int flag = 0;
  if(f2 != NULL){
	  fclose(f2);
	  flag = 1;}
  if(f1 == NULL){
	  flag = 1;}
  else if(flag == 1 && f1 != NULL){
	  fclose(f1);}
  if(flag == 1){
	  return -1;}
  f2 = fopen(fdest, "w");
  char c = fgetc(f1);
  while(c != EOF){
	  fputc(c, f2);
	  c = fgetc(f1);
//  	  printf("%ld\n", f_size);
  }
  size_t g = ftell(f1);
//  int off = fseek(f1, 0, SEEK_END);
//  if(off != 0){
//	  fclose(f1);
//	  fclose(f2);
//	  return -1;}
//  size_t f_size = ftell(f1);
//  off = fseek(f1, 0, SEEK_SET);
//  printf("HERE: %ld\n", f_size);
//  f_size = fwrite(f1, sizeof(char), f_size, f2);
//  printf("HERE2: %ld\n", (f_size * sizeof(char)));
  fclose(f1);
  fclose(f2);
  return g;
}

/*
 * Print a hierachival directory view starting at DNAME to file descriptor FD 
 * as shown in an example below where DNAME == 'data.dir'
 *
 *   data.dir
 *       blbcbuvjjko
 *           lgvoz
 *               jfwbv
 *                   jqlbbb
 *                   yfgwpvax
 *           tcx
 *               jbjfwbv
 *                   demvlgq
 *                   us
 *               zss
 *                   jfwbv
 *                       ahfamnz
 *       vkhqmgwsgd
 *           agmugje
 *               surxeb
 *                   dyjxfseur
 *                   wy
 *           tcx
 */
void fileman_dir(const int fd, const char *const dname) 
{
}

/*
 * Print a hierachival directory tree view starting at DNAME to file descriptor 
 * FD as shown in an example below where DNAME == 'world'.
 * 
 * Use the extended ASCII box drawing characters TEE, HOR, VER, and ELB.
 *
 *   world
 *   ├── europe
 *   │   ├── france
 *   │   │   └── paris
 *   │   │       ├─- entente
 *   │   │       └── saint-germain
 *   │   └── uk
 *   │       ├── london
 *   │       │   ├── arsenal
 *   │       │   └── chelsea
 *   │       └── manchester
 *   │           └── city
 *   │           └── united
 *   └── usa
 *       ├── ma
 *       │   └── boston
 *       │       ├── bruins
 *       │       └── sox
 *       └── tx
 */
void fileman_tree(const int fd, const char *const dname) 
{
}
