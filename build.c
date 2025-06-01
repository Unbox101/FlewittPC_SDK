//cc build.c -o build

#define NOB_IMPLEMENTATION
#include "nob.h"

int main(int argc, char **argv){
	NOB_GO_REBUILD_URSELF(argc, argv);
	Nob_Cmd cmd = {0};
	nob_cmd_append(&cmd, "cc", "main.c", "-o", "main", "-g", "-I", "includes");
	if (!nob_cmd_run_sync_and_reset(&cmd)) return 1;
	return 0;
}

