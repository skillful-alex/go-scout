#include <math.h>
#include <string.h>

#include "transition-smallest.h"
#define UNLIKELY(x) __builtin_expect(!!(x), 0)
#define LIKELY(x) __builtin_expect(!!(x), 1)

#define TRAP(x) (wasm_rt_trap(WASM_RT_TRAP_##x), 0)

#define FUNC_PROLOGUE                                            \
  if (++wasm_rt_call_stack_depth > WASM_RT_MAX_CALL_STACK_DEPTH) \
    TRAP(EXHAUSTION)

#define FUNC_EPILOGUE --wasm_rt_call_stack_depth

#define UNREACHABLE TRAP(UNREACHABLE)

#define CALL_INDIRECT(table, t, ft, x, ...)          \
  (LIKELY((x) < table.size && table.data[x].func &&  \
          table.data[x].func_type == func_types[ft]) \
       ? ((t)table.data[x].func)(__VA_ARGS__)        \
       : TRAP(CALL_INDIRECT))

#define MEMCHECK(mem, a, t)  \
  if (UNLIKELY((a) + sizeof(t) > mem->size)) TRAP(OOB)

#define DEFINE_LOAD(name, t1, t2, t3)              \
  static inline t3 name(wasm_rt_memory_t* mem, u64 addr) {   \
    MEMCHECK(mem, addr, t1);                       \
    t1 result;                                     \
    memcpy(&result, &mem->data[addr], sizeof(t1)); \
    return (t3)(t2)result;                         \
  }

#define DEFINE_STORE(name, t1, t2)                           \
  static inline void name(wasm_rt_memory_t* mem, u64 addr, t2 value) { \
    MEMCHECK(mem, addr, t1);                                 \
    t1 wrapped = (t1)value;                                  \
    memcpy(&mem->data[addr], &wrapped, sizeof(t1));          \
  }

DEFINE_LOAD(i32_load, u32, u32, u32);
DEFINE_LOAD(i64_load, u64, u64, u64);
DEFINE_LOAD(f32_load, f32, f32, f32);
DEFINE_LOAD(f64_load, f64, f64, f64);
DEFINE_LOAD(i32_load8_s, s8, s32, u32);
DEFINE_LOAD(i64_load8_s, s8, s64, u64);
DEFINE_LOAD(i32_load8_u, u8, u32, u32);
DEFINE_LOAD(i64_load8_u, u8, u64, u64);
DEFINE_LOAD(i32_load16_s, s16, s32, u32);
DEFINE_LOAD(i64_load16_s, s16, s64, u64);
DEFINE_LOAD(i32_load16_u, u16, u32, u32);
DEFINE_LOAD(i64_load16_u, u16, u64, u64);
DEFINE_LOAD(i64_load32_s, s32, s64, u64);
DEFINE_LOAD(i64_load32_u, u32, u64, u64);
DEFINE_STORE(i32_store, u32, u32);
DEFINE_STORE(i64_store, u64, u64);
DEFINE_STORE(f32_store, f32, f32);
DEFINE_STORE(f64_store, f64, f64);
DEFINE_STORE(i32_store8, u8, u32);
DEFINE_STORE(i32_store16, u16, u32);
DEFINE_STORE(i64_store8, u8, u64);
DEFINE_STORE(i64_store16, u16, u64);
DEFINE_STORE(i64_store32, u32, u64);

#define I32_CLZ(x) ((x) ? __builtin_clz(x) : 32)
#define I64_CLZ(x) ((x) ? __builtin_clzll(x) : 64)
#define I32_CTZ(x) ((x) ? __builtin_ctz(x) : 32)
#define I64_CTZ(x) ((x) ? __builtin_ctzll(x) : 64)
#define I32_POPCNT(x) (__builtin_popcount(x))
#define I64_POPCNT(x) (__builtin_popcountll(x))

#define DIV_S(ut, min, x, y)                                 \
   ((UNLIKELY((y) == 0)) ?                TRAP(DIV_BY_ZERO)  \
  : (UNLIKELY((x) == min && (y) == -1)) ? TRAP(INT_OVERFLOW) \
  : (ut)((x) / (y)))

#define REM_S(ut, min, x, y)                                \
   ((UNLIKELY((y) == 0)) ?                TRAP(DIV_BY_ZERO) \
  : (UNLIKELY((x) == min && (y) == -1)) ? 0                 \
  : (ut)((x) % (y)))

#define I32_DIV_S(x, y) DIV_S(u32, INT32_MIN, (s32)x, (s32)y)
#define I64_DIV_S(x, y) DIV_S(u64, INT64_MIN, (s64)x, (s64)y)
#define I32_REM_S(x, y) REM_S(u32, INT32_MIN, (s32)x, (s32)y)
#define I64_REM_S(x, y) REM_S(u64, INT64_MIN, (s64)x, (s64)y)

#define DIVREM_U(op, x, y) \
  ((UNLIKELY((y) == 0)) ? TRAP(DIV_BY_ZERO) : ((x) op (y)))

#define DIV_U(x, y) DIVREM_U(/, x, y)
#define REM_U(x, y) DIVREM_U(%, x, y)

#define ROTL(x, y, mask) \
  (((x) << ((y) & (mask))) | ((x) >> (((mask) - (y) + 1) & (mask))))
#define ROTR(x, y, mask) \
  (((x) >> ((y) & (mask))) | ((x) << (((mask) - (y) + 1) & (mask))))

#define I32_ROTL(x, y) ROTL(x, y, 31)
#define I64_ROTL(x, y) ROTL(x, y, 63)
#define I32_ROTR(x, y) ROTR(x, y, 31)
#define I64_ROTR(x, y) ROTR(x, y, 63)

#define FMIN(x, y)                                          \
   ((UNLIKELY((x) != (x))) ? NAN                            \
  : (UNLIKELY((y) != (y))) ? NAN                            \
  : (UNLIKELY((x) == 0 && (y) == 0)) ? (signbit(x) ? x : y) \
  : (x < y) ? x : y)

#define FMAX(x, y)                                          \
   ((UNLIKELY((x) != (x))) ? NAN                            \
  : (UNLIKELY((y) != (y))) ? NAN                            \
  : (UNLIKELY((x) == 0 && (y) == 0)) ? (signbit(x) ? y : x) \
  : (x > y) ? x : y)

#define TRUNC_S(ut, st, ft, min, max, maxop, x)                             \
   ((UNLIKELY((x) != (x))) ? TRAP(INVALID_CONVERSION)                       \
  : (UNLIKELY((x) < (ft)(min) || (x) maxop (ft)(max))) ? TRAP(INT_OVERFLOW) \
  : (ut)(st)(x))

#define I32_TRUNC_S_F32(x) TRUNC_S(u32, s32, f32, INT32_MIN, INT32_MAX, >=, x)
#define I64_TRUNC_S_F32(x) TRUNC_S(u64, s64, f32, INT64_MIN, INT64_MAX, >=, x)
#define I32_TRUNC_S_F64(x) TRUNC_S(u32, s32, f64, INT32_MIN, INT32_MAX, >,  x)
#define I64_TRUNC_S_F64(x) TRUNC_S(u64, s64, f64, INT64_MIN, INT64_MAX, >=, x)

#define TRUNC_U(ut, ft, max, maxop, x)                                    \
   ((UNLIKELY((x) != (x))) ? TRAP(INVALID_CONVERSION)                     \
  : (UNLIKELY((x) <= (ft)-1 || (x) maxop (ft)(max))) ? TRAP(INT_OVERFLOW) \
  : (ut)(x))

#define I32_TRUNC_U_F32(x) TRUNC_U(u32, f32, UINT32_MAX, >=, x)
#define I64_TRUNC_U_F32(x) TRUNC_U(u64, f32, UINT64_MAX, >=, x)
#define I32_TRUNC_U_F64(x) TRUNC_U(u32, f64, UINT32_MAX, >,  x)
#define I64_TRUNC_U_F64(x) TRUNC_U(u64, f64, UINT64_MAX, >=, x)

#define DEFINE_REINTERPRET(name, t1, t2)  \
  static inline t2 name(t1 x) {           \
    t2 result;                            \
    memcpy(&result, &x, sizeof(result));  \
    return result;                        \
  }

DEFINE_REINTERPRET(f32_reinterpret_i32, u32, f32)
DEFINE_REINTERPRET(i32_reinterpret_f32, f32, u32)
DEFINE_REINTERPRET(f64_reinterpret_i64, u64, f64)
DEFINE_REINTERPRET(i64_reinterpret_f64, f64, u64)


static u32 func_types[4];

static void init_func_types(void) {
  func_types[0] = wasm_rt_register_func_type(0, 1, WASM_RT_I32);
  func_types[1] = wasm_rt_register_func_type(0, 0);
  func_types[2] = wasm_rt_register_func_type(3, 1, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32);
  func_types[3] = wasm_rt_register_func_type(36, 0, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32);
}

static void __wasm_call_ctors(void);
static void _start(void);
static void cwa_main(void);
static u32 memset_0(u32, u32, u32);
static void resume(void);
static void Transition(u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32);

static u32 g0;
static u32 __heap_base;
static u32 __data_end;
static u32 __dso_handle;

static void init_globals(void) {
  g0 = 65536u;
  __heap_base = 65536u;
  __data_end = 65536u;
  __dso_handle = 65536u;
}

static wasm_rt_memory_t memory;

static wasm_rt_table_t T0;

static void __wasm_call_ctors(void) {
  FUNC_PROLOGUE;
  FUNC_EPILOGUE;
}

static void _start(void) {
  FUNC_PROLOGUE;
  u32 i0;
  i0 = (*Z_envZ_io_get_stdoutZ_iv)();
  FUNC_EPILOGUE;
}

static void cwa_main(void) {
  FUNC_PROLOGUE;
  u32 i0;
  i0 = (*Z_envZ_io_get_stdoutZ_iv)();
  FUNC_EPILOGUE;
}

static u32 memset_0(u32 p0, u32 p1, u32 p2) {
  u32 l3 = 0, l4 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1;
  i0 = 0u;
  l3 = i0;
  L2: 
    i0 = l3;
    i1 = p2;
    i0 = i0 >= i1;
    if (i0) {goto B1;}
    i0 = p0;
    i1 = l3;
    i0 += i1;
    l4 = i0;
    i0 = !(i0);
    if (i0) {goto B0;}
    i0 = l4;
    i1 = p1;
    i32_store8((&memory), (u64)(i0), i1);
    i0 = l3;
    i1 = 1u;
    i0 += i1;
    l3 = i0;
    goto L2;
  B1:;
  i0 = p0;
  goto Bfunc;
  B0:;
  UNREACHABLE;
  Bfunc:;
  FUNC_EPILOGUE;
  return i0;
}

static void resume(void) {
  FUNC_PROLOGUE;
  UNREACHABLE;
  FUNC_EPILOGUE;
}

static void Transition(u32 p0, u32 p1, u32 p2, u32 p3, u32 p4, u32 p5, u32 p6, u32 p7, 
    u32 p8, u32 p9, u32 p10, u32 p11, u32 p12, u32 p13, u32 p14, u32 p15, 
    u32 p16, u32 p17, u32 p18, u32 p19, u32 p20, u32 p21, u32 p22, u32 p23, 
    u32 p24, u32 p25, u32 p26, u32 p27, u32 p28, u32 p29, u32 p30, u32 p31, 
    u32 p32, u32 p33, u32 p34, u32 p35) {
  FUNC_PROLOGUE;
  u32 i0, i1;
  u64 j1;
  i0 = p34;
  i1 = 2u;
  i0 = i0 < i1;
  if (i0) {goto B0;}
  i0 = p33;
  i0 = i32_load8_u((&memory), (u64)(i0 + 1));
  i1 = 1u;
  i0 = i0 != i1;
  if (i0) {goto B1;}
  i0 = 4294967295u;
  p33 = i0;
  L2: 
    i0 = p33;
    i1 = 1u;
    i0 += i1;
    p33 = i0;
    i1 = 32u;
    i0 = i0 < i1;
    if (i0) {goto L2;}
  B1:;
  i0 = p0;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 40), i1);
  i0 = p0;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0 + 32), j1);
  i0 = p0;
  i1 = p1;
  i32_store8((&memory), (u64)(i0), i1);
  i0 = p0;
  i1 = p2;
  i32_store8((&memory), (u64)(i0 + 1), i1);
  i0 = p0;
  i1 = p3;
  i32_store8((&memory), (u64)(i0 + 2), i1);
  i0 = p0;
  i1 = p4;
  i32_store8((&memory), (u64)(i0 + 3), i1);
  i0 = p0;
  i1 = p5;
  i32_store8((&memory), (u64)(i0 + 4), i1);
  i0 = p0;
  i1 = p6;
  i32_store8((&memory), (u64)(i0 + 5), i1);
  i0 = p0;
  i1 = p7;
  i32_store8((&memory), (u64)(i0 + 6), i1);
  i0 = p0;
  i1 = p8;
  i32_store8((&memory), (u64)(i0 + 7), i1);
  i0 = p0;
  i1 = p9;
  i32_store8((&memory), (u64)(i0 + 8), i1);
  i0 = p0;
  i1 = p10;
  i32_store8((&memory), (u64)(i0 + 9), i1);
  i0 = p0;
  i1 = p11;
  i32_store8((&memory), (u64)(i0 + 10), i1);
  i0 = p0;
  i1 = p12;
  i32_store8((&memory), (u64)(i0 + 11), i1);
  i0 = p0;
  i1 = p13;
  i32_store8((&memory), (u64)(i0 + 12), i1);
  i0 = p0;
  i1 = p14;
  i32_store8((&memory), (u64)(i0 + 13), i1);
  i0 = p0;
  i1 = p15;
  i32_store8((&memory), (u64)(i0 + 14), i1);
  i0 = p0;
  i1 = p16;
  i32_store8((&memory), (u64)(i0 + 15), i1);
  i0 = p0;
  i1 = p17;
  i32_store8((&memory), (u64)(i0 + 16), i1);
  i0 = p0;
  i1 = p18;
  i32_store8((&memory), (u64)(i0 + 17), i1);
  i0 = p0;
  i1 = p19;
  i32_store8((&memory), (u64)(i0 + 18), i1);
  i0 = p0;
  i1 = p20;
  i32_store8((&memory), (u64)(i0 + 19), i1);
  i0 = p0;
  i1 = p21;
  i32_store8((&memory), (u64)(i0 + 20), i1);
  i0 = p0;
  i1 = p22;
  i32_store8((&memory), (u64)(i0 + 21), i1);
  i0 = p0;
  i1 = p23;
  i32_store8((&memory), (u64)(i0 + 22), i1);
  i0 = p0;
  i1 = p24;
  i32_store8((&memory), (u64)(i0 + 23), i1);
  i0 = p0;
  i1 = p25;
  i32_store8((&memory), (u64)(i0 + 24), i1);
  i0 = p0;
  i1 = p26;
  i32_store8((&memory), (u64)(i0 + 25), i1);
  i0 = p0;
  i1 = p27;
  i32_store8((&memory), (u64)(i0 + 26), i1);
  i0 = p0;
  i1 = p28;
  i32_store8((&memory), (u64)(i0 + 27), i1);
  i0 = p0;
  i1 = p29;
  i32_store8((&memory), (u64)(i0 + 28), i1);
  i0 = p0;
  i1 = p30;
  i32_store8((&memory), (u64)(i0 + 29), i1);
  i0 = p0;
  i1 = p31;
  i32_store8((&memory), (u64)(i0 + 30), i1);
  i0 = p0;
  i1 = p32;
  i32_store8((&memory), (u64)(i0 + 31), i1);
  goto Bfunc;
  B0:;
  UNREACHABLE;
  Bfunc:;
  FUNC_EPILOGUE;
}


static void init_memory(void) {
  wasm_rt_allocate_memory((&memory), 16, 65536);
}

static void init_table(void) {
  uint32_t offset;
  wasm_rt_allocate_table((&T0), 1, 1);
}

/* export: 'memory' */
wasm_rt_memory_t (*WASM_RT_ADD_PREFIX(Z_memory));
/* export: '__wasm_call_ctors' */
void (*WASM_RT_ADD_PREFIX(Z___wasm_call_ctorsZ_vv))(void);
/* export: '__heap_base' */
u32 (*WASM_RT_ADD_PREFIX(Z___heap_baseZ_i));
/* export: '__data_end' */
u32 (*WASM_RT_ADD_PREFIX(Z___data_endZ_i));
/* export: '__dso_handle' */
u32 (*WASM_RT_ADD_PREFIX(Z___dso_handleZ_i));
/* export: '_start' */
void (*WASM_RT_ADD_PREFIX(Z__startZ_vv))(void);
/* export: 'cwa_main' */
void (*WASM_RT_ADD_PREFIX(Z_cwa_mainZ_vv))(void);
/* export: 'memset' */
u32 (*WASM_RT_ADD_PREFIX(Z_memsetZ_iiii))(u32, u32, u32);
/* export: 'resume' */
void (*WASM_RT_ADD_PREFIX(Z_resumeZ_vv))(void);
/* export: 'Transition' */
void (*WASM_RT_ADD_PREFIX(Z_TransitionZ_viiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii))(u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32);

static void init_exports(void) {
  /* export: 'memory' */
  WASM_RT_ADD_PREFIX(Z_memory) = (&memory);
  /* export: '__wasm_call_ctors' */
  WASM_RT_ADD_PREFIX(Z___wasm_call_ctorsZ_vv) = (&__wasm_call_ctors);
  /* export: '__heap_base' */
  WASM_RT_ADD_PREFIX(Z___heap_baseZ_i) = (&__heap_base);
  /* export: '__data_end' */
  WASM_RT_ADD_PREFIX(Z___data_endZ_i) = (&__data_end);
  /* export: '__dso_handle' */
  WASM_RT_ADD_PREFIX(Z___dso_handleZ_i) = (&__dso_handle);
  /* export: '_start' */
  WASM_RT_ADD_PREFIX(Z__startZ_vv) = (&_start);
  /* export: 'cwa_main' */
  WASM_RT_ADD_PREFIX(Z_cwa_mainZ_vv) = (&cwa_main);
  /* export: 'memset' */
  WASM_RT_ADD_PREFIX(Z_memsetZ_iiii) = (&memset_0);
  /* export: 'resume' */
  WASM_RT_ADD_PREFIX(Z_resumeZ_vv) = (&resume);
  /* export: 'Transition' */
  WASM_RT_ADD_PREFIX(Z_TransitionZ_viiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii) = (&Transition);
}

void WASM_RT_ADD_PREFIX(init)(void) {
  init_func_types();
  init_globals();
  init_memory();
  init_table();
  init_exports();
}
