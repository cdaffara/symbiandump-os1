/* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef OWFDISPLAYCONTEXT_H
#define OWFDISPLAYCONTEXT_H

#include <e32base.h>
#include <e32cmn.h>

#include <dispchannel.h>
#include "owfmemory.h"
#include "owfscreen.h"
#include "owfdisplaycontextgeneral.h"
#include <WF\openwfc_ri_display.h>

#define _OWF_DISPLAYCONTEXT(x)          (static_cast<OWFDisplayContext *>(x))

struct TBufferAddress
    {
    RChunk  iChunk;
    TInt    iOffset;
    TUint8* iAddress;
    };

/**
Container that allocates and destroys the chunks used by the scratch buffers.
*/
class COwfScratchBuffers : public CBase
    {
public:
    void* Allocate(TInt aMinSize);
    void Destroy(void* aAddress);
    ~COwfScratchBuffers();
private:
    RArray <TBufferAddress> iBuffers;
    };

typedef struct OWFDisplayContext_
    {
public:
    COpenWFC_RI_Display *iScreenContext;
    COwfScratchBuffers iBuffers;
    WFCint iEventServerUpdate;
    WFCboolean iCompositionOngoing;
    WFCint iPreviousCommit;
    WFCint iSerialNumber;
    OWF_SEMAPHORE iRendezvous;
    OWFNativeStreamType fastpathStream;
    WFCboolean fastpathChecked;
    WFCboolean iInternalStreamAccessed;
public:
    virtual ~OWFDisplayContext_();
    } OWFDisplayContext;

#endif //OWFDISPLAYCONTEXT_H
