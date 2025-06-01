#ifndef INCLUDE_Preprocessor_API_H
#define INCLUDE_Preprocessor_API_H

#include "LazyG.h"

#define Entity u64

typedef bool(*custom_macro_func_t)(Entity optionalEntityToModify, SString* output, s64* charId, SString input, char* token);
typedef struct sh_token_func_t{
	char* key;
	custom_macro_func_t value;
}sh_token_func_t;//string hashmap-able token func


bool IsCharAny(char c, char* any){
	for(u64 i = 0; i < strlen(any); i++){
		if(c==any[i]){ return true; }
	}
	return false;
}
bool IsCharWhitespace(char c){
	// return isspace(c);
	return (c == ' ' || c == '\t' || c == '\n' || c == '\0' || c == '\r' || c == '\v' || c == '\f');
}

bool IsCharAlphaNumericOrUnderscore(char c){
	return (c>=48 && c<=57) || (c>=65 && c<=90) || (c>=97 && c<=122) || c==95;
}

bool charBackslashValid(SString input, s64 offset){
	bool valid = true;
	s64 backUpTerry = 1;
	while(SSGetChar(input, offset-backUpTerry)=='\\'){
		valid = !valid;
		backUpTerry++;
	}
	return valid;
}
//MARK: PreprocessText
SString PreprocessTextUsingTransformers(Entity optionalEntityToModify, SString inputStr, sh_token_func_t* textTransformersStrMap){
	
	sh_token_func_t* tokenFuncs = textTransformersStrMap;
	
	SString input = SSNew(inputStr);
	s64 charId = 0;
	bool didSomething = true;
	// print("inputStr = \n%s", inputStr)
	// print("input = \n%s", input)
	while(didSomething){//loop until did nothing
		charId = 0;
		SString output = SSNew("");
		didSomething = false;
		while(true){//loop over charId until end of input string
			s64 tokId = 0;
			for(tokId = 0; tokId < shlen(tokenFuncs); tokId++){
				// print("output%p",output)
				if((tokenFuncs[tokId].value)(optionalEntityToModify, &output, &charId, input, tokenFuncs[tokId].key)){
					didSomething = true;
					break;
				}
				// print("output%s",output)
			}
			if(charId >= SSLen(input)){
				break;
			}
		}
		if(didSomething == false){
			break;
		}
		SString temp = input;
		input = output;
		SSFree(temp);
	}
	
	return input;
}

bool mFunc_IgnoreCommentsAndQuotes(Entity optionalEntityToModify, SString* output, s64* charId, SString input, char* token){
	//quotes with proper backslashing
	if(SSGetChar(input, (*charId)) == '\"' && charBackslashValid(input, (*charId))){
		SString gather = SSNew("");
		(*charId)++;
		s64 startChar = (*charId);
		while(true){
			if(SSGetChar(input, (*charId)) == '\"' && charBackslashValid(input, (*charId))){
				(*charId)++;
				break;
			}
			if((*charId) >= SSLen(input)){
				SSSetLen(gather, 0);
				(*charId) = startChar;
				break;
			}
			gather = SSAppendC(gather, SSGetChar(input, (*charId)));
			(*charId)++;
			
		}
		ssprintf(output, "\"%s\"", gather);
		SSFree(gather);
		return false;
	}
	//single line comment
	if(SSGetChar(input, (*charId)) == '/' && SSGetChar(input, (*charId)+1) == '/'){
		SString gather = SSNew("");
		s64 startChar = (*charId);
		while(true){
			if(SSGetChar(input, (*charId)) == '\n'){
				break;
			}
			if((*charId) >= SSLen(input)){
				SSSetLen(gather, 0);
				(*charId) = startChar;
				break;
			}
			gather = SSAppendC(gather, SSGetChar(input, (*charId)));
			(*charId)++;
		}
		ssprintf(output, "%s", gather);
		SSFree(gather);
		return false;
	}
	//multi line comment
	if(SSGetChar(input, (*charId)) == '/' && SSGetChar(input, (*charId)+1) == '*'){
		SString gather = SSNew("");
		s64 startChar = (*charId);
		while(true){
			if(SSGetChar(input, (*charId)+1) == '/' && SSGetChar(input, (*charId)) == '*'){
				break;
			}
			if((*charId) >= SSLen(input)){
				SSSetLen(gather, 0);
				(*charId) = startChar;
				break;
			}
			gather = SSAppendC(gather, SSGetChar(input, (*charId)));
			(*charId)++;
		}
		ssprintf(output, "%s", gather);
		SSFree(gather);
		return false;
	}
	return true;
}

bool mFunc_NormalTextCollector(Entity optionalEntityToModify, SString* output, s64* charId, SString input, char* token){
	
	if(mFunc_IgnoreCommentsAndQuotes(optionalEntityToModify, output, charId, input, token)){
		char c = SSGetChar(input, (*charId));
		if(!c){return false;}
		ssprintf(output, "%c", c);
		(*charId)+=1;
	}
	
	return false;
}

//CUSTOM MACROS feel free to make your own!
bool mFunc_Template(Entity optionalEntityToModify, SString* output, s64* charId, SString input, char* token){
	if(
		!IsCharAlphaNumericOrUnderscore(SSGetChar(input, (*charId)-1)) &&
		SSMatch(input, (*charId), token) &&
		!IsCharAlphaNumericOrUnderscore(SSGetChar(input, (*charId)+strlen(token)))
	){
		(*charId)+=strlen(token);
		SString gather = GatherBalanced(input, (*charId), "(", ")", false);
		if(!gather){return true;}
		(*charId)+=SSLen(gather)+strlen("()");
		
		//do stuff here
		
		SSFree(gather);
		
		//and/or do stuff here
		
		return true;
	}
	return false;
}






#endif //INCLUDE_Preprocessor_API_H