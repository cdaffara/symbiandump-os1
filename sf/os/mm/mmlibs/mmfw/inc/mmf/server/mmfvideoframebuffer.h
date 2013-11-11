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
// include\mmf\server\mmfvideoframebuffer.h
// 
//


#ifndef __MMF_SERVER_VIDEOFRAMEBUFFER_H__
#define __MMF_SERVER_VIDEOFRAMEBUFFER_H__

#include <e32base.h>
#include <fbs.h>
#include <mmf/server/mmfbuffer.h>


/**
@publishedAll
@deprecated

Class to store a frame of video data using an EPOC bitmap (RGB data).
*/
class CMMFBitmapFrameBuffer : public CMMFBuffer
	{
public:
	IMPORT_C ~CMMFBitmapFrameBuffer();
	IMPORT_C static CMMFBitmapFrameBuffer* NewL(const TSize& aSize,TDisplayMode aDisplayMode);
	IMPORT_C static CMMFBitmapFrameBuffer* NewL(TInt aBitmapHandle);
	inline TUint BufferSize() const;
	inline CFbsBitmap* Data();
private:
	CMMFBitmapFrameBuffer() : CMMFBuffer(KUidMmfBitmapFrameBuffer) {};
	void ConstructL();
private:
	CFbsBitmap* iFrame;
	};

/**
Dummy implementation. Not applicable to bitmap frame data.

@return Always zero for bitmap frames
*/
inline TUint CMMFBitmapFrameBuffer::BufferSize() const
	{
	return 0; //Not applicable to bitmap
	}

/**
Retrieves the frame data.

@return A pointer to a bitmap containing the frame data.
*/
inline CFbsBitmap* CMMFBitmapFrameBuffer::Data()
	{
	if (iFrame) return iFrame;
	else return NULL;
	}


/**
@publishedAll
@deprecated

Class to store a frame of video data using YUV data instead of an EPOC bitmap.
*/
class CMMFYUVBuffer : public CMMFBuffer
	{
public:
	IMPORT_C static CMMFYUVBuffer* NewL();
	IMPORT_C ~CMMFYUVBuffer();
	inline TUint BufferSize() const;
private:
	CMMFYUVBuffer() : CMMFBuffer(KUidMmfYUVBuffer) {};
	void ConstructL();
public:
	/** The buffer to hold the YUV data */
	HBufC8* iYUVBuffer;
	/** The size of a pixel */
	TSize iPixelSize;
private:
	};

/**
Returns the buffer size.

@return The buffer size.
*/
inline TUint CMMFYUVBuffer::BufferSize() const
	{
	return iYUVBuffer->Size();
	}

#endif
