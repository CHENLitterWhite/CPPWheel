#ifndef _ATOMIC_H_
#define _ATOMIC_H_

#ifdef __STDC_NO_ATOMICS__

#include <stddef.h>
#include <stdint.h>

// __sync_XXXX:在单个处理器上实现原子性,但在多处理器系统中需要使用锁保证全局原子性
// 配合 volatile(确保对变量的访问是可见且具有一致性) 可以保证原子性和可见性
#define ATOM_INT        volatile int
#define ATOM_POINTER    volatile uintptr_t
#define ATOM_ULONG      volatile unsigned long
#define ATOM_SIZET      volatile size_t

#define ATOM_INIT(PTR, VAL) (*(PTR) = VAL)
#define ATOM_LOAD(PTR) (*(PTR))
#define ATOM_STORE(PTR, VAL) (*(PTR) = VAL)

#define ATOM_CAS(PTR, oval, nval) __sync_bool_compare_and_swap(PTR, oval, nval)
#define ATOM_CAS_ULONG(PTR, oval, nval) __sync_bool_compare_and_swap(PTR, oval, nval)
#define ATOM_CAS_POINTER(PTR, oval, nval) __sync_bool_compare_and_swap(PTR, oval, nval)
#define ATOM_CAS_SIZET(PTR, oval, nval) __sync_bool_compare_and_swap(PTR, oval, nval)

#define ATOM_FINC(PTR) __sync_fetch_and_add(PTR, 1)
#define ATOM_FDEC(PTR) __sync_fetch_and_sub(PTR, 1)
#define ATOM_FADD(PTR,n) __sync_fetch_and_add(PTR, n)
#define ATOM_FSUB(PTR,n) __sync_fetch_and_sub(PTR, n)
#define ATOM_FAND(ptr,n) __sync_fetch_and_and(PTR, n)     // 与


#else

// atomic_xxx: C11 - C++11支持,在多线程环境下

#include <stddef.h>

#ifdef __cplusplus
#include <atomic>
#define atomic_value_type_(p, v) decltype((p)->load())(v) 
#define STD_ std::
#else
#include <stdatomic.h>
#define atomic_value_type_(p, v) v
#define STD_
#endif

#define ATOM_INT             STD_ atomic_int
#define ATOM_POINTER         STD_ atomic_uintptr_t
#define ATOM_SIZET           STD_ atomic_size_t
#define ATOM_ULONG           STD_ atomic_ulong
#define ATOM_INIT(PTR, VAL)  STD_ atomic_init(PTR, VAL)
#define ATOM_LOAD(PTR)       STD_ atomic_load(PTR)
#define ATOM_STORE(PTR, VAL) STD_ atomic_store(PTR, VAL)

// #define ATOM_CAS(ptr, oval, nval) STD_ atomic_compare_exchange_weak(ptr, oval, nval)
// #define ATOM_CAS_POINTER(ptr, oval, nval) STD_ atomic_compare_exchange_weak(ptr, oval, nval)
// #define ATOM_CAS_SIZET(ptr, oval, nval) STD_ atomic_compare_exchange_weak(ptr, oval, nval)
// #define ATOM_CAS_ULONG(ptr, oval, nval) STD_ atomic_compare_exchange_weak(ptr, oval, nval)

static inline int
ATOM_CAS(STD_ atomic_int *ptr, int oval, int nval) {
	return STD_ atomic_compare_exchange_weak(ptr, &(oval), nval);
}

static inline int
ATOM_CAS_POINTER(STD_ atomic_uintptr_t *ptr, uintptr_t oval, uintptr_t nval) {
	return STD_ atomic_compare_exchange_weak(ptr, &(oval), nval);
}

static inline int
ATOM_CAS_SIZET(STD_ atomic_size_t *ptr, size_t oval, size_t nval) {
	return STD_ atomic_compare_exchange_weak(ptr, &(oval), nval);
}

static inline int
ATOM_CAS_ULONG(STD_ atomic_ulong *ptr, unsigned long oval, unsigned long nval) {
	return STD_ atomic_compare_exchange_weak(ptr, &(oval), nval);
}

#define ATOM_FINC(ptr) STD_ atomic_fetch_add(ptr, atomic_value_type_(ptr,1))
#define ATOM_FDEC(ptr) STD_ atomic_fetch_sub(ptr, atomic_value_type_(ptr, 1))
#define ATOM_FADD(ptr,n) STD_ atomic_fetch_add(ptr, atomic_value_type_(ptr, n))
#define ATOM_FSUB(ptr,n) STD_ atomic_fetch_sub(ptr, atomic_value_type_(ptr, n))
#define ATOM_FAND(ptr,n) STD_ atomic_fetch_and(ptr, atomic_value_type_(ptr, n))  // 与

#endif
#endif
