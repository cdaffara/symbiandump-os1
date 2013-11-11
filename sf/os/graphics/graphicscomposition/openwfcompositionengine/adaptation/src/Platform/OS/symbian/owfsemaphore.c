/* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <errno.h>
#include <time.h>
#include <semaphore.h>
#include <stdlib.h>

#include "owfsemaphore.h"
#include "owfmemory.h"

#define SEM(x)          (sem_t*)(*x)

OWF_API_CALL OWFint
OWF_Semaphore_Init(OWF_SEMAPHORE* sem, OWFint val)
{
    *sem = xalloc(1, sizeof(sem_t));
    if (!*sem) {
        return ENOMEM;
    }
    return sem_init(SEM(sem), 0, val);
}

OWF_API_CALL OWFint
OWF_Semaphore_Wait(OWF_SEMAPHORE* sem)
{
    if (!*sem) {
        return EINVAL;
    }
    return sem_wait(SEM(sem));
}

OWF_API_CALL OWFint
OWF_Semaphore_TryWait(OWF_SEMAPHORE* sem)
{
    if (!*sem) {
        return EINVAL;
    }
    return sem_trywait(SEM(sem));
}

OWF_API_CALL void
OWF_Semaphore_Post(OWF_SEMAPHORE* sem)
{
    if (!*sem) {
        return;
    }
    sem_post(SEM(sem));
}

OWF_API_CALL OWFint
OWF_Semaphore_GetValue(OWF_SEMAPHORE* sem, OWFint* val)
{
    if (!*sem) {
        return EINVAL;
    }
    return sem_getvalue(SEM(sem), (int *)val);
}

OWF_API_CALL OWFint
OWF_Semaphore_Destroy(OWF_SEMAPHORE* sem)
{
    OWFint             err = EINVAL;

    if (*sem) {
        err = sem_destroy(SEM(sem));
        xfree(*sem);
        *sem = NULL;
    }
    return err;
}
