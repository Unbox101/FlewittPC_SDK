#ifndef INCLUDE_LAZY_GLOBALS
#define INCLUDE_LAZY_GLOBALS
	#define STB_DS_IMPLEMENTATION
	#include "stb_ds.h"
	#include "CustomTypes.h"
	#define STB_SPRINTF_IMPLEMENTATION
	#pragma GCC diagnostic push 
	#pragma GCC diagnostic ignored "-Wsign-compare"
	#include "stb_sprintf.h"
	#pragma GCC diagnostic pop
	#define SSTRING_IMPLEMENTATION
	#include "SString.h"
	#include "simple_preprocessor.h"
	#include "custom_macro_language.h"
	#include <sys/stat.h>
#endif //INCLUDE_LAZY_GLOBALS