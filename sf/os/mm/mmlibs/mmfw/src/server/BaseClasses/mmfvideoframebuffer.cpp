// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// source\mmf\server\baseclasses\mmfvideoframebuffer.cpp
// 
//

#include "mmfvideoframebuffer.h"


/**
Factory function to create objects of type CMMFYUVBuffer used
to store a frame using YUV data. Allocates and constructs a YUV frame buffer.

@return	A pointer to a fully constructed CMMFYUVBuffer.
*/
EXPORT_C CMMFYUVBuffer* CMMFYUVBuffer::NewL()
	{
	CMMFYUVBuffer* self = new(ELeave) CMMFYUVBuffer;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(); // self
	return self;
	}


void CMMFYUVBuffer::ConstructL()
	{
	}

/**
Destructor.
*/
EXPORT_C CMMFYUVBuffer::~CMMFYUVBuffer()
	{
	delete iYUVBuffer;
	}

/**
Destructor.
*/
EXPORT_C CMMFBitmapFrameBuffer::~CMMFBitmapFrameBuffer()
	{
	if (iFrame) iFrame->Reset(); //release bitmap handle
	delete iFrame;
	}

/**
Factory function to create objects of type CMMFBitmapFrameBuffer used
to store a frame an EPOC bitmap.
Allocates and constructs a bitmap frame buffer with the specified size
and display mode.

@param  aSize
        The bitmap frame buffer size.
@param  aDisplayMode
        The display mode.

@return A pointer to a fully constructed CMMFBitmapFrameBuffer.
*/
EXPORT_C CMMFBitmapFrameBuffer* CMMFBitmapFrameBuffer::NewL(const TSize& aSize,TDisplayMode aDisplayMode)
	{
	CMMFBitmapFrameBuffer* self = new(ELeave) CMMFBitmapFrameBuffer;
	CleanupStack::PushL(self);
	self->ConstructL();
	User::LeaveIfError(self->iFrame->Create(aSize,aDisplayMode));
	CleanupStack::Pop(); // self
	return self;
	}

/**
Factory function to create objects of type CMMFBitmapFrameBuffer used
to store a frame using an EPOC bitmap.

@param  aBitmapHandle
        The handle to the bitmap from which to make a copy.

@return A pointer to a fully constructed CMMFBitmapFrameBuffer.
*/
EXPORT_C CMMFBitmapFrameBuffer* CMMFBitmapFrameBuffer::NewL(TInt aBitmapHandle)
	{
	CMMFBitmapFrameBuffer* self = new(ELeave) CMMFBitmapFrameBuffer;
	CleanupStack::PushL(self);
	self->ConstructL();
	User::LeaveIfError(self->iFrame->Duplicate(aBitmapHandle));
	CleanupStack::Pop(); // self
	return self;
	}

void CMMFBitmapFrameBuffer::ConstructL()
	{
	iFrame = new(ELeave)CFbsBitmap;
	}
