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
// Display Screen Interface implementation for Display Channel
// 
//

#ifndef OPENWFC_RI_DISPLAYCHANNEL_H
#define OPENWFC_RI_DISPLAYCHANNEL_H

#include <WF/openwfc_ri_display.h>
#include <dispchannel.h>

NONSHARABLE_CLASS(COpenWFC_RI_DisplayChannel) : public COpenWFC_RI_Display
    {
public:
    ~COpenWFC_RI_DisplayChannel();
    static COpenWFC_RI_DisplayChannel* NewL(TUint aScreen);
public:
    TInt GetAttribute(TInt aAttributeId, TAny* aAttribute, TInt aAttributeSize);
    TInt SetAttribute(TInt aAttributeId, TAny* aAttribute, TInt aAttributeSize);
    TInt CommitAttribute();
    TInt SetLayerSurface(TInt aLayer, SymbianStreamType aStream,TInt* aNonTrivialAttribs);
    TInt UpdateDisplay();
    TInt SetTopLayerSurface(SymbianStreamType aStream,TInt* aNonTrivialAttribs);
    
private:    
    COpenWFC_RI_DisplayChannel(TUint aScreen);
    void ConstructL();
private:
    struct TBufferAddress
        {
        RChunk  iChunk;
        TInt    iOffset;
        TUint8* iAddress;
        };
private:
    TUint iScreenNumber;
    TInt iRotationOffset;
    RDisplayChannel::TDisplayRotation iDefaultRotation;
    TScreenGeometryAttribute iScreenInfo;
    RArray <TBufferAddress*> iCompositionBuffer;
    RDisplayChannel iDispChan;
    TScreenRotation iNewRotation;
    SymbianStreamType iCurrentSceneStream;
public:
    static const TInt32 KImplementationVersion = 0x00000001;
    };

#endif // OPENWFC_RI_DISPLAYCHANNEL_H
