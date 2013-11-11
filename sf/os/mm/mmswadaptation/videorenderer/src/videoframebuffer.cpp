// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

#include <e32std.h>
#include "videoframebuffer.h"

const TInt TVideoFrameBuffer::iOffset = _FOFF(TVideoFrameBuffer, iDblQueLink);

/**
Constructor

@publishedPartner
@prototype 
@param aFormat Data format for the buffer.
@param aStride Stride for the buffer, i.e. the number of bytes from the start 
				of one pixel row to the next.
@param aBufferId Buffer identifier, set by the buffer allocator. Buffer IDs 
				are used to identify buffers in e.g. buffer rendering notifications.
@param aChunk The chunk where the buffer is located. Note that the TVideoFrame
				Buffer does not take ownership of the RChunk.
@param aOffsetInChunk Offset from the beginning of the chunk for this buffer, in bytes.
*/
EXPORT_C TVideoFrameBuffer::TVideoFrameBuffer(const TUncompressedVideoFormat& aFormat, 
												TInt aStride, 
												TInt aBufferId, 
												const RChunk& aChunk, 
												TInt aOffsetInChunk)
: iFormat(aFormat), iStride(aStride), iBufferId(aBufferId), iChunk(aChunk), iOffsetInChunk(aOffsetInChunk)
	{
	}

/**
Returns the data format of the buffer. For YUV formats, typically only 
TUncompressedVideoFormat.iYuvFormat.iDataLayout and iPattern are valid.

@publishedPartner
@prototype 
@return The data format of the buffer.
*/
EXPORT_C TUncompressedVideoFormat TVideoFrameBuffer::Format() const
	{
	return iFormat;
	}

/**
Returns the stride of the buffer. This is the number of bytes from the start 
of one pixel row to the next. The stride is commonly equal to bytes_per_pixel*width, 
but can be larger if the buffer requires padding. With planar YUV data the 
stride refers to luminance (Y) stride. Chrominance (U & V) stride is iStride/2.

@publishedPartner
@prototype 
@return The stride of the buffer.
*/
EXPORT_C TUint TVideoFrameBuffer::Stride() const
	{
	return iStride;
	}

/**
Returns the buffer identifier of the buffer. Buffer IDs are used to identify buffers.

@publishedPartner
@prototype 
@return The buffer id of the buffer.
*/
EXPORT_C TInt TVideoFrameBuffer::BufferId() const
	{
	return iBufferId;
	}

/**
Returns the RChunk where the buffer is located. The RChunk can be shared 
between several TVideoFrameBuffers.

@publishedPartner
@prototype 
@return The RChunk where the buffer is located.
*/
EXPORT_C const RChunk& TVideoFrameBuffer::Chunk() const
	{
	return iChunk;
	}

/**
Returns a pointer to buffer data as mapped to the current process’s address space.

@publishedPartner
@prototype 
@return A pointer to the buffer data.
*/
EXPORT_C TUint8* TVideoFrameBuffer::Buffer() const
	{
	return iChunk.Base() + iOffsetInChunk;
	}

/**
Returns the buffer status.

@internalComponent
@return The buffer status.
*/
TVideoFrameBuffer::TBufferStatus TVideoFrameBuffer::BufferStatus() const
	{
	return iStatus;
	}

/**
Set buffer status.

@internalComponent
@param aStatus Buffer status set by video renderer
*/
void TVideoFrameBuffer::SetBufferStatus(TVideoFrameBuffer::TBufferStatus aStatus)
	{
	iStatus = aStatus;
	}

/**
Returns the queue link for use by video renderer.

@internalComponent
@return A reference to the queue link
*/
TDblQueLink& TVideoFrameBuffer::DblQueLink()
	{
	return iDblQueLink;
	}

/**
Set presentation time.

@internalComponent
@param aTime Presentation time
*/
void TVideoFrameBuffer::SetPresentationTime(const TTime& aTime)
	{
	iPresentationTime = aTime;
	}

/**
Returns presentation time.

@internalComponent
@return Presentation time
*/
TTime TVideoFrameBuffer::PresentationTime() const
	{
	return iPresentationTime;
	}
