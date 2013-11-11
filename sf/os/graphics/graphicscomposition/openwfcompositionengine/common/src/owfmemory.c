/* Copyright (c) 2009 The Khronos Group Inc.
 * Portions copyright (c) 2009-2010  Nokia Corporation and/or its subsidiary(-ies)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and/or associated documentation files (the
 * "Materials"), to deal in the Materials without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Materials, and to
 * permit persons to whom the Materials are furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Materials.
 *
 * THE MATERIALS ARE PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * MATERIALS OR THE USE OR OTHER DEALINGS IN THE MATERIALS.
 */


#ifdef __cplusplus
extern "C"
{
#endif


#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#include "owfmemory.h"
#include "owfdebug.h"

#ifdef DEBUG

#define MAGIK   0x05EAF00D
#define FENCE1  0xFACED00D
#define FENCE2  0xFACE50FF

#define OFFSET(x,y) ((OWFuint32)&(((x*) 0x1000)->y) - 0x1000)

typedef struct BLOCK_ {
    OWFint                      magik;
    struct BLOCK_*              next;
    const char*                 file;
    OWFint                      line;
    OWFuint32                   size;
    OWFuint32                   nInts;
    OWFint                      memory[2];
} BLOCK;




static void
OWF_Memory_Shutdown(void);



static void
OWF_Memory_LockUnlockManagedBlocks(OWFboolean lock)
{
#ifndef DEBUG
    (void) lock;
#endif

#ifdef DEBUG
    /* cannot use OWF_MUTEX because it uses managed memory */
    static pthread_mutex_t mem_mutex = PTHREAD_MUTEX_INITIALIZER;

    if (lock)
    {
        pthread_mutex_lock(&mem_mutex);
    }
    else
    {
        pthread_mutex_unlock(&mem_mutex);
    }
#endif
}

static void
OWF_Memory_LockManagedBlocks()
{
    OWF_Memory_LockUnlockManagedBlocks(OWF_TRUE);
}

static void
OWF_Memory_UnlockManagedBlocks()
{
    OWF_Memory_LockUnlockManagedBlocks(OWF_FALSE);
}


OWF_API_CALL BLOCK*
OWF_Memory_GetSetManagedBlocks(BLOCK* b, OWFboolean set)
{
#ifdef DEBUG
    static BLOCK* managed_blocks = NULL;
    static OWFboolean initialized = OWF_FALSE;

    if (!initialized) {
        /* add exit handler (leak report) */
        atexit(OWF_Memory_Shutdown);
        initialized = OWF_TRUE;
    }

    if (set)
    {
        managed_blocks = b;
        return b;
    }
    else
    {
        return managed_blocks;
    }
#else
    if (set)
    {
        b = b; /* dummy code */
    }
    return NULL;
#endif
}

static BLOCK*
OWF_Memory_GetManagedBlocks()
{
    return OWF_Memory_GetSetManagedBlocks(NULL, OWF_FALSE);
}

static void
OWF_Memory_SetManagedBlocks(BLOCK* b)
{
    OWF_Memory_GetSetManagedBlocks(b, OWF_TRUE);
}

OWF_API_CALL void*
OWF_Memory_Alloc(const char* file, OWFint line, OWFuint32 size)
{
#define INT_SIZE sizeof(OWFint)

    BLOCK* block = NULL;

    /* size rounded to nearest sizeof(int)*n + 2*sizeof(int) for fences */
    OWFuint32 nInts = 2 + (((size + INT_SIZE-1) &~ (INT_SIZE-1)) >> 2);
    OWFuint32 realSize = nInts * INT_SIZE + sizeof(BLOCK);

    if (realSize > size) /* not int overflow */
    {
        block = (BLOCK*) malloc(realSize);
    }

    if (!block)
    {
        DPRINT(("Couldn't alloc %u bytes\n", size));
        return NULL;
    }

    /* populate block header */
    memset(block, 0, realSize);
    block->magik = MAGIK;
    block->file = file;
    block->line = line;
    block->size = size;
    block->nInts = nInts;
    block->memory[0] = FENCE1;
    block->memory[nInts-1] = FENCE2;

    OWF_Memory_LockManagedBlocks();
    {
        /* insert to list at position 0 */
        block->next = (BLOCK*) OWF_Memory_GetManagedBlocks();
        OWF_Memory_SetManagedBlocks(block);
    }
    OWF_Memory_UnlockManagedBlocks();

    return &block->memory[1];
}

OWF_API_CALL void
OWF_Memory_Free(void* ptr)
{
    BLOCK* block        = NULL;

    if (!ptr) {
        return;
    }

    /* POINTER ARITHMETIC HAZARD */
    block = (BLOCK*) ((OWFuint32) ptr - (OWFuint32) OFFSET(BLOCK,memory[1]));

    if (!block)
    {
        DPRINT(("Sanity check failed. Ptr was zero....\n"));
        return;
    }

    OWF_Memory_LockManagedBlocks();
    {
        BLOCK* temp             = NULL;
        BLOCK* prev             = NULL;
        OWFboolean stillExists  = OWF_FALSE;

        temp = OWF_Memory_GetManagedBlocks();

        while (temp)
        {
            /* the block is on the list? */
            if (temp == block && ptr == &temp->memory[1])
            {
                stillExists = OWF_TRUE;
                break;
            }
            prev = temp;
            temp = temp->next;
        }

        if (stillExists == OWF_TRUE)
        {
            if (MAGIK != temp->magik)
            {
                DPRINT(("Possibly corrupt or invalid block, addr = %p\n", ptr));
            }
            if (block->memory[0] != (OWFint)FENCE1)
            {
                DPRINT(("Block's start fence corrupted, addr = %p\n", ptr));
            }
            if (block->memory[block->nInts-1] != (OWFint)FENCE2)
            {
                DPRINT(("Block's end fence corrupted, addr = %p\n", ptr));
            }

            /* valid block, unlink & free it */
            if (prev)
            {
                prev->next = temp->next;
            }
            else
            {
                OWF_Memory_SetManagedBlocks(temp->next);
            }
            free(block);
        }
        else
        {
            /* possibly already freed, strangled pointer. complain. */
            DPRINT(("Block possibly freed already! (block = %p, addr = %p)\n",
                    (void*) block, ptr));
        }
    }
    OWF_Memory_UnlockManagedBlocks();

}

OWF_API_CALL void
OWF_Memory_BlockDump()
{
    BLOCK* block = OWF_Memory_GetManagedBlocks();

    /* managed blocks locked when this is executed */

    while (block)
        {
        BLOCK* next = (BLOCK*) block->next;
        if (MAGIK == block->magik)
            {
            DPRINT(("Block: %p\nFile: %s(%d)\nSize: %u bytes\n",
                            (void*) &block->memory[1],
                            block->file,
                            block->line,
                            block->size));
            }
        else
            {
            DPRINT(("Possibly corrupt or invalid block (addr = %p)\n",
                                (void*) block));
            }
        block = next;
        }
}

#ifdef DEBUG
static void
OWF_Memory_Shutdown()
{
    BLOCK* block = NULL;

    OWF_Memory_LockManagedBlocks();
    {

        if (OWF_Memory_GetManagedBlocks())
        {
            DPRINT(("======================================================================\n"));DPRINT(("MEMORY LEAK REPORT\n"));DPRINT(("======================================================================\n"));
            OWF_Memory_BlockDump();
            block = OWF_Memory_GetManagedBlocks();
            while (block)
            {
                BLOCK* next = (BLOCK*) block->next;
                if (MAGIK == block->magik)
                {
                    free(block);
                }
                block = next;
            }
            OWF_Memory_SetManagedBlocks(NULL);
        }

    }
    OWF_Memory_UnlockManagedBlocks();
}
#endif
#endif

#ifdef __cplusplus
}
#endif
