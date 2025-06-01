#ifndef INCLUDE_CUSTOM_TYPES
#define INCLUDE_CUSTOM_TYPES
#include "stdio.h"
#ifndef print
	#define print(...) printf(__VA_ARGS__);printf("\n");
#endif

#include <stdint.h>
#include <stdbool.h>
typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;

typedef int64_t s64;
typedef int32_t s32;
typedef int16_t s16;
typedef int8_t s8;

typedef float f32;
typedef double f64;

typedef u64* 	u64_arr;	//stb_ds dynamic array
typedef u32* 	u32_arr;	//stb_ds dynamic array
typedef u16* 	u16_arr;	//stb_ds dynamic array
typedef u8* 	u8_arr;		//stb_ds dynamic array

typedef s64* 	s64_arr;	//stb_ds dynamic array
typedef s32* 	s32_arr;	//stb_ds dynamic array
typedef s16* 	s16_arr;	//stb_ds dynamic array
typedef s8* 	s8_arr;		//stb_ds dynamic array

typedef f32* 	f32_arr;	//stb_ds dynamic array
typedef f64* 	f64_arr;	//stb_ds dynamic array

typedef void* func_t;
typedef void (*procedure)(void);

#define cast(t, m) ((t)(m))
#define autocast(m) ((void*)(m))
#define bitcast(t, m) (*cast(t*, &(m)))

#endif//INCLUDE_CUSTOM_TYPES
