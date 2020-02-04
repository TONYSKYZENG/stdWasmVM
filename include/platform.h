#ifndef FS_H
#define FS_H

void free_memory();


void *acalloc(size_t nmemb, size_t size,  char *name);
void *arecalloc(void *ptr, size_t old_nmemb, size_t nmemb,
                size_t size,  char *name);

void dumpMemoryInfo();

#endif // of FS_H
