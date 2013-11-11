// Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and/or associated documentation files (the
// "Materials"), to deal in the Materials without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Materials, and to
// permit persons to whom the Materials are furnished to do so, subject to
// the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Materials.
//
// THE MATERIALS ARE PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// MATERIALS OR THE USE OR OTHER DEALINGS IN THE MATERIALS.
//
// Description:
// 
// 
//


#include <WF/wfc.h>
#include <e32debug.h>
#include <e32std.h>

#include "wfcdevice.h"
#include "owfdisplaycontextgeneral.h"
#include "owfmutex.h"
#include "owfdisplaycontext.h"
#include <graphics/symbianstream.h>

static const TInt updateReceiverPriorityTable[] = {10, 9, 8, 7, 6, 5, 4, 3};

/* Arbitrary internal version number for SymbianStream registration. */
static const TInt KInternalVersion = 4;

OWF_DISPCTX OWF_DisplayContext_Create(TInt screenNum)
    {
    TInt priorityTableSize = sizeof(updateReceiverPriorityTable) / sizeof(updateReceiverPriorityTable[0]);
    TInt priority = 0;
    
    DPRINT(("++ OWF_DisplayContext_Create(screenNum = %d)", screenNum));
    OWFDisplayContext *pDispCtx = new OWFDisplayContext;
    if (!pDispCtx)
        {
        DPRINT(("-- Not enough memory to create OWFDisplayContext object"));
        return NULL;
        }
    pDispCtx->iEventServerUpdate  = 0;
    pDispCtx->iCompositionOngoing = WFC_FALSE;
    pDispCtx->iPreviousCommit     = WFC_FALSE;
    pDispCtx->iSerialNumber       = INITIAL_CONTEXT_SERIAL_NUMBER;
    pDispCtx->fastpathChecked     = WFC_FALSE;
    pDispCtx->fastpathStream      = OWF_INVALID_HANDLE;
    pDispCtx->iScreenContext      = NULL;
    pDispCtx->iInternalStreamAccessed = WFC_FALSE;    
    
    OWF_Semaphore_Init(&pDispCtx->iRendezvous, 0);
    
    if (screenNum != OWF_RESERVED_BAD_SCREEN_NUMBER)
        {
        if((screenNum < 0) || (screenNum >= priorityTableSize))
            {
            DPRINT(("   Can't retrieve backend priority for the screen number: %d\n", screenNum));
            }
        else
            {
            priority = updateReceiverPriorityTable[screenNum];
            }
		// False positive leaving function is in a TRAPD
		// coverity[leave_without_push]
        TInt err = SymbianStreamRegisterScreenNotifications(screenNum, priority, KInternalVersion);
        if (!(err == KErrNone || err == KErrNotReady || err == KErrAlreadyExists))
            {
            DPRINT(("-- Register with Surface Update Server failed for screen: %d err=%d\n", screenNum, err));
            delete pDispCtx;
            return NULL;
            }
        if (err == KErrNotReady)
            {
            DPRINT(("   Surface Update Server is not ready for screen: %d\n", screenNum));
            }
        if (err == KErrAlreadyExists)
            {
            DPRINT(("   Backend object already exists for screen: %d\n", screenNum));
            }
        }
    
    pDispCtx->iScreenContext = NULL;
    
    DPRINT(("-- OWF_DisplayContext_Create(screenNum = %d) : %p", screenNum, pDispCtx));
    return static_cast<OWF_DISPCTX>(pDispCtx);
    }

void OWF_DisplayContext_Destroy(TInt screenNum, OWF_DISPCTX dc)
    {
    DPRINT(("++ OWF_DisplayContext_Destroy(screenNum = %d) : %p", screenNum, dc));
    if (dc != OWF_INVALID_HANDLE)
        {
        OWFDisplayContext *pDispCtx = _OWF_DISPLAYCONTEXT(dc);
        delete pDispCtx;
        }
    
    if (screenNum != OWF_RESERVED_BAD_SCREEN_NUMBER)
        {
        TInt err = SymbianStreamUnregisterScreenNotifications(screenNum);
        if (!(err == KErrNone || err == KErrNotReady))
            {
            DPRINT(("!! Unregister with Surface Update Server failed for screen: %d with %d\n", screenNum, err));
            }
        }
    DPRINT(("-- OWF_DisplayContext_Destroy()"));
    }

OWFDisplayContext_::~OWFDisplayContext_()
    {
    }

void* COwfScratchBuffers::Allocate(TInt aMinSize)
/**
Create a chunk for a new scratch buffer that is at least aMinSize bytes large.
@param  aMinSize    The minimum size of the buffer required.
@return A pointer to the base of the new buffer if successful; otherwise null is returned. 
*/
    {
    TBufferAddress buf;
    if (buf.iChunk.CreateLocal(aMinSize, aMinSize) != KErrNone) 
        {
        return 0;
        }    
    buf.iAddress = buf.iChunk.Base();
    buf.iOffset = 0;
    if(iBuffers.Append(buf) == KErrNone)
        {
        return buf.iAddress;
        }
    else
        {
        buf.iChunk.Close();
        return 0;
        }
    }

void COwfScratchBuffers::Destroy(void* aAddress)
/**
Destroys the buffer with base address aAddress
@param  aAddress    The address of the buffer to destroy
*/
    {
    for (TInt i = iBuffers.Count() - 1; i >= 0; --i)
        {
        if (iBuffers[i].iAddress == static_cast<TUint8*>(aAddress))
            {
            iBuffers[i].iChunk.Close();
            iBuffers.Remove(i);
            break;
            }
        }
    }
    
COwfScratchBuffers::~COwfScratchBuffers()
/**
Destructor
Close all the chunks.
*/
    {
    for (TInt i = iBuffers.Count() - 1; i >= 0; --i)
        {
        iBuffers[i].iChunk.Close();
        }
    iBuffers.Close();
    }    

void* OWF_DisplayContext_ScratchBuffer_Allocate(OWF_DISPCTX dc, int size)
    {
    OWF_ASSERT(dc);
    
    OWFDisplayContext *pDispCtx = _OWF_DISPLAYCONTEXT(dc);
    return pDispCtx->iBuffers.Allocate(size);
    }

void OWF_DisplayContext_ScratchBuffer_Destroy(OWF_DISPCTX dc, void* buffer)
    {
    OWF_ASSERT(dc);
    
    OWFDisplayContext *pDispCtx = _OWF_DISPLAYCONTEXT(dc);
    pDispCtx->iBuffers.Destroy(buffer);
    }

void OWF_DisplayContext_IncrementSerialNumber(OWF_DISPCTX dc)
    {
    OWF_ASSERT(dc);
    
    OWFDisplayContext *pDispCtx = _OWF_DISPLAYCONTEXT(dc);
    pDispCtx->iSerialNumber++;
    if (pDispCtx->iSerialNumber == INITIAL_CONTEXT_SERIAL_NUMBER)
        {
        pDispCtx->iSerialNumber++;
        }
    }

void OWF_DisplayContext_SetCompositionOngoing(OWF_DISPCTX dc, WFCboolean val)
    {
    OWF_ASSERT(dc);
    
    OWFDisplayContext *pDispCtx = _OWF_DISPLAYCONTEXT(dc);
    pDispCtx->iCompositionOngoing = val;    
    }

void OWF_ComposerThread_Rendezvous(OWF_DISPCTX dc)
    {
    OWF_ASSERT(dc);
    
    OWFDisplayContext *pDispCtx = _OWF_DISPLAYCONTEXT(dc);
    OWF_Semaphore_Post(&pDispCtx->iRendezvous);
    }

void OWF_ComposerThread_RendezvousWait(OWF_DISPCTX dc)
    {
    OWF_ASSERT(dc);
    
    OWFDisplayContext *pDispCtx = _OWF_DISPLAYCONTEXT(dc);
    OWF_Semaphore_Wait(&pDispCtx->iRendezvous);
    }

void OWF_ComposerThread_RendezvousDestroy(OWF_DISPCTX dc)
    {
    OWF_ASSERT(dc);
    
    OWFDisplayContext *pDispCtx = _OWF_DISPLAYCONTEXT(dc);
    OWF_Semaphore_Destroy(&pDispCtx->iRendezvous);
    }

void OWF_DisplayContext_EnableFastpath(OWF_DISPCTX dc, OWFNativeStreamType fastpathStream)
    {
    DPRINT(("++ OWF_DisplayContext_EnableFastpath(%p, %d)", dc, fastpathStream));
    OWF_ASSERT(dc);
    
    OWFDisplayContext *pDispCtx = _OWF_DISPLAYCONTEXT(dc);
    pDispCtx->fastpathStream = fastpathStream;
    }

void OWF_DisplayContext_DisableFastpath(OWF_DISPCTX dc)
    {
    DPRINT(("++ OWF_DisplayContext_DisableFastpath(%p)", dc));
    OWF_ASSERT(dc);
    
    OWFDisplayContext *pDispCtx = _OWF_DISPLAYCONTEXT(dc);
    pDispCtx->fastpathStream = OWF_INVALID_HANDLE;
    }

WFCboolean OWF_DisplayContext_FastpathEnabled(OWF_DISPCTX dc)
    {
    OWF_ASSERT(dc);
    
    OWFDisplayContext *pDispCtx = _OWF_DISPLAYCONTEXT(dc);
    DPRINT(("++ OWF_DisplayContext_FastpathEnabled(%p) : %d", dc, (pDispCtx->fastpathStream != OWF_INVALID_HANDLE)));
    return (pDispCtx->fastpathStream != OWF_INVALID_HANDLE) ? WFC_TRUE : WFC_FALSE;
    }

OWFNativeStreamType OWF_DisplayContext_FastpathStream(OWF_DISPCTX dc)
    {
    OWF_ASSERT(dc);
    
    OWFDisplayContext *pDispCtx = _OWF_DISPLAYCONTEXT(dc);
    DPRINT(("++ OWF_DisplayContext_FastpathStream(%p) : %p", dc, pDispCtx->fastpathStream));
    return pDispCtx->fastpathStream;
    }

WFCboolean OWF_DisplayContext_FastpathChecked(OWF_DISPCTX dc)
    {
    OWF_ASSERT(dc);
    
    OWFDisplayContext *pDispCtx = _OWF_DISPLAYCONTEXT(dc);
    DPRINT(("++ OWF_DisplayContext_FastpathChecked(%p) : %d", dc, pDispCtx->fastpathChecked));
    return pDispCtx->fastpathChecked;
    }

void OWF_DisplayContext_ResetFastpathCheck(OWF_DISPCTX dc)
    {
    DPRINT(("++ OWF_DisplayContext_ResetFastpathCheck(%p)", dc));
    OWF_ASSERT(dc);
    
    OWFDisplayContext *pDispCtx = _OWF_DISPLAYCONTEXT(dc);
    pDispCtx->fastpathChecked = WFC_FALSE;
    }

void OWF_DisplayContext_SetFastpathChecked(OWF_DISPCTX dc)
    {
    DPRINT(("++ OWF_DisplayContext_SetFastpathCheck(%p)", dc));
    OWF_ASSERT(dc);
    
    OWFDisplayContext *pDispCtx = _OWF_DISPLAYCONTEXT(dc);
    pDispCtx->fastpathChecked = WFC_TRUE;
    }

void OWF_DisplayContext_FlagInternalStreamAccessed(OWF_DISPCTX dc)
    {
    DPRINT(("++ OWF_DisplayContext_FlagInternalStreamAccessed(%p)", dc));
    OWF_ASSERT(dc);
    
    OWFDisplayContext *pDispCtx = _OWF_DISPLAYCONTEXT(dc);
    pDispCtx->iInternalStreamAccessed = WFC_TRUE;
    }

WFCboolean OWF_DisplayContext_InternalStreamAccessed(OWF_DISPCTX dc)
    {
    OWF_ASSERT(dc);
    
    OWFDisplayContext *pDispCtx = _OWF_DISPLAYCONTEXT(dc);
    DPRINT(("++ OWF_DisplayContext_InternalStreamAccessed(%p) : ", dc, pDispCtx->iInternalStreamAccessed));
    return pDispCtx->iInternalStreamAccessed;
    }

OWFboolean OWF_DisplayContext_Check_Fastpathed_Stream(OWF_DISPCTX dc, OWFNativeStreamType fastpathedStream)
    {
    if (!dc)
        {
        return OWF_FALSE;
        }
    
    khronos_int32_t pixelFormat = 0;
    SymbianStreamGetHeader((SymbianStreamType)fastpathedStream, NULL, NULL, NULL, &pixelFormat, NULL);
    
    return ((EUidPixelFormatARGB_8888_PRE == pixelFormat) || (EUidPixelFormatARGB_8888 == pixelFormat) || (EUidPixelFormatXRGB_8888 == pixelFormat)) ? OWF_TRUE : OWF_FALSE;
    }

OWFboolean OWF_DisplayContext_CopyFastpathedStreamToTargetStream(void* ctxt)
    {
    DPRINT(("++ OWF_DisplayContext_CopyFastpathedStreamToTargetStream(%p)", ctxt));
    WFC_CONTEXT* context = CONTEXT(ctxt);
    if (!context || !context->displayContext)
        {
        DPRINT(("!! OWF_DisplayContext_CopyFastpathedStreamToTargetStream(%p) invalid", ctxt));
        return OWF_FALSE;
        }
    khronos_int32_t dstStride = context->state.targetImage->stride;
    TInt height = context->state.targetImage->height;
    void* dstPtr     = context->state.targetPixels;
    OWFDisplayContext* pDispCtx = _OWF_DISPLAYCONTEXT(context->displayContext);
    SymbianStreamType fastpathedStream = (SymbianStreamType)pDispCtx->fastpathStream;
    if (!OWF_DisplayContext_Check_Fastpathed_Stream(pDispCtx,pDispCtx->fastpathStream))
        {   //The format does not lend itself to quick copying.
            //Clients listening for update callbacks will be disappointed.
        return OWF_FALSE;
        }
    void* srcPtr     = NULL;
    khronos_int32_t srcStride = 0;
    SymbianStreamBuffer frontBuffer;
    /* Stride may include unaddressable memory, so only copy valid pixels */
    khronos_int32_t copyLen = context->state.targetImage->width * context->state.targetImage->pixelSize;
    
    SymbianStreamAcquireReadBuffer(fastpathedStream, &frontBuffer);
    SymbianStreamGetBufferPointer(fastpathedStream, frontBuffer, &srcPtr);
    
    SymbianStreamGetHeader(fastpathedStream, NULL, NULL, &srcStride, NULL, NULL);
    
    for (TInt i = 0; i < height; i++)
        {
        Mem::Copy(dstPtr, srcPtr, copyLen);
        dstPtr = (khronos_uint8_t*)dstPtr + dstStride;
        srcPtr = (khronos_uint8_t*)srcPtr + srcStride;
        }
    
    SymbianStreamReleaseReadBuffer(fastpathedStream, frontBuffer);
    DPRINT(("-- OWF_DisplayContext_CopyFastpathedStreamToTargetStream(%p)", ctxt));
    return OWF_TRUE;
    }

