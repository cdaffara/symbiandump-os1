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

/**
 @file
 @internalComponent
*/

#ifndef BUFFERMANAGER_H
#define BUFFERMANAGER_H

#include <e32std.h>
#include <e32base.h>
#include <e32cmn.h>
#include <graphics/surfacemanager.h>

// class declaration
class TVideoFrameBuffer;

/** Buffer manager for video renderer */
NONSHARABLE_CLASS(CRendererBufferManager) : public CBase
	{
public:
	static CRendererBufferManager* NewL(const RSurfaceManager::TSurfaceInfoV01& aSurfaceInfo, const RArray<TInt>& aBufferOffsets, const RChunk& aChunk, TBool aTimed);
	~CRendererBufferManager();
	TVideoFrameBuffer* NextBuffer();
	TBool ReleaseBuffer(TVideoFrameBuffer* aBuffer);
	TBool UpdateBuffer(TVideoFrameBuffer* aBuffer, const TTime& aTime);
	void BufferAvailable(TInt aBufferId);
	TVideoFrameBuffer* WaitingBuffer(TBool aRemoveFromList, TBool& aIsLast);
	void BufferSubmitted(TVideoFrameBuffer* aBuffer);
	TBool WaitingListIsEmpty();

private:
	CRendererBufferManager(const RChunk& aChunk, TBool aTimed);
	void ConstructL(const RSurfaceManager::TSurfaceInfoV01& aSurfaceInfo, const RArray<TInt>& aBufferOffsets);
	TBool BufferUsedByClient(TVideoFrameBuffer* aBuffer);
	void Lock();
	void Unlock();

private:
	RChunk iChunk;
	TBool iTimed;
	RPointerArray<TVideoFrameBuffer> iBuffers;
	TDblQue<TVideoFrameBuffer> iAvailBuffers;
	TDblQue<TVideoFrameBuffer> iWaitingBuffers; // buffers waiting to be submitted
	TDblQueIter<TVideoFrameBuffer> iWaitingBuffersIter;
	RMutex iMutex;
	};

#endif // BUFFERMANAGER_H
