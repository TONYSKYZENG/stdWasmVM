#include "wasmvm.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <limits.h>
#include <math.h>


// Call table/trapping table lookups/execution
#include <signal.h>

#include "util.h"
#include "platform.h"
#include "wa.h"
#include "thunk.h"
#include "system_api.h"

Module *vm_create(char *code, int len){
    uint8_t  *bytes = NULL;
    int       byte_count;

    bytes = (uint8_t *) code;
    byte_count = len;
	
    Options opts;
    Module *m = load_module(bytes, byte_count, opts);
    m->path = "";
   // init_thunk_in(m);
    return m;
}


int set_Pi32(int32_t num, Module *m){
    m->stack[++m->sp].value_type = I32;
    m->stack[m->sp].value.uint32 = num; 
    return 0;

}

int set_Pi64(int64_t num, Module *m){
    m->stack[++m->sp].value_type = I64;
    m->stack[m->sp].value.uint32 = num; 
    return 0;

}

int set_Pf32(float num, Module *m){
    m->stack[++m->sp].value_type = F32;
    m->stack[m->sp].value.uint32 = num; 
    return 0;   

}

int set_Pf64(double num, Module *m){
    m->stack[++m->sp].value_type = F64;
    m->stack[m->sp].value.uint32 = num; 
    return 0;    

}


int vm_exec(Module *m, char *func){
    int fidx = 0, res = 0;
    fidx = get_export_fidx(m, func);
    if (fidx == -1) {
	FATAL("no exported function %s\n", func);
    }
    res = invoke(m, fidx);

    if (!res) {
        error("Exception: %s\n", exception);
        return 1;
    }
    return 0;
}
int vm_fuckJump(Module *m, char *func)
{	printf("this is a fucked-jump\r\n");
	int fidx = 0, res = 0;
	   fidx = get_export_fidx(m, func);
	   if (fidx == -1) {
	   FATAL("no exported function %s\n", func);
	   }
	   if (TRACE && DEBUG) { dump_stacks(m); }

         setup_call(m, fidx);
	   return 0;



}

int32_t get_Ri32(Module *m){
    if (m->sp >= 0) {
    StackValue *result = &m->stack[m->sp--];
    if(result->value_type == I32) {
	return  result->value.uint32;
	}
    }
    return 0;
}

int64_t get_Ri64(Module *m){
    if (m->sp >= 0) {
    StackValue *result = &m->stack[m->sp--];
    if(result->value_type == I64) {
	return result->value.uint64;
    }
    }
    return 0;
}

float get_Rf32(Module *m){
    if (m->sp >= 0) {
    StackValue *result = &m->stack[m->sp--];
    if(result->value_type == F32) {
	return result->value.f32;
    }
    }
    return 0;

}	

double get_Rf64(Module *m){
    if (m->sp >= 0) {
    StackValue *result = &m->stack[m->sp--];
    if(result->value_type == F64) {
	return result->value.f64;
    }
    }
    return 0;

}

   // sleep(3);
 //   strcpy(msg,"Bye!
