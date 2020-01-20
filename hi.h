#ifndef HI_H_GENERATED_
#define HI_H_GENERATED_
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "wasm-rt.h"

#ifndef WASM_RT_MODULE_PREFIX
#define WASM_RT_MODULE_PREFIX
#endif

#define WASM_RT_PASTE_(x, y) x ## y
#define WASM_RT_PASTE(x, y) WASM_RT_PASTE_(x, y)
#define WASM_RT_ADD_PREFIX(x) WASM_RT_PASTE(WASM_RT_MODULE_PREFIX, x)

/* TODO(binji): only use stdint.h types in header */
typedef uint8_t u8;
typedef int8_t s8;
typedef uint16_t u16;
typedef int16_t s16;
typedef uint32_t u32;
typedef int32_t s32;
typedef uint64_t u64;
typedef int64_t s64;
typedef float f32;
typedef double f64;

extern void WASM_RT_ADD_PREFIX(init)(void);

/* import: 'env' 'prints' */
extern u32 (*Z_envZ_printsZ_ii)(u32);

/* export: 'memory' */
extern wasm_rt_memory_t (*WASM_RT_ADD_PREFIX(Z_memory));
/* export: 'add' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_addZ_iii))(u32, u32);
/* export: 'sub' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_subZ_iii))(u32, u32);
/* export: 'mul' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_mulZ_iii))(u32, u32);
/* export: 'div' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_divZ_iii))(u32, u32);
/* export: 'test' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_testZ_iv))(void);
#ifdef __cplusplus
}
#endif

#endif  /* HI_H_GENERATED_ */
