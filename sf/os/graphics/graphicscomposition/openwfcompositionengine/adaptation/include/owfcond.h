
/* Copyright (c) 2009 The Khronos Group Inc.
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

#ifndef OWFCOND_H_
#define OWFCOND_H_

#include "owftypes.h"
#include "owfmutex.h"


#ifdef __cplusplus
extern "C" {
#endif


typedef void* OWF_COND;

/*! \brief Initialize a condition variable
 *
 *  \param cond
 *  \param mutex mutex to be used when wait is called
 *
 *  \return true or false depending whether initialization
 *  succeeded or not
 */
OWF_API_CALL OWFboolean
OWF_Cond_Init(OWF_COND* cond, OWF_MUTEX mutex);

/*! \brief Destroy a condition variable
 *
 *  \cond cond variable
 */
OWF_API_CALL void
OWF_Cond_Destroy(OWF_COND* cond);

/*! \brief Block on a condition variable
 *
 *  \param cond
 *  \param timeout maximum time to wait in nanoseconds
 *
 * Cond must be initialized with OWF_Cond_Init. Caller MUST hold
 * the mutex specified upon initialization. Call will block on
 * the cond and release mutex until cond is signalled. After cond
 * is signalled the mutex is regained.
 *
 * The call will not block if timeout equals to zero. If timeout is
 * greater than zero, the call will not return until 'timeout'
 * nanoseconds has elapsed or cond is signalled. It timeout equal to
 * OWF_FOREVER, the call returns only after cond is signalled.
 */
OWF_API_CALL OWFboolean
OWF_Cond_Wait(OWF_COND cond, OWFtime timeout);

/*! \brief Signal a condition variable
 *
 *  \param cond
 *
 *  Only one of waiters for cond will be woke-up
 */
OWF_API_CALL OWFboolean
OWF_Cond_Signal(OWF_COND cond);

/*! \brief Signal a condition variable
 *
 *  \param cond
 *
 *  Wake-up all waiters of cond.
 */
OWF_API_CALL OWFboolean
OWF_Cond_SignalAll(OWF_COND cond);


#ifdef __cplusplus
}
#endif



#endif /* OWFCOND_H_ */
