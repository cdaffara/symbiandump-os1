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
#ifndef OWFDISPLAYCONTEXTGENERAL_H_
#define OWFDISPLAYCONTEXTGENERAL_H_

#include <WF/wfc.h>
#include <e32def.h>
#include <KHR/khrplatform.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void*   OWF_DISPCTX;

OWF_DISPCTX OWF_DisplayContext_Create(TInt screenNum);
void OWF_DisplayContext_Destroy(TInt screenNum, OWF_DISPCTX dc);

/**
Create a chunk for a new scratch buffer that is at least size bytes large.
@param  dc      The display context that owns the scratch buffer.
@param  size    The minimum size of the buffer required.
@return A pointer to the base of the new buffer if successful; otherwise null is returned. 
*/
void* OWF_DisplayContext_ScratchBuffer_Allocate(OWF_DISPCTX dc, int size);

/**
Destroys the buffer with base address aAddress
@param  dc      The display context that owns the scratch buffer
@param  buffer  A pointer to the buffer to destroy.
*/
void OWF_DisplayContext_ScratchBuffer_Destroy(OWF_DISPCTX dc, void* buffer);

/**
Increments the serial number of the display context
@param  dc      The display context who's serial number is to be incremented
*/
void OWF_DisplayContext_IncrementSerialNumber(OWF_DISPCTX dc);

/**
Sets the composition ongoing flag
@param  dc      The display context who's composition ongoing flag is to be set
@param val      The boolean value
*/
void OWF_DisplayContext_SetCompositionOngoing(OWF_DISPCTX dc, WFCboolean val);

/**
Creates posts the rendezvous semaphore
@param  dc      The display context
*/
void OWF_ComposerThread_Rendezvous(OWF_DISPCTX dc);

/**
Waits on rendezvous semaphore
@param  dc      The display context who's composition ongoing flag is to be set
*/
void OWF_ComposerThread_RendezvousWait(OWF_DISPCTX dc);

/**
Destroys on rendezvous semaphore
@param  dc      The display context
*/
void OWF_ComposerThread_RendezvousDestroy(OWF_DISPCTX dc);

/**
Activates fastpath mechanism and associates the stream output
@param  dc      The display context
@param  fastpathStream      Sets the fastpath stream
*/
void OWF_DisplayContext_EnableFastpath(OWF_DISPCTX dc, OWFNativeStreamType fastpathStream);

/**
Disable fastpath stream
@param  dc      The display context
*/
void OWF_DisplayContext_DisableFastpath(OWF_DISPCTX dc);

/**
Returns fastpath state
@param  dc      The display context
@return OWF_TRUE if the current stream is fastpathable, OWF_FALSE otherwise 
*/
WFCboolean OWF_DisplayContext_FastpathEnabled(OWF_DISPCTX dc);

/**
Returns fastpathed stream
@param  dc      The display context
@return handle of the fastpathed stream 
*/
OWFNativeStreamType OWF_DisplayContext_FastpathStream(OWF_DISPCTX dc);

/**
Retrieves state of the fastpath
@param  dc      The display context
@return OWF_TRUE if the fastpath has been checked, OWF_FALSE otherwise 
*/
WFCboolean OWF_DisplayContext_FastpathChecked(OWF_DISPCTX dc);
/**
Resets fatpath check
@param  dc      The display context
*/
void OWF_DisplayContext_ResetFastpathCheck(OWF_DISPCTX dc);
/**
Sets fatpath check
@param  dc      The display context
*/
void OWF_DisplayContext_SetFastpathChecked(OWF_DISPCTX dc);

/**
Flags that the internal stream has been made accessible for outside world
The fast path mechanism is using it in order to fill the internal stream with
the content of the fastpathed stream
@param  dc      The display context
*/
void OWF_DisplayContext_FlagInternalStreamAccessed(OWF_DISPCTX dc);

/**
Returns the internal stream accessibilty flag
@param  dc      The display context
@return OWF_TRUE if the fastpat has been checked, OWF_FALSE otherwise 
*/
WFCboolean OWF_DisplayContext_InternalStreamAccessed(OWF_DISPCTX dc);

OWFboolean OWF_DisplayContext_CopyFastpathedStreamToTargetStream(void* context);


#ifdef __cplusplus
}
#endif

#endif /* OWFDISPLAYCONTEXTGENERAL_H_ */
