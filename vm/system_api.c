#include <stdlib.h>
#include <string.h>
#include "wasmvm.h"
#define apis 20
struct api_func{
    char *name;
    void *addr;
};
extern VTIMERCTL g_vtimerCtl;

struct api_func table[apis];


char msg[100];
uint32_t api_getS(void)
{
return vtimerctl_getS(&g_vtimerCtl);
}
uint32_t api_getMs(void)
{
return vtimerctl_getMs(&g_vtimerCtl);
}

int printi(int num){
    printf("%d\n", num);
    return 0;
}
int fuck1(int i,int j)
{
	printi(i);
	return i+j;


}
int fuckvm(int p, Module *m)
{
	strcpy(msg, (m->memory.bytes + p));
		printf("%s\n", msg);
		 vm_fuckJump(m,msg);
		//pop_block(m);
return 0;

}
int prints(int p, Module *m){
    strcpy(msg, (m->memory.bytes + p));
printf("%s\n", msg);
	//vm_exec(m,"test");
//	pop_block(m);
    return 0;
}


int add_func(int index, int nameLen, char *name, void *funcAddr){
    table[index].name = malloc(nameLen);
    table[index].addr = funcAddr;
    memcpy(table[index].name , name, nameLen);
    return 0;
}

int init_table(){
    add_func(0, 7, "printi", (void*)printi);
    add_func(1, 7, "prints", (void*)prints);
    add_func(2, 7, "malloc", (void*)malloc);
    add_func(3, 5, "free", (void*)free);
	add_func(4, 7, "memset", (void*)memset);
	add_func(5, 7, "memcpy", (void*)memcpy);
	add_func(6, 7, "memcmp", (void*)memcmp);
	add_func(7,6,"fuck1",(void*)fuck1);
	add_func(8,5,"getS",(void*)api_getS);
	add_func(9,6,"getMs",(void*)api_getMs);
	add_func(10,7,"fuckvm",(void*)fuckvm);
	add_func(11,7,"fuckos",(void*)os_apiP3);
    return 0;
}

void *sym_table_lookup(char *symbol){
    int i;
    for (i = 0; i < apis; i++){
       if (strcmp(symbol, table[i].name ) == 0){
                return table[i].addr;
       }
    }
    return NULL;
}

bool resolvesym(char *filename, char *symbol, void **val, char **err) {
    info("resolvesym filename: '%s', symbol: '%s'\n", filename, symbol);
    if (filename && strcmp(filename, "env") != 0) {
        *err = "resolvesym with filename unimplmented";
        return false;
    }
    *val = sym_table_lookup(symbol);
    info("           *val: %p\n", *val);
    return true;
}





