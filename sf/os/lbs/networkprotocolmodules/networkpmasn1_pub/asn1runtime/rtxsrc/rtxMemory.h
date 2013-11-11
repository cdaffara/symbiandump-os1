// Copyright (c) 2001-2009 Objective Systems, Inc. (http://www.obj-sys.com) 
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available 
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Objective Systems, Inc - initial contribution.
//
// Contributors:
//
// Description:
// 

/** 
 * @file rtxMemory.h 
 * Memory management function and macro definitions.
 */

/**
@file
@internalTechnology
*/

#ifndef __RTXMEMORY_H__
#define __RTXMEMORY_H__

#include "rtxsrc/rtxContext.h"

#define RT_MH_DONTKEEPFREE 0x1

#define OSRTMH_PROPID_DEFBLKSIZE   1
#define OSRTMH_PROPID_SETFLAGS     2
#define OSRTMH_PROPID_CLEARFLAGS   3

#define OSRTMH_PROPID_USER         10

#define OSRTXM_K_MEMBLKSIZ         (4*1024)

/**
 * @defgroup rtmem Memory Allocation Macros and Functions 
 *
 * Memory allocation functions and macros handle memory management for the
 * XBinder C run-time. Special algorithms are used for allocation and 
 * deallocation of memory to improve the run-time performance. @{
 */
/**
 * This macro allocates a single element of the given type.
 *
 * @param pctxt        - Pointer to a context block
 * @param type         - Data type of record to allocate
 */
#define OSRTALLOCTYPE(pctxt,type) \
(type*) rtxMemHeapAlloc (&(pctxt)->pMemHeap, sizeof(type))

/**
 * This macro allocates and zeros a single element of the given type.
 *
 * @param pctxt        - Pointer to a context block
 * @param type         - Data type of record to allocate
 */
#define OSRTALLOCTYPEZ(pctxt,type) \
(type*) rtxMemHeapAllocZ (&(pctxt)->pMemHeap, sizeof(type))

/*
 * Reallocate an array. This macro reallocates an array (either expands or
 * contracts) to hold the given number of elements. The number of elements is
 * specified in the <i>n</i> member variable of the pseqof argument.
 *
 * @param pctxt        - Pointer to a context block
 * @param pseqof       - Pointer to a generated SEQUENCE OF array structure.
 *                       The <i>n</i> member variable must be set to the number
 *                       of records to allocate.
 * @param type         - Data type of an array record
 */
#define OSRTREALLOCARRAY(pctxt,pseqof,type) do {\
if (sizeof(type)*(pseqof)->n < (pseqof)->n) return RTERR_NOMEM; \
if (((pseqof)->elem = (type*) rtxMemHeapRealloc \
(&(pctxt)->pMemHeap, (pseqof)->elem, sizeof(type)*(pseqof)->n)) == 0) \
return RTERR_NOMEM; \
} while (0)

#ifndef _NO_MALLOC
#define OSCRTMALLOC0(nbytes)       malloc(nbytes)
#define OSCRTFREE0(ptr)            free(ptr)
#else

#ifdef _NO_THREADS
extern EXTERNRT OSCTXT g_ctxt;

#define OSCRTMALLOC0(nbytes)       rtxMemAlloc(&g_ctxt,(nbytes))
#define OSCRTFREE0(ptr)            rtxMemFreePtr(&g_ctxt,(ptr))
#else
#define OSCRTMALLOC0(nbytes)       (void*)0
#define OSCRTFREE0(ptr)            (void*)0

#endif /* _NO_THREADS */
#endif /* _NO_MALLOC */

#define OSCRTMALLOC rtxMemAlloc
#define OSCRTFREE   rtxMemFreePtr

struct OSRTDList;

#ifdef __cplusplus
extern "C" {
#endif

/* Alias for __cdecl modifier; if __cdecl keyword is not supported, 
 * redefine it as empty macro. */

#if !defined(OSCDECL)
#if defined(_MSC_VER) || defined(__BORLANDC__)
#define OSCDECL __cdecl
#else
#define OSCDECL
#endif
#endif /* OSCDECL */

/* Pointers to C Run-Time memory allocation functions *
 * (See rtxMemSetAllocFuncs)                          */

typedef void *(OSCDECL *OSMallocFunc ) (size_t size);
typedef void *(OSCDECL *OSReallocFunc) (void *ptr, size_t size);
typedef void  (OSCDECL *OSFreeFunc   ) (void *ptr);

#ifdef _RT_ALLOC_FUNC_WITH_CONTEXT

/* Variation of the memory allocation functions only available *
 * after recompilation of the runtime with the macro defined.  *
 * (See rtxMemSetAllocCtxtFuncs)                               */

typedef void *(OSCDECL *OSMallocCtxtFunc ) (OSCTXT* pctxt, size_t size);
typedef void *(OSCDECL *OSReallocCtxtFunc) (OSCTXT* pctxt, void *ptr, size_t size);
typedef void  (OSCDECL *OSFreeCtxtFunc   ) (OSCTXT* pctxt, void *ptr);

#endif

EXTERNRT void  rtxMemHeapAddRef (void** ppvMemHeap);
EXTERNRT void* rtxMemHeapAlloc (void** ppvMemHeap, size_t nbytes);
EXTERNRT void* rtxMemHeapAllocZ (void** ppvMemHeap, size_t nbytes);
EXTERNRT int   rtxMemHeapCheckPtr (void** ppvMemHeap, void* mem_p);
EXTERNRT int   rtxMemHeapCreate (void** ppvMemHeap);
EXTERNRT void  rtxMemHeapFreeAll (void** ppvMemHeap);
EXTERNRT void  rtxMemHeapFreePtr (void** ppvMemHeap, void* mem_p);
EXTERNRT void* rtxMemHeapMarkSaved (void** ppvMemHeap, 
                                     const void* mem_p, 
                                     OSBOOL saved);
EXTERNRT void* rtxMemHeapRealloc (void** ppvMemHeap, 
                                   void* mem_p, size_t nbytes_);
EXTERNRT void  rtxMemHeapRelease (void** ppvMemHeap);
EXTERNRT void  rtxMemHeapReset (void** ppvMemHeap);
EXTERNRT void  rtxMemHeapSetProperty (void** ppvMemHeap, 
                                       OSUINT32 propId, void* pProp);



EXTERNRT void* rtxMemNewArray (size_t nbytes);
EXTERNRT void* rtxMemNewArrayZ (size_t nbytes);
EXTERNRT void rtxMemDeleteArray (void* mem_p);

EXTERNRT void* rtxMemHeapAutoPtrRef (void** ppvMemHeap, void* ptr);
EXTERNRT int rtxMemHeapAutoPtrUnref (void** ppvMemHeap, void* ptr);
EXTERNRT int rtxMemHeapAutoPtrGetRefCount (void** ppvMemHeap, void* mem_p);

#ifndef __SYMBIAN32__
/**
 * This function sets the pointers to standard allocation functions. These
 * functions are used to allocate/reallocate/free memory blocks. By
 * default, standard C functions - 'malloc', 'realloc' and 'free' - are used.
 * But if some platforms do not support these functions (or some other reasons
 * exist) they can be overloaded. The functions being overloaded should have
 * the same prototypes as the standard functions.
 *
 * @param malloc_func Pointer to the memory allocation function ('malloc' by
 *    default).
 * @param realloc_func Pointer to the memory reallocation function ('realloc'
 *    by default).
 * @param free_func Pointer to the memory deallocation function ('free' by
 *    default).
 */
EXTERNRT void rtxMemSetAllocFuncs (OSMallocFunc malloc_func,
                                    OSReallocFunc realloc_func,
                                    OSFreeFunc free_func);
#endif

#ifdef _RT_ALLOC_FUNC_WITH_CONTEXT

EXTERNRT void rtxMemSetAllocCtxtFuncs (OSMallocCtxtFunc malloc_func,
                                        OSReallocCtxtFunc realloc_func,
                                        OSFreeCtxtFunc free_func);

#endif

EXTERNRT void rtxMemFreeOpenSeqExt 
(OSCTXT* pctxt, struct OSRTDList *pElemList);

/*
 * This function returns the actual granularity of memory blocks in the 
 * context.
 *
 * @param pctxt        Pointer to a context block.
 */
EXTERNRT OSUINT32 rtxMemHeapGetDefBlkSize (OSCTXT* pctxt);

#ifndef __SYMBIAN32__
/**
 * This function sets the minimum size and the granularity of memory blocks
 * for newly created memory heaps.  
 *
 * @param blkSize      The minimum size and the granularity of
 *                       memory blocks.
 */
EXTERNRT void rtxMemSetDefBlkSize (OSUINT32 blkSize);
#endif

/**
 * This function returns the actual granularity of memory blocks.
 *
 * @return             The currently used minimum size and the granularity of
 *                       memory blocks.
 */
EXTERNRT OSUINT32 rtxMemGetDefBlkSize ();

/**
 * This helper function determines if an arbitrarily sized block of 
 * memory is set to zero.
 *
 * @param pmem          Pointer to memory block to check
 * @param memsiz        Size of the memory block
 * @return              Boolean result: true if memory is all zero
 */
EXTERNRT OSBOOL rtxMemIsZero (const void* pmem, size_t memsiz);

#ifdef _STATIC_HEAP
EXTERNRT void rtxMemSetStaticBuf (void* memHeapBuf, OSUINT32 blkSize);
#endif

/*
 * Allocate memory.  This macro allocates the given number of bytes.  It is 
 * similar to the C \c malloc run-time function.
 * 
 * @param pctxt - Pointer to a context block
 * @param nbytes - Number of bytes of memory to allocate
 * @return - Void pointer to allocated memory or NULL if insufficient memory 
 *   was available to fulfill the request.
 */
#define rtxMemAlloc(pctxt,nbytes) \
rtxMemHeapAlloc(&(pctxt)->pMemHeap,nbytes)

/*
 * Allocate and zero memory.  This macro allocates the given number of bytes
 * and then initializes the memory block to zero.
 * 
 * @param pctxt - Pointer to a context block
 * @param nbytes - Number of bytes of memory to allocate
 * @return - Void pointer to allocated memory or NULL if insufficient memory 
 *   was available to fulfill the request.
 */
#define rtxMemAllocZ(pctxt,nbytes) \
rtxMemHeapAllocZ(&(pctxt)->pMemHeap,nbytes)

/*
 * Reallocate memory.  This macro reallocates a memory block (either 
 * expands or contracts) to the given number of bytes.  It is 
 * similar to the C \c realloc run-time function.
 * 
 * @param pctxt - Pointer to a context block
 * @param mem_p - Pointer to memory block to reallocate.  This must have been 
 *   allocated using the rtxMemAlloc macro or the rtxMemHeapAlloc function.
 * @param nbytes - Number of bytes of memory to which the block is to be 
 *   resized.
 * @return - Void pointer to allocated memory or NULL if insufficient memory 
 *   was available to fulfill the request.  This may be the same as the pmem 
 *   pointer that was passed in if the block did not need to be relocated.
 */
#define rtxMemRealloc(pctxt,mem_p,nbytes) \
rtxMemHeapRealloc(&(pctxt)->pMemHeap, (void*)mem_p, nbytes)

/*
 * Free memory pointer.  This macro frees memory at the given pointer.  
 * The memory must have been allocated using the rtxMemAlloc (or similar) 
 * macros or the rtxMem memory allocation macros.  This macro is 
 * similar to the C \c free function.
 * 
 * @param pctxt - Pointer to a context block
 * @param mem_p - Pointer to memory block to free.  This must have 
 *   been allocated using the rtxMemAlloc or rtxMemAlloc macro or the 
 *   rtxMemHeapAlloc function.
 */
#define rtxMemFreePtr(pctxt,mem_p) \
rtxMemHeapFreePtr(&(pctxt)->pMemHeap, (void*)mem_p)

/*
 * Free memory associated with a context.  This macro frees all memory 
 * held within a context.  This is all memory allocated using the 
 * rtxMemAlloc (and similar macros) and the rtxMem memory allocation 
 * functions using the given context variable.
 * 
 * @param pctxt - Pointer to a context block
 */
#define rtxMemFree(pctxt) \
rtxMemHeapFreeAll(&(pctxt)->pMemHeap)

/*
 * Reset memory associated with a context.  This macro resets all memory 
 * held within a context.  This is all memory allocated using the rtxMemAlloc 
 * (and similar macros) and the rtxMem memory allocation functions using the 
 * given context variable.
 *
 * <p>The difference between this and the OSMEMFREE macro is that the 
 * memory blocks held within the context are not actually freed.  Internal 
 * pointers are reset so the existing blocks can be reused.  This can 
 * provide a performace improvement for repetitive tasks such as decoding 
 * messages in a loop.
 * 
 * @param pctxt - Pointer to a context block
 */
#define rtxMemReset(pctxt) \
rtxMemHeapReset(&(pctxt)->pMemHeap)

/**
 * Allocate type.  This macro allocates memory to hold a variable of the 
 * given type.
 * 
 * @param pctxt - Pointer to a context block
 * @param ctype - Name of C typedef 
 * @return - Pointer to allocated memory or NULL if insufficient memory 
 *   was available to fulfill the request.
 */
#define rtxMemAllocType(pctxt,ctype) \
(ctype*)rtxMemHeapAlloc(&(pctxt)->pMemHeap,sizeof(ctype))

/**
 * Allocate type and zero memory.  This macro allocates memory to hold a 
 * variable of the given type and initializes the allocated memory to zero.
 * 
 * @param pctxt - Pointer to a context block
 * @param ctype - Name of C typedef 
 * @return - Pointer to allocated memory or NULL if insufficient memory 
 *   was available to fulfill the request.
 */
#define rtxMemAllocTypeZ(pctxt,ctype) \
(ctype*)rtxMemHeapAllocZ(&(pctxt)->pMemHeap,sizeof(ctype))

/*
 * Free memory pointer.  This macro frees memory at the given pointer.  
 * The memory must have been allocated using the rtxMemAlloc (or similar) 
 * macros or the rtxMem memory allocation macros.  This macro is 
 * similar to the C \c free function.
 * 
 * @param pctxt - Pointer to a context block
 * @param mem_p - Pointer to memory block to free.  This must have 
 *   been allocated using the rtxMemAlloc or rtxMemAlloc macro or the 
 *   rtxMemHeapAlloc function.
 */
#define rtxMemFreeType(pctxt,mem_p) \
rtxMemHeapFreePtr(&(pctxt)->pMemHeap, (void*)mem_p)

/**
 * Allocate a dynamic array. This macro allocates a dynamic array of 
 * records of the given type. The pointer to the allocated array is 
 * returned to the caller.
 *
 * @param pctxt        - Pointer to a context block
 * @param n            - Number of records to allocate
 * @param type         - Data type of an array record
 */
#define rtxMemAllocArray(pctxt,n,type) \
(type*)rtxMemHeapAlloc (&(pctxt)->pMemHeap, sizeof(type)*n)

#define rtxMemAllocArrayZ(pctxt,n,type) \
(type*)rtxMemHeapAllocZ (&(pctxt)->pMemHeap, sizeof(type)*n)

/*
 * Free memory pointer.  This macro frees memory at the given pointer.  
 * The memory must have been allocated using the rtxMemAlloc (or similar) 
 * macros or the rtxMem memory allocation macros.  This macro is 
 * similar to the C \c free function.
 * 
 * @param pctxt - Pointer to a context block
 * @param mem_p - Pointer to memory block to free.  This must have 
 *   been allocated using the rtxMemAlloc or rtxMemAlloc macro or the 
 *   rtxMemHeapAlloc function.
 */
#define rtxMemFreeArray(pctxt,mem_p) \
rtxMemHeapFreePtr(&(pctxt)->pMemHeap, (void*)mem_p)

/*
 * Reallocate memory.  This macro reallocates a memory block (either 
 * expands or contracts) to the given number of bytes.  It is 
 * similar to the C \c realloc run-time function.
 * 
 * @param pctxt - Pointer to a context block
 * @param mem_p - Pointer to memory block to reallocate.  This must have been 
 *   allocated using the rtxMemAlloc macro or the rtxMemHeapAlloc function.
 * @param nbytes - Number of bytes of memory to which the block is to be 
 *   resized.
 * @return - Void pointer to allocated memory or NULL if insufficient memory 
 *   was available to fulfill the request.  This may be the same as the pmem 
 *   pointer that was passed in if the block did not need to be relocated.
 */
#define rtxMemReallocArray(pctxt,mem_p,n,type) \
(type*)rtxMemHeapRealloc(&(pctxt)->pMemHeap, (void*)mem_p, sizeof(type)*n)

/* Auto-pointer functions */
/**
 * This function allocates a new block of memory and creates an auto-pointer
 * with reference count set to one.  The \c rtxMemAutoPtrRef and 
 * \c rtxMemAutoPtrUnref functions can be used to increment and decrement 
 * the reference count.  When the count goes to zero, the memory held by 
 * the pointer is freed.
 *
 * @param pctxt         Pointer to a context structure.
 * @param nbytes        Number of bytes to allocate.
 * @return              Pointer to allocated memory or NULL if not enough 
 *                        memory is available.
 */
#define rtxMemNewAutoPtr(pctxt,nbytes) \
rtxMemHeapAlloc(&(pctxt)->pMemHeap, nbytes)

/**
 * This function increments the auto-pointer reference count.
 *
 * @param pctxt         Pointer to a context structure.
 * @param ptr           Pointer on which reference count is to be incremented.
 * @return              Referenced pointer value (ptr argument) or NULL 
 *                        if reference count could not be incremented.
 */
#define rtxMemAutoPtrRef(pctxt,ptr) \
rtxMemHeapAutoPtrRef(&(pctxt)->pMemHeap, ptr)

/**
 * This function decrements the auto-pointer reference count.  If the count 
 * goes to zero, the memory is freed.
 *
 * @param pctxt         Pointer to a context structure.
 * @param ptr           Pointer on which reference count is to be decremented.
 * @return              Positive reference count or a negative error 
 *                        code.  If zero, memory held by pointer will 
 *                        have been freed.
 */
#define rtxMemAutoPtrUnref(pctxt,ptr) \
rtxMemHeapAutoPtrUnref(&(pctxt)->pMemHeap, ptr)

/**
 * This function returns the reference count of the given pointer.
 * goes to zero, the memory is freed.
 *
 * @param pctxt         Pointer to a context structure.
 * @param ptr           Pointer on which reference count is to be fetched.
 * @return              Pointer reference count.
 */
#define rtxMemAutoPtrGetRefCount(pctxt,ptr) \
rtxMemHeapAutoPtrGetRefCount(&(pctxt)->pMemHeap, ptr)

#ifdef __cplusplus
}
#endif
/**
 * @}
 */
#endif /*__RTXMEMORY_H__*/
