/*
 * types.h
 *
 *  Created on: Apr 17, 2010
 *      Author: anizzomc
 */

#ifndef TYPES_H_
#define TYPES_H_


typedef unsigned char byte;
typedef short int word;
typedef int dword;
typedef unsigned long qword;


typedef unsigned int uint32;

typedef char boolean;
#define true 1
#define false 0

typedef struct {
	int x;
	int y;
} coord_t;



#pragma pack(push)
#pragma pack (1) 		/* Alinear las siguiente estructuras a 1 byte */

/* Descriptor de segmento */
typedef struct {
  word limit,
       base_l;
  byte base_m,
       access,
       attribs,
       base_h;
} DESCR_SEG;


/* Descriptor de interrupcion */
typedef struct {
  word      offset_l,
            selector;
  byte      zero_byte,
            access;
  word	    offset_m;
  dword     offset_h,
            zero_dword;
} DESCR_INT;

/* IDTR  */
typedef struct {
  word  limit;
  dword base;
} IDTR;

#pragma pack(pop)

#endif /* TYPES_H_ */
