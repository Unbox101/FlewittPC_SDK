//Bootstrap nob
//cc main.c -o main -I includes
#include "LazyG.h"

#define DEFAULT_WORKSPACE "workspace/"

typedef bool(*swous_t)(SString_arr lines, u64 currLine, SString_arr args, u8_arr* output);
SString_arr funnie_comments = NULL;

/*
| Opcode | Byte | Operands | Cycles | Description                                                |
| :----- | :--: | :------: | :----: | ---------------------------------------------------------- |
| nop    |  0   |    -     |   ?    | No operation.                                              |
| add    |  1   |    -     |   ?    | Add registers a and b. Outputs to register a.              |
| sub    |  2   |    -     |   ?    | Sub registers a and b. Outputs to register a.              |
| load   |  3   |   byte   |   ?    | Load the next constant into register a.                    |
| loadj  |  4   |  short   |   ?    | Load the next 2 constants into jump-register.              |
| fetch  |  5   |    -     |   ?    | Load memory from jump-register address into register a.    |
| fetchj |  6   |    -     |   ?    | Load memory from jump-register address into jump-register. |
| write  |  7   |    -     |   ?    | Write memory from register a to jump-register address.     |
| copyab |  8   |    -     |   ?    | Copy register a to register b.                             |
| copyba |  9   |    -     |   ?    | Copy register b to register a.                             |
| jmp    |  10  |    -     |   ?    | Copies jump-register address into program_counter.         |
| jz     |  11  |    -     |   ?    | "jmp" if register a is zero.                               |
| jc     |  12  |    -     |   ?    | "jmp" if the register a + register b > 255.                |
| writed |  13  |   byte   |   ?    | Write the next constant into the LCD display.              |
| readd  |  14  |    -     |   ?    | Copy LCD display output into register a.                   |
| halt   | 255  |    -     |   ?    | Stops the computer.                                        |
*/

void Stringify_charptr_to_u64(char* str, u64 charPtrLen, u64* valOut, bool* errOut){
	bool _errOut = false;
	u64 _valOut = 0;
	(errOut) = (errOut) ? (errOut) : (&_errOut);
	(valOut) = (valOut) ? (valOut) : (&_valOut);
	char* endptr;
	u64 val = strtoull(str, &endptr, 0);
	u64 diff = (u64)endptr - (u64)str;
	(*valOut) = (charPtrLen != diff) ? (*valOut) : val;
	(*errOut) = (charPtrLen != diff) ? true : false;
}

void Stringify_str_to_u64(SString str, u64* valOut, bool* errOut){
	Stringify_charptr_to_u64(str, SSLenu(str), valOut, errOut);
}

#define assertArgCount(desiredNum) if((arrlenu(args)-1) < desiredNum){ print("Line:%llu Error! Invalid arg count", currLine+1) return false; }
#define DeclareSimpleOpcode(opName, argCount, byteOut)\
const u8 INS_##opName = byteOut;\
bool swous_##opName(SString_arr lines, u64 currLine, SString_arr args, u8_arr* output){\
	assertArgCount(argCount)\
	arrput(funnie_comments, #opName);\
	arrput((*output), byteOut);\
	return true;\
}
DeclareSimpleOpcode(nop,					0, 0)
DeclareSimpleOpcode(add,					0, 1)
DeclareSimpleOpcode(sub,					0, 2)
// DeclareSimpleOpcode(load,					0, 3)//these are not simple. see below for the function definition.
// DeclareSimpleOpcode(loadj,					0, 4)//these are not simple. see below for the function definition.
DeclareSimpleOpcode(fetch,					0, 5)
DeclareSimpleOpcode(fetchj,					0, 6)
DeclareSimpleOpcode(write,					0, 7)
DeclareSimpleOpcode(copyab,					0, 8)
DeclareSimpleOpcode(copyba,					0, 9)
DeclareSimpleOpcode(jmp,					0, 10)
DeclareSimpleOpcode(jz,						0, 11)
DeclareSimpleOpcode(jc,						0, 12)
// DeclareSimpleOpcode(_debug_print_mem,		0, 253)
DeclareSimpleOpcode(_debug_halt,			0, 254)
DeclareSimpleOpcode(halt,					0, 255)


const u8 INS_load = 3;
bool swous_load(SString_arr lines, u64 currLine, SString_arr args, u8_arr* output){
	assertArgCount(1)
	u64 val; bool err = false;
	Stringify_str_to_u64(args[1], &val, &err);
	if(err){
		print("Error! Failed to turn \"%s\" into a valid number!", args[1])
		return false;
	}
	arrput((*output), INS_load); arrput(funnie_comments, "load");
	arrput((*output), val); arrput(funnie_comments, "");
	return true;
}

const u8 INS_loadj = 4;
bool swous_loadj(SString_arr lines, u64 currLine, SString_arr args, u8_arr* output){
	assertArgCount(1)
	u64 val; bool err = false;
	Stringify_str_to_u64(args[1], &val, &err);
	if(err){
		print("Error! Failed to turn \"%s\" into a valid number!", args[1])
		return false;
	}
	arrput((*output), INS_loadj); arrput(funnie_comments, "loadj");
	u8* addr2 = (u8*)&val;
	arrput((*output), addr2[0]); arrput(funnie_comments, "");
	arrput((*output), addr2[1]); arrput(funnie_comments, "");
	return true;
}

const u8 INS__debug_print_mem = 253;
bool swous__debug_print_mem(SString_arr lines, u64 currLine, SString_arr args, u8_arr* output){
	assertArgCount(1)
	u64 val; bool err = false;
	Stringify_str_to_u64(args[1], &val, &err);
	if(err){
		print("Error! Failed to turn \"%s\" into a valid number!", args[1])
		return false;
	}
	arrput((*output), INS__debug_print_mem); arrput(funnie_comments, "_debug_print_mem");
	u8* addr2 = (u8*)&val;
	arrput((*output), addr2[0]); arrput(funnie_comments, "");
	arrput((*output), addr2[1]); arrput(funnie_comments, "");
	return true;
}

typedef struct dict_opname_opsettings_t{
	char* key;
	swous_t value;
}dict_opname_opsettings_t;

void f_compile(void);
void f_assemble(void);
void f_emulate(void);

int main(int argc, char **argv){
	mkdir(DEFAULT_WORKSPACE);
	bool compile = false;
	bool emulate = false;
	bool assemble = false;
	
	SString_arr s_args = NULL;
	for(u64 i = 0; i < argc; i++){
		arrput(s_args, SSNew(argv[i]));
	}
	if(arrlenu(s_args)==1){
		arrput(s_args, SSNew("-h"));
	}
	for(u64 i = 0; i < arrlenu(s_args); i++){
		// print("argv[%llu] = %s", (u64)i, argv[i])
		if(s_args[i][0] == '-'){
			if(
				strcmp(s_args[i], "-help") == 0 ||
				strcmp(s_args[i], "-h") == 0 ||
				strcmp(s_args[i], "--help") == 0 ||
				strcmp(s_args[i], "--h") == 0
			){
				print("Running with \"-help\" arg.")
				print("Available program arguments:")
				print("  [-compile, -co]        \"Runs the compiler on \""DEFAULT_WORKSPACE"input.c\". Outputs to \""DEFAULT_WORKSPACE"input.asm\". WARNING: This OVERRIDES the \""DEFAULT_WORKSPACE"input.asm\" file!")
				print("  [-assemble, -as]       \"Runs the assembler on \""DEFAULT_WORKSPACE"input.asm\". Outputs to \""DEFAULT_WORKSPACE"output.bin\" and \""DEFAULT_WORKSPACE"output.debug\".")
				print("  [-emulate, -em]        \"Runs the emulator on \""DEFAULT_WORKSPACE"output.bin\".")
				print("  [-all, -a]             \"Runs the compiler, assembler, and emulator.")
			}else if(strcmp(s_args[i], "-compile") == 0 || strcmp(s_args[i], "-co") == 0){
				print("Running with \"-compile\" arg.")
				compile = true;
			}else if(strcmp(s_args[i], "-assemble") == 0 || strcmp(s_args[i], "-as") == 0){
				print("Running with \"-assemble\" arg.")
				assemble = true;
			}else if(strcmp(s_args[i], "-emulate") == 0 || strcmp(s_args[i], "-em") == 0){
				print("Running with \"-emulate\" arg.")
				emulate = true;
			}else if(strcmp(s_args[i], "-all") == 0 || strcmp(s_args[i], "-a") == 0){
				print("Running with \"-all\" arg.")
				compile = true;
				emulate = true;
				assemble = true;
			}else{
				print("Unkown arg: \"%s\"", s_args[i])
				exit(EXIT_FAILURE);
			}
		}
	}
	
	
	
	if(compile){
		f_compile();
	}
	
	if(assemble){
		f_assemble();
	}
	
	if(emulate){
		f_emulate();
	}
	
	return 0;
}

//MARK: compile
void f_compile(void){
	FILE* f = fopen(DEFAULT_WORKSPACE"input.c", "rb");
	if(!f){
		print("Error: Missing file "DEFAULT_WORKSPACE"input.c")
		exit(1);
	}
	fseek(f, 0, SEEK_END);
	long fsize = ftell(f);
	fseek(f, 0, SEEK_SET);
	SString text = SSNewCount(fsize);
	fread(text, SSLenu(text), 1, f);
	fclose(f);
	
	//Construct preprocessor macro steps/phases
	sh_token_func_t** grouped_mfuncs = NULL;
		
		sh_token_func_t* mfuncs3 = NULL;
			sh_new_strdup(mfuncs3);
			shput(mfuncs3, "goto", &mFunc_goto);
			shput(mfuncs3, "floop", &mFunc_floop);
			shput(mfuncs3, "this token is ignored 1", &mFunc_NormalTextCollector);
		arrput(grouped_mfuncs, mfuncs3);
		
		sh_token_func_t* mfuncs1 = NULL;
			sh_new_strdup(mfuncs1);
			shput(mfuncs1, "define", &mFunc_define);
			shput(mfuncs1, "this token is ignored 0", &mFunc_NormalTextCollector);
		arrput(grouped_mfuncs, mfuncs1);
		
		sh_token_func_t* mfuncs2 = NULL;
			sh_new_strdup(mfuncs2);
			shput(mfuncs2, "this token is ignored 0", &mFunc_broadcast_defines);
			shput(mfuncs2, "this token is ignored 1", &mFunc_NormalTextCollector);
		arrput(grouped_mfuncs, mfuncs2);
		
		//Label finding and broadcasting must be done last (it's kinda like linking) everything else can happen before it.
		sh_token_func_t* mfuncs_secondlast = NULL;
			sh_new_strdup(mfuncs_secondlast);
			shput(mfuncs_secondlast, "ins", &mFunc_ins);
			shput(mfuncs_secondlast, "label", &mFunc_label);
			shput(mfuncs_secondlast, "this token is ignored 0", &mFunc_NormalTextCollector);
		arrput(grouped_mfuncs, mfuncs_secondlast);
		
		sh_token_func_t* mfuncs_last = NULL;
			sh_new_strdup(mfuncs_last);
			shput(mfuncs_last, "this token is ignored 0", &mFunc_broadcast_labels);
			shput(mfuncs_last, "this token is ignored 1", &mFunc_NormalTextCollector);
		arrput(grouped_mfuncs, mfuncs_last);
	
	//run the preprocessor
	SString newText = SSNew(text);
	for(u64 i = 0; i < arrlenu(grouped_mfuncs); i++){
		sh_token_func_t* listception = grouped_mfuncs[i];
		
		SString freeThis = newText;
		newText = PreprocessTextUsingTransformers(0, newText, listception);
		print("%s\n-----------------------------------------------------------", freeThis)
		SSFree(freeThis);
	}
	
	SString textOut = SSNew("");
	
	for(u64 i = 0; i < SSLenu(newText); i++){
		char c = SSGetChar(newText, i);
		if(c=='\t'){continue;}
		textOut = SSAppendC(textOut, c);
	}
	
	FILE* output = fopen(DEFAULT_WORKSPACE"input.asm", "wb");
	fwrite(textOut, SSLenu(textOut), 1, output);
	fflush(output);
	fclose(output);
	
	print("%s", textOut)
}
//MARK: assemble
void f_assemble(void){
	dict_opname_opsettings_t* op_codes = NULL;
	sh_new_strdup(op_codes);
	#define imlazy(opname) shput(op_codes, #opname, &swous_##opname);
	imlazy(nop)
	imlazy(add)
	imlazy(sub)
	imlazy(load)
	imlazy(loadj)
	imlazy(fetch)
	imlazy(fetchj)
	imlazy(write)
	imlazy(copyab)
	imlazy(copyba)
	imlazy(jmp)
	imlazy(jz)
	imlazy(jc)
	imlazy(_debug_print_mem)
	imlazy(_debug_halt)
	imlazy(halt)
	#undef imlazy
	
	FILE* f = fopen(DEFAULT_WORKSPACE"input.asm", "rb");
	if(!f){
		print("Error: Missing file "DEFAULT_WORKSPACE"input.asm")
		exit(1);
	}
	fseek(f, 0, SEEK_END);
	long fsize = ftell(f);
	fseek(f, 0, SEEK_SET);
	
	SString ftext = SSNewCount(fsize);
	fread(ftext, SSLenu(ftext), 1, f);
	fclose(f);
	for(u64 i = 0; i < SSLenu(ftext); i++){
		if(SSGetChar(ftext, i)=='\r'){arrdel(ftext, i);}
	}
	SString_arr lines = SSplit(ftext, '\n');
	
	u8_arr output = NULL;
	for(u64 i = 0; i < arrlenu(lines); i++){
		if(SSGetChar(lines[i], 0) =='/' && SSGetChar(lines[i], 1) =='/'){ continue; }//ignore comments
		SString_arr args = SSplit(lines[i], ' ');
		if(arrlenu(args)<=0){ goto skip; }
		swous_t op = shget(op_codes, args[0]);
		if(!op){ print("Line:%llu Error! Invalid op code name \"%s\"! Skipping...", i+1, args[0]) goto skip; }
		bool succ = (op)(lines, i, args, &output);
		if(succ){ print("Valid op \"%s\"", args[0]) }
		skip:
		SSFreeStringList(args);
	}
	SSFreeStringList(lines);
	FILE* doutfile = fopen(DEFAULT_WORKSPACE"output.debug", "wb");
	if(!doutfile){print("Error! Failed to open file \""DEFAULT_WORKSPACE"output.bin\" for writing!") exit(1);}
	for(u64 i = 0; i < arrlenu(output); i++){
		fprintf(doutfile, "%d\n", output[i]);
	}
	fprintf(doutfile, "\n");
	fprintf(doutfile, "char code[] = {\n");
	for(u64 i = 0; i < arrlenu(output); i++){
		fprintf(doutfile, "     %d, //%s\n", output[i], funnie_comments[i]);
	}
	fprintf(doutfile, "};\n");
	fclose(doutfile);
	FILE* outfile = fopen(DEFAULT_WORKSPACE"output.bin", "wb");
	for(u64 i = 0; i < arrlenu(output); i++){
		fprintf(outfile, "%c", output[i]);
	}
	fclose(outfile);
}
//MARK: emulate
void f_emulate(void){
	u8 reg_a;
	u8 reg_b;
	u8 reg_j[2];
	u64 pc = 0;
	u8 ins = 0;
	u64 write_ret_tracker = 0;
	FILE* mem = fopen(DEFAULT_WORKSPACE"output.bin", "r+");
	// FILE* f = fopen("output.bin", "r+");
	if(!mem){print("Error! Failed to open file \""DEFAULT_WORKSPACE"output.bin\" for reading and emulating!") exit(1);}
	
	#define readmem(offset) \
		fseek(mem, offset, SEEK_SET); \
		ins = fgetc(mem);
	
	#define writemem(byte, offset) \
		fseek(mem, offset, SEEK_SET); \
		fputc(byte, mem);
	
	#define readnext() readmem(pc) pc++;
	bool debugging = true;
	print("Emulating...")
	while(true){
		char stoob[512] = {0};
		readmem(pc);pc++;
		// readnext();//get next instruction as "ins"
		// print("instruction = %llu", (u64)ins)
		
		#define lazy_next_instruction(instruction_name, ...) if(ins==instruction_name){if(debugging){print("instruction[%s]", #instruction_name)}__VA_ARGS__}
		
		lazy_next_instruction(INS_nop, 
			
		)else
		lazy_next_instruction(INS_add, 
			reg_a = reg_a + reg_b;
			print("INS_add result = %llu", (u64)reg_a)
		)else
		lazy_next_instruction(INS_sub, 
			reg_a = reg_a - reg_b;
			print("INS_sub result = %llu", (u64)reg_a)
		)else
		lazy_next_instruction(INS_load, 
			readnext();
			reg_a = ins;
			print("INS_load result = %llu", (u64)reg_a)
		)else
		lazy_next_instruction(INS_loadj, 
			readnext();
			reg_j[0] = ins;
			readnext();
			reg_j[1] = ins;
			print("INS_loadj result = %llu", (u64)bitcast(u16,reg_j))
		)else
		lazy_next_instruction(INS_fetch, 
			u16 addr = bitcast(u16, reg_j);
			readmem(addr)
			reg_a = ins;
			print("INS_fetch addr = %llu", (u64)addr)
			print("INS_fetch result = %llu", (u64)reg_a)
		)else
		lazy_next_instruction(INS_fetchj, 
			u16 addr = bitcast(u16, reg_j);
			readmem(addr)
			reg_j[0] = ins;
			readmem(addr)
			reg_j[1] = ins;
			print("INS_fetchj addr = %llu", (u64)addr)
			print("INS_fetchj result = %llu", (u64)bitcast(u16, reg_j))
		)else
		lazy_next_instruction(INS_write, 
			u16 addr = bitcast(u16, reg_j);
			writemem(reg_a, addr)
			print("INS_fetchj addr = %llu", (u64)bitcast(u16, addr))
		)else
		lazy_next_instruction(INS_copyab, 
			reg_b = reg_a;
			print("INS_copyab result = %llu", (u64)reg_b)
		)else
		lazy_next_instruction(INS_copyba, 
			reg_a = reg_b;
			print("INS_copyba result = %llu", (u64)reg_a)
		)else
		lazy_next_instruction(INS_jmp, 
			pc = bitcast(u16, reg_j);
			print("INS_jmp result = %llu", (u64)pc)
		)else
		lazy_next_instruction(INS_jz, 
			pc = (reg_a==0) ? bitcast(u16, reg_j) : pc;
			print("INS_jz result = %llu", (u64)pc)
		)else
		lazy_next_instruction(INS_jc, 
			pc = ((u64)reg_a+(u64)reg_b > 255) ? bitcast(u16, reg_j) : pc;
			print("INS_jc result = %llu", (u64)pc)
		)else
		lazy_next_instruction(INS__debug_print_mem,
			readnext();
			reg_j[0] = ins;
			readnext();
			reg_j[1] = ins;
			readmem(bitcast(u16, reg_j));
			print("------------------ debug halt ------------------")
			print("_debug_print:%llu", (u64)ins);
			printf("Press any key to continue running program.");
			scanf("%c", &stoob);
		)else
		lazy_next_instruction(INS__debug_halt,
			print("------------------ debug halt ------------------")
			printf("Press any key to continue running program.");
			scanf("%c", &stoob);
		)else
		lazy_next_instruction(INS_halt, 
			break;
		)
		// print("pc=%llu", (u64)pc)
		// scanf("%c", &stoob);
	}
	fclose(mem);
}