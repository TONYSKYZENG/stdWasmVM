#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include "vmconfig.h"


#define DEBUG VM_DEBUG
#define INFO VM_DEBUG
#define WARN VM_DEBUG
#define TRACE VM_DEBUG


#define FATAL(...) { \
    fprintf(stderr, "Error(%s:%d): ", __FILE__, __LINE__); \
    fprintf(stderr, __VA_ARGS__); exit(1); \
}

#define ASSERT(exp, ...) { \
    if (! (exp)) { \
        fprintf(stderr, "Assert Failed (%s:%d): ", __FILE__, __LINE__); \
        fprintf(stderr, __VA_ARGS__); exit(1); \
    } \
}

#if TRACE
#define trace(...) fprintf(stderr, __VA_ARGS__);
#else
#define trace(...) ;
#endif

#if DEBUG
#define debug(...) fprintf(stderr, __VA_ARGS__);
#else
#define debug(...) ;
#endif

#if INFO
#define info(...) fprintf(stderr, __VA_ARGS__);
#else
#define info(...) ;
#endif

#if WARN
#define warn(...) fprintf(stderr, __VA_ARGS__);
#else
#define warn(...) ;
#endif

#define log(...) fprintf(stderr, __VA_ARGS__);

#define error(...) fprintf(stderr, __VA_ARGS__);


uint64_t read_LEB(uint8_t *bytes, uint32_t *pos, uint32_t maxbits);
uint64_t read_LEB_signed(uint8_t *bytes, uint32_t *pos, uint32_t maxbits);

uint32_t read_uint32(uint8_t *bytes, uint32_t *pos);

char *read_string(uint8_t *bytes, uint32_t *pos, uint32_t *result_len);

// Math
void sext_8_32(uint32_t *val);
void sext_16_32(uint32_t *val);
void sext_8_64(uint64_t *val);
void sext_16_64(uint64_t *val);
void sext_32_64(uint64_t *val);
uint32_t rotl32 (uint32_t n, unsigned int c);
uint32_t rotr32 (uint32_t n, unsigned int c);
uint64_t rotl64(uint64_t n, unsigned int c);
uint64_t rotr64(uint64_t n, unsigned int c);
double wa_fmax(double a, double b);
double wa_fmin(double a, double b);

#endif // of UTIL_H
