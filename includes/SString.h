

/*
#define SSTRING_IMPLEMENTATION
#include "SString.h"
*/

#ifndef INCLUDE_SSTRING
#define INCLUDE_SSTRING

#define SSTRING_VERSION 7

#include "CustomTypes.h"
#include <ctype.h>

typedef char* SString;//always an stb_ds array. or else.
//Note:SStrings are always null terminated AND are an stb_ds array meaning arrlenu(SString str)-1; can be used to get the length of chars. See SStringLen() and SStringLenu()
typedef SString* SString_arr;
void ssprintf(SString* sstr_ptr, char const *fmt, ...);
SString SStringNew(char* cString);
SString SStringNewCount(u64 len);
SString SStringSetLen(SString str, u64 len);
SString SStringAppend(SString str, char* cString);
SString SStringAppendC(SString str, char c);
SString SStringAppendS(SString str, char* cString, s64 cStrLen);
SString SStringSub(SString str, s64 start, s64 end);
SString SStringSubFree(SString str, s64 start, s64 end);
SString SStringPop(SString str);
s64 SStringLen(SString str);
u64 SStringLenu(SString str);
void SStringFree(SString str);
bool SStringEquals(SString str1, SString str2);
bool SStringEqualsC(SString str1, char* str2);
// i32 SStringCmp(SString str1, SString str2);
char SStringGetChar(SString str, s64 index);
SString_arr SStringSplit(SString str, char splitOnChar);
SString_arr SStringSplitMultipleSeparate(SString str, char* splitOnChars);
SString SStringRemoveChar(SString str, char c);
bool SSIsCharAny(char c, char* any);
bool SStringMatch(SString str1, s64 offset, char* str2);
void SStringFreeStringList(SString_arr list);
SString SStringToLower(SString str);
// SString SStringRemoveString(SString str, char* c);//TODO
u64 CountNewLinesTillByteLocation(SString text, u64 byteLocation);
SString GatherBalanced(SString text, s64 offset, char* openingString, char* closingString, bool isOffsetPastTheFirstOpeningString);
bool SStringContainsString(SString str, char* cStr);

#ifndef SSTRING_RAW_NAMES_ONLY
#define SSNew SStringNew
#define SSNewCount SStringNewCount
#define SSSetLen SStringSetLen
#define SSAppend SStringAppend
#define SSAppendC SStringAppendC
#define SSAppendS SStringAppendS
// #define SSAppendCount SStringAppendCount
#define SSSub SStringSub
#define SSSubFree SStringSubFree
#define SSPop SStringPop
#define SSLen SStringLen
#define SSLenu SStringLenu
#define SSFree SStringFree
#define SSEquals SStringEquals
#define SSEqualsC SStringEqualsC
// #define SS_Cmp SStringCmp
#define SSGetChar SStringGetChar
#define SSplit SStringSplit
#define SSplitMultiSep SStringSplitMultipleSeparate
#define SSRemoveChar SStringRemoveChar
#define SSMatch SStringMatch
#define SSFreeStringList SStringFreeStringList
#define SSToLower SStringToLower
#endif//SSTRING_RAW_NAMES_ONLY

// #define SSTRING_IMPLEMENTATION
#ifdef SSTRING_IMPLEMENTATION

char* SString_sprintf_callback(const char* buf, void* user_made_ptr, int len){
	SString* str = (SString*)user_made_ptr;
	(*str) = SSAppendS((void*)(*str), (void*)buf, len);
	return (void*)buf;
}

char SString_sprintf_buff[STB_SPRINTF_MIN] = {0};
void ssprintf(SString* sstr_ptr, char const *fmt, ...){
	#pragma omp critical
	{
		memset(SString_sprintf_buff, 0, STB_SPRINTF_MIN);
		va_list args;
		va_start(args, fmt);
		stbsp_vsprintfcb(&SString_sprintf_callback, sstr_ptr, SString_sprintf_buff, fmt, args);
		va_end(args);
	}
}

s64 SSMod(s64 a, s64 b){
    s64 r = a % b;
    return r < (s64)0 ? r + b : r;
}
SString SStringNewCount(u64 len){
	SString ret = NULL;
	arrsetlen(ret, len+1);
	memset(ret, 0, len+1);
	return ret;
}
SString SStringNew(char* cString){
	if(!cString){
		return SStringNewCount(0);
	}
	SString ret = NULL;
	u64 len = strlen(cString);
	arrsetlen(ret, len+1);
	memset(ret, 0, len+1);
	memmove(ret, cString, len);
	
	return ret;
}

//Note: This function does not memset but does still null terminate. If you need the entire string to start as all zeros use SStringNewCount instead.
SString SStringNewCountFast(u64 len){
	SString ret = NULL;
	arrsetlen(ret, len+1);
	ret[len] = 0;
	return ret;
}

SString SStringSetLen(SString str, u64 len){
	arrsetlen(str, len+1);
	// arrsetcap(str, len+1);
	memset(str, 0, len+1);
	return str;
}
SString SStringAppendC(SString str, char c){
	// if(c==0){return str;}
	s64 len = arrlen(str);
	str[len-1] = c;
	arrsetlen(str, len+1);
	str[arrlenu(str)-1] = 0;
	
	// str[len-1] = '\0';
	// arrput(str, '\0');
	return str;
}
SString SStringAppend(SString str, char* cString){
	for(u64 i = 0; i < strlen(cString); i++){
		str = SSAppendC(str, cString[i]);
	}
	return str;
}
SString SStringAppendS(SString str, char* cString, s64 cStrLen){
	for(s64 i = 0; i < cStrLen; i++){
		str = SSAppendC(str, cString[i]);
	}
	return str;
}
SString SStringSub(SString str, s64 start, s64 end){
	// if(SStringLen(str)==0){return str;}
	SString stringOut = NULL;
	
	//I'm sorry for these 2 lines. I really am. I'm just to stupid to know basic addition so I just changed numbers till it worked like Lua's substring function.
	start = (start < 0) ? (SStringLen(str)+(start+2)) : start;
	end = (end < 0) ? (SStringLen(str)+(end+2)) : end;
	
	start = (start < 0) ? 0 : start;
	end = (end < 0) ? 0 : end;
	end = (start!=end) ? end : end+1;
	s64 mi = (start < end) ? start : end;
	s64 ma = (start > end) ? start : end;
	s64 newSize = ma-mi;
	
	newSize = (newSize < 0) ? 0 : newSize;
	newSize = (newSize > SStringLen(str)) ? SStringLen(str) : newSize;
	
	arrsetlen(stringOut, newSize+1);
	arrsetcap(stringOut, newSize+1);
	memset(stringOut, 0, arrlenu(stringOut));
	memcpy(stringOut, &str[start], newSize);
	return stringOut;
}
SString SStringSubFree(SString str, s64 start, s64 end){
	SString ret = SStringSub(str, start, end);
	SStringFree(str);
	return ret;
}
SString SStringPop(SString str){
	arrpop(str);
	str[arrlenu(str)-1] = '\0';
	return str;
}
s64 SStringLen(SString str){
	return arrlen(str)-1;
}
u64 SStringLenu(SString str){
	return arrlenu(str)-1;
}
void SStringFree(SString str){
	if(str!=NULL){
		arrfree(str);
	}
}
bool SStringEquals(SString str1, SString str2){
	if(SSLenu(str1) != SSLenu(str2)){ return false; }
	for(u64 i = 0; i < SSLenu(str1); i++){
		if(str1[i] != str2[i]){return false;}
	}
	return true;
}
bool SStringEqualsC(SString str1, char* str2){
	
	if(SSLenu(str1) != strlen(str2)){ return false; }
	for(u64 i = 0; i < SSLenu(str1); i++){
		if(str1[i] != str2[i]){return false;}
	}
	return true;
}
// i32 SStringCmp(SString str1, SString str2){//not finished/todo
	
// 	if(arrlenu(str1) < arrlenu(str1)){return -1;}
// 	if(arrlenu(str1) != arrlenu(str2)){ return false; }
// 	for(u64 i = 0; i < arrlenu(str1); i++){
// 		if(str1[i] != str2[i]){return false;}
// 	}
// 	return true;
// 	return ;
// }
char SStringGetChar(SString str, s64 index){
	if(index < SStringLen(str) && index >= 0){
		return str[index];
	}else{
		return 0;
	}
}

SString_arr SStringSplit(SString str, char splitOnChar){
	SString_arr ret = NULL;
	arrsetlen(ret, 0);
	SString buildUp = SSNew("");
	for(u64 i = 0; i < SSLenu(str); i++){
		if(SSGetChar(str, i)==splitOnChar){
			arrput(ret, buildUp);
			buildUp = SSNew("");
			continue;
		}
		buildUp = SSAppendC(buildUp, SSGetChar(str, i));
	}
	arrput(ret, buildUp);
	
	return ret;
}

SString_arr SStringSplitMultipleSeparate(SString str, char* splitOnChars){
	SString_arr ret = NULL;
	arrsetlen(ret, 0);
	SString buildUp = SSNew("");
	for(u64 i = 0; i < SSLenu(str); i++){
		if(SSIsCharAny(SSGetChar(str, i), splitOnChars)){
			arrput(ret, buildUp);
			buildUp = SSNew("");
			continue;
		}
		buildUp = SSAppendC(buildUp, SSGetChar(str, i));
	}
	arrput(ret, buildUp);
	
	return ret;
}

SString SStringRemoveChar(SString str, char c){
	for(u64 i = SSLenu(str)-1; i < SSLenu(str); i--){
		if(SSGetChar(str, i) == c){
			arrdel(str, i);
		}
	}
	return str;
}
bool SSIsCharAny(char c, char* any){
	for(u64 i = 0; i < strlen(any); i++){
		if(c==any[i]){ return true; }
	}
	return false;
}
bool SViewMatchMin(char* str1, s64 len1, char* str2, s64 len2){
	s64 smallest = (len1 < len2) ? len1 : len2;
	for(s64 i = 0; i < smallest; i++){
		if(str1[i] != str2[i]){return false;}
	}
	return true;
}
// bool SMatchViewWithView(SString str1, i64 offset1, SString str2, i64 offset2){
	
// }
// bool SMatchViewWithString(SString str1, i64 offset1, SString str2){
	
// }
bool SStringMatch(SString text, s64 offset, char* matchStr){
	for(u64 i = 0; i < strlen(matchStr); i++){
		if(SSGetChar(text, offset+i) != matchStr[i]){return false;}
	}
	return true;
}

bool SStringMatchMin(SString str1, SString str2){
	s64 smallest = (SSLen(str1) < SSLen(str2)) ? SSLen(str1) : SSLen(str2);
	for(s64 i = 0; i < smallest; i++){
		if(SSGetChar(str1, i) != SSGetChar(str2, i)){return false;}
	}
	return true;
}

void SStringFreeStringList(SString_arr list){
	if(list){
		for(u64 i = 0; i < arrlenu(list); i++){
			SSFree(list[i]);
		}
		arrfree(list);
	}
}

SString SStringToLower(SString str){
	for(s64 i = 0; i < SSLen(str); i++){
		str[i] = tolower(str[i]);
	}
	return str;
}


u64 CountNewLinesTillByteLocation(SString text, u64 byteLocation){
	u64 lineCount = 1;
	for(u64 i=0; i<byteLocation; i++){
		if(SSGetChar(text, i)=='\n'){
			lineCount++;
		}
	}
	return lineCount;
}

SString GatherBalanced(SString text, s64 offset, char* openingString, char* closingString, bool isOffsetPastTheFirstOpeningString){
	SString ret = NULL;
	s64 c = offset;
	s64 parenStart = c;
	s64 parenEnd = 0;
	s64 parenCount = (s64)isOffsetPastTheFirstOpeningString;
	if(isOffsetPastTheFirstOpeningString==false && !SSMatch(text, offset, openingString)){return ret;}
	while(true){
		/*TODO: Fix (give or take) the below code not working.
		for(){
			//{
			Defer(print("TWO"))
			//}
			print("ONE")
		}
		The above code should print ONE then TWO but I think right now it literally just breaks and emits an erroneous squiggly bracket.
		*/ 
		if(SSMatch(text, c, openingString)){
			parenCount++;
		}
		if(SSMatch(text, c, closingString)){
			parenCount--;
		}
		if(c>SSLen(text)){
			printf("Warning! Failed to find balanced \"%s\" \"%s\" starting at byte location %llu line %llu \n", openingString, closingString, c, CountNewLinesTillByteLocation(text, parenStart));
			
			// for(i64 i=0; i<c; i++){ printf("%c", SSGetChar(text, i+offset)); }
			// printf("\n");
			return ret;
		}
		if(parenCount==0){
			parenEnd = c;
			break;
		}
		c++;
	}
	if(parenStart==parenEnd-1){
		return SSNew("");
	}
	ret = SSSub(text, parenStart+1, parenEnd);
	return ret;
}

bool SStringContainsString(SString str, char* cStr){
	for(s64 i = 0; i < SSLen(str); i++){
		if(SSMatch(str, i, cStr)){return true;}
	}
	return false;
}

#endif//STB_DS_IMPLEMENTATION
#endif//INCLUDE_SSTRING
