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


#ifndef VIDEOFRAMEBUFFER_H
#define VIDEOFRAMEBUFFER_H

#include <e32def.h>
#include <mmf/devvideo/devvideobase.h> // for TUncompressedVideoFormat

/** 
TVideoFrameBuffer represents a buffer for a single decoded video picture. The 
buffer must be contained within a chunk that can be shared across processes.

The frame buffer class is used both for video renderer data flow and with the 
MMMFVideoClientBufferSupport extension.

@publishedPartner
@released
*/
class TVideoFrameBuffer
	{
public:
	IMPORT_C TVideoFrameBuffer(const TUncompressedVideoFormat& aFormat, 
								TInt aStride, 
								TInt aBufferId, 
								const RChunk& aChunk, 
								TInt aOffsetInChunk);

	IMPORT_C TUncompressedVideoFormat Format() const;
	IMPORT_C TUint Stride() const;
	IMPORT_C TInt BufferId() const;
	IMPORT_C const RChunk& Chunk() const;
	IMPORT_C TUint8* Buffer() const;

	/** Status used by video renderer
	@internalComponent
	*/
	enum TBufferStatus
		{
		EAvailable,
		EUsedByClient,
		EWaiting,
		ESubmitted
		};
	
	TBufferStatus BufferStatus() const;
	void SetBufferStatus(TBufferStatus aStatus);
	TDblQueLink& DblQueLink();
	void SetPresentationTime(const TTime& aTime);
	TTime PresentationTime() const;

public: 
	/** Offset for link list
	@internalComponent
	*/
	static const TInt iOffset;

private:
	TUncompressedVideoFormat iFormat;
	TInt iStride;
	TInt iBufferId;
	RChunk iChunk;
	TInt iOffsetInChunk;
	TDblQueLink iDblQueLink;
	TBufferStatus iStatus;
	TTime iPresentationTime;
	};

#endif // VIDEOFRAMEBUFFER_H
