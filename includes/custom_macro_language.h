#include "LazyG.h"
//MARK: custom macros

#define define(...)
#define ins(...)
#define label(...)
#define goto(...)
#define floop(...)

//MARK: define
SString_arr defines = NULL;
SString_arr define_args = NULL;
bool mFunc_define(Entity optionalEntityToModify, SString* output, s64* charId, SString input, char* token){
	if(
		!IsCharAlphaNumericOrUnderscore(SSGetChar(input, (*charId)-1)) &&
		SSMatch(input, (*charId), token) &&
		!IsCharAlphaNumericOrUnderscore(SSGetChar(input, (*charId)+strlen(token)))
	){
		(*charId)+=strlen(token);
		SString gather = GatherBalanced(input, (*charId), "(", ")", false);
		if(!gather){return true;}
		(*charId)+=SSLen(gather)+strlen("()");
			gather = SSRemoveChar(gather, ' ');
			gather = SSRemoveChar(gather, '\t');
			SString_arr split = SSplit(gather, ',');
			if(arrlenu(split)==2){
				arrput(defines, SSNew(split[0]));
				arrput(define_args, SSNew(split[1]));
			}
			SSFreeStringList(split);
			SSFree(gather);
		return true;
	}
	return false;
}

//MARK: broadcast_defines
bool mFunc_broadcast_defines(Entity optionalEntityToModify, SString* output, s64* charId, SString input, char* token){
	for(u64 i = 0; i < arrlenu(defines); i++){
		if(
			!IsCharAlphaNumericOrUnderscore(SSGetChar(input, (*charId)-1)) &&
			SSMatch(input, (*charId), defines[i]) &&
			!IsCharAlphaNumericOrUnderscore(SSGetChar(input, (*charId)+strlen(defines[i])))
		){
			(*charId)+=strlen(defines[i]);
			ssprintf(output, "%s", define_args[i]);
			// SString gather = GatherBalanced(input, (*charId), "(", ")", false);
			// if(!gather){return true;}
			// (*charId)+=SSLen(gather)+strlen("()");
			
			// //do stuff here
			
			// SSFree(gather);
			
			//and/or do stuff here
			
			return true;
		}
	}
	return false;
}

//instruction lookup tables? no. I just used if statements instead. see mFunc_ins() below.

u64 ins_counter = 0;

//MARK: ins
bool mFunc_ins(Entity optionalEntityToModify, SString* output, s64* charId, SString input, char* token){
	if(
		!IsCharAlphaNumericOrUnderscore(SSGetChar(input, (*charId)-1)) &&
		SSMatch(input, (*charId), token) &&
		!IsCharAlphaNumericOrUnderscore(SSGetChar(input, (*charId)+strlen(token)))
	){
		(*charId)+=strlen(token);
		SString gather = GatherBalanced(input, (*charId), "(", ")", false);
		if(!gather){return true;}
		(*charId)+=SSLen(gather)+strlen("()");
		
		gather = SSRemoveChar(gather, ' ');
		gather = SSRemoveChar(gather, '\t');
		SString_arr split = SSplit(gather, ',');
		if(arrlenu(split)>0){
			ins_counter++;
			if(SSEqualsC(split[0], "load")){
				ins_counter+=1;
			}else if(SSEqualsC(split[0], "loadj")){
				ins_counter+=2;
			}else if(SSEqualsC(split[0], "_debug_print_mem")){
				ins_counter+=2;
			}
		}
		
		for(u64 i = 0; i < arrlenu(split); i++){
			ssprintf(output, "%s", split[i]);
			if(i!=arrlenu(split)-1){
				ssprintf(output, " ");
			}
		}
		ssprintf(output, " ");
		SSFreeStringList(split);
		SSFree(gather);
		return true;
	}
	return false;
}

//MARK: goto
bool mFunc_goto(Entity optionalEntityToModify, SString* output, s64* charId, SString input, char* token){
	if(
		!IsCharAlphaNumericOrUnderscore(SSGetChar(input, (*charId)-1)) &&
		SSMatch(input, (*charId), token) &&
		!IsCharAlphaNumericOrUnderscore(SSGetChar(input, (*charId)+strlen(token)))
	){
		(*charId)+=strlen(token);
		SString gather = GatherBalanced(input, (*charId), "(", ")", false);
		if(!gather){return true;}
		(*charId)+=SSLen(gather)+strlen("()");
		
		gather = SSRemoveChar(gather, ' ');
		gather = SSRemoveChar(gather, '\t');
		ssprintf(output, "ins(loadj,%s)\nins(jmp)\n", gather);
		
		SSFree(gather);
		return true;
	}
	return false;
}

char* floop_code_begin = "\n\
define(i_addr,256)\n\
define(n,%s)//u64 iter_amount\n\
//if(n==0){goto(&end)}\n\
	ins(loadj,end)\n\
	ins(load,n)\n\
	ins(jz)\n\
	\n\
//copy n into &i\n\
	ins(load,n)\n\
	ins(loadj,i_addr)\n\
	ins(write)\n\
	\n\
//begin for loop using &i and n\n\
	label(start)\n\
	\n\
	\n\
	\n";

char* floop_code_end = "\n\
// ins(_debug_print_mem,&i)\n\
		ins(load,1) //load 1 into reg_a\n\
		ins(copyab) //copy reg_a to reg_b\n\
		//reg_a == 1 && reg_b == 1\n\
	//load memory at &i into reg_a\n\
		ins(loadj,i_addr)\n\
		ins(fetch)\n\
		//reg_a == i && reg_b == 1\n\
	//subtract\n\
		ins(sub)\n\
		//reg_a == i-1 && reg_b == 1 && (&i) == i && reg_j == (&i)\n\
	//write reg_a (i) into &i\n\
		ins(loadj,i_addr)\n\
		ins(write)\n\
	//if(reg_a==0){ goto &end; }else{ goto &start; }\n\
		ins(loadj,end)\n\
		ins(jz)\n\
		ins(loadj,start)\n\
		ins(jmp)\n\
	label(end)\n";

//MARK: floop
bool mFunc_floop(Entity optionalEntityToModify, SString* output, s64* charId, SString input, char* token){
	if(
		!IsCharAlphaNumericOrUnderscore(SSGetChar(input, (*charId)-1)) &&
		SSMatch(input, (*charId), token) &&
		!IsCharAlphaNumericOrUnderscore(SSGetChar(input, (*charId)+strlen(token)))
	){
		(*charId)+=strlen(token);
		SString gather = GatherBalanced(input, (*charId), "(", ")", false);
		if(!gather){return true;}
		(*charId)+=SSLen(gather)+strlen("()");
		
		gather = SSRemoveChar(gather, ' ');
		gather = SSRemoveChar(gather, '\t');
			SString_arr split = SSplit(gather, ',');
			print("%llu", arrlenu(split))
			if(arrlenu(split)>1){
				ssprintf(output, floop_code_begin, split[0], split[1]);
				for(u64 i = 1; i < arrlenu(split); i++){
					ssprintf(output, "%s", split[i]);
					if(i!=arrlenu(split)-1){
						ssprintf(output, ",");
					}
				}
				ssprintf(output, "%s", floop_code_end);
			}
			
			SSFreeStringList(split);
		
		
		SSFree(gather);
		return true;
	}
	return false;
}


SString_arr labels = NULL;
u64_arr label_locs = NULL;

//MARK: label
bool mFunc_label(Entity optionalEntityToModify, SString* output, s64* charId, SString input, char* token){
	if(
		!IsCharAlphaNumericOrUnderscore(SSGetChar(input, (*charId)-1)) &&
		SSMatch(input, (*charId), token) &&
		!IsCharAlphaNumericOrUnderscore(SSGetChar(input, (*charId)+strlen(token)))
	){
		(*charId)+=strlen(token);
		SString gather = GatherBalanced(input, (*charId), "(", ")", false);
		if(!gather){return true;}
		(*charId)+=SSLen(gather)+strlen("()");
		
		gather = SSRemoveChar(gather, ' ');
		gather = SSRemoveChar(gather, '\t');
		arrput(labels, SSNew(gather));
		arrput(label_locs, ins_counter);
		
		SSFree(gather);
		return true;
	}
	return false;
}

//MARK: broadcast_labels
bool mFunc_broadcast_labels(Entity optionalEntityToModify, SString* output, s64* charId, SString input, char* token){
	for(u64 i = 0; i < arrlenu(labels); i++){
		if(
			!IsCharAlphaNumericOrUnderscore(SSGetChar(input, (*charId)-1)) &&
			SSMatch(input, (*charId), labels[i]) &&
			!IsCharAlphaNumericOrUnderscore(SSGetChar(input, (*charId)+strlen(labels[i])))
		){
			(*charId)+=strlen(labels[i]);
			ssprintf(output, "%llu", label_locs[i]);
			// SString gather = GatherBalanced(input, (*charId), "(", ")", false);
			// if(!gather){return true;}
			// (*charId)+=SSLen(gather)+strlen("()");
			
			// //do stuff here
			
			// SSFree(gather);
			
			//and/or do stuff here
			
			return true;
		}
	}
	return false;
}