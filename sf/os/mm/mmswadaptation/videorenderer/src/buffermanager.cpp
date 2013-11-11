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

#include <mmf/devvideo/devvideobase.h>
#include "buffermanager.h"
#include "videoframebuffer.h"
#include "rendererutil.h"

/** Constructor for performing 1st stage construction */
CRendererBufferManager::CRendererBufferManager(const RChunk& aChunk, TBool aTimed)
: iChunk(aChunk),
  iTimed(aTimed),
  iAvailBuffers(TVideoFrameBuffer::iOffset), 
  iWaitingBuffers(TVideoFrameBuffer::iOffset), 
  iWaitingBuffersIter(iWaitingBuffers)
	{}

/** Destructor.	*/
CRendererBufferManager::~CRendererBufferManager()
	{
	if (iTimed)
		{
		iMutex.Close();
		}
	iAvailBuffers.Reset();
	iWaitingBuffers.Reset();
	iBuffers.ResetAndDestroy();
	iChunk.Close();
	}

/** Two-phased constructor. */
CRendererBufferManager* CRendererBufferManager::NewL(const RSurfaceManager::TSurfaceInfoV01& aSurfaceInfo, const RArray<TInt>& aBufferOffsets, const RChunk& aChunk, TBool aTimed)
	{
	CRendererBufferManager* self = new (ELeave) CRendererBufferManager(aChunk, aTimed);
	CleanupStack::PushL(self);
	self->ConstructL(aSurfaceInfo, aBufferOffsets);
	CleanupStack::Pop(); // self;
	return self;
	}

/** Constructor for performing 2nd stage construction */
void CRendererBufferManager::ConstructL(const RSurfaceManager::TSurfaceInfoV01& aSurfaceInfo, const RArray<TInt>& aBufferOffsets)
	{
	if (iTimed)
		{
		User::LeaveIfError(iMutex.CreateLocal());
		}
	
	TUncompressedVideoFormat format = VideoRendererUtil::ConvertUidPixelFormatToUncompressedVideoFormatL(aSurfaceInfo.iPixelFormat);
	
	TVideoFrameBuffer* buffer = NULL;
	for (TInt i = 0; i < aSurfaceInfo.iBuffers; ++i)
		{
		buffer = new (ELeave) TVideoFrameBuffer(format, aSurfaceInfo.iStride, i, iChunk, aBufferOffsets[i]);
		buffer->SetBufferStatus(TVideoFrameBuffer::EAvailable);
		CleanupStack::PushL(buffer);
		iBuffers.AppendL(buffer);
		CleanupStack::Pop(buffer);
		
		iAvailBuffers.AddLast(*buffer);
		}
	}

/** Lock buffer table on timed mode */
void CRendererBufferManager::Lock()
	{
	if (iTimed)
		{
		iMutex.Wait();
		}
	}

/** Unlock buffer table on timed mode */
void CRendererBufferManager::Unlock()
	{
	if (iTimed)
		{
		iMutex.Signal();
		}
	}

/** Get the next available buffer */
TVideoFrameBuffer* CRendererBufferManager::NextBuffer()
	{
	Lock();
	
	TVideoFrameBuffer* buffer = NULL;
	if (!iAvailBuffers.IsEmpty())
		{
		buffer = iAvailBuffers.First();
		
		__ASSERT_DEBUG(buffer->BufferStatus() == TVideoFrameBuffer::EAvailable,
				User::Panic(_L("CRBM::NextBuffer"), KErrCorrupt));
		buffer->SetBufferStatus(TVideoFrameBuffer::EUsedByClient);
		buffer->DblQueLink().Deque();
		}

	Unlock();
	return buffer;
	}

/** Check if a buffer was previously sent to client */
TBool CRendererBufferManager::BufferUsedByClient(TVideoFrameBuffer* aBuffer)
	{
	TVideoFrameBuffer* buffer = iBuffers[aBuffer->BufferId()];
	if (buffer->BufferStatus() == TVideoFrameBuffer::EUsedByClient && aBuffer == buffer)
		{
		return ETrue;
		}
	return EFalse;
	}

/** Release the buffer for reuse */
TBool CRendererBufferManager::ReleaseBuffer(TVideoFrameBuffer* aBuffer)
	{
	TBool released = EFalse;
	Lock();
	
	if (BufferUsedByClient(aBuffer))
		{
		// release the buffer if it was previously sent to client
		aBuffer->SetBufferStatus(TVideoFrameBuffer::EAvailable);
		iAvailBuffers.AddLast(*aBuffer);		
		released = ETrue;
		}

	Unlock();
	return released;
	}

/** Received notification that buffer become available */
void CRendererBufferManager::BufferAvailable(TInt aBufferId)
	{
	Lock();

	TVideoFrameBuffer* buffer = iBuffers[aBufferId];

	__ASSERT_DEBUG(buffer->BufferStatus() == TVideoFrameBuffer::EWaiting || buffer->BufferStatus() == TVideoFrameBuffer::ESubmitted,
			User::Panic(_L("CRBM::BufferAvailable"), KErrCorrupt));

	buffer->SetBufferStatus(TVideoFrameBuffer::EAvailable);
	buffer->DblQueLink().Deque();
	iAvailBuffers.AddLast(*buffer);
	
	Unlock();
	}

/** Get the next waiting buffer to submit
 @param aRemoveFromList If true, mark the buffer as submitted and deque from waiting list
 @param aIsLast ETrue on result if the returned buffer is the last buffer in waiting list
 */
TVideoFrameBuffer* CRendererBufferManager::WaitingBuffer(TBool aRemoveFromList, TBool& aIsLast)
	{
	Lock();
	
	TVideoFrameBuffer* buf = NULL;
	if (iWaitingBuffers.IsEmpty() == EFalse)
		{
		buf = iWaitingBuffers.First();

		__ASSERT_DEBUG(buf->BufferStatus() == TVideoFrameBuffer::EWaiting,
				User::Panic(_L("CRBM::WaitingBuffer"), KErrCorrupt));
		aIsLast = iWaitingBuffers.IsLast(buf);

		if (aRemoveFromList)
			{
			buf->SetBufferStatus(TVideoFrameBuffer::ESubmitted);
			buf->DblQueLink().Deque();
			}
		}

	Unlock();
	return buf;
	}

/** Mark the buffer as submitted and deque from waiting list */
void CRendererBufferManager::BufferSubmitted(TVideoFrameBuffer* aBuffer)
	{
	Lock();

	__ASSERT_DEBUG(aBuffer->BufferStatus() == TVideoFrameBuffer::EWaiting,
			User::Panic(_L("CRBM::BufferSubmitted"), KErrArgument));

	aBuffer->SetBufferStatus(TVideoFrameBuffer::ESubmitted);
	aBuffer->DblQueLink().Deque();

	Unlock();
	}

/** Return ETrue if the waiting list is empty */
TBool CRendererBufferManager::WaitingListIsEmpty()
	{
	Lock();
	
	TBool ret = iWaitingBuffers.IsEmpty();
	
	Unlock();
	return ret;
	}

/** 
Mark a buffer as waiting for update and add to waiting list, return ETrue 
if rescheduled is needed (i.e. head of waiting list has changed and the list wasn't empty) */
TBool CRendererBufferManager::UpdateBuffer(TVideoFrameBuffer* aBuffer, const TTime& aTime)
	{
	Lock();
	
	TBool headChanged = EFalse;
	if (BufferUsedByClient(aBuffer))
		{
		aBuffer->SetBufferStatus(TVideoFrameBuffer::EWaiting);
		aBuffer->SetPresentationTime(aTime);
		
		// add buffer to waiting buffer list according to presentation time
		if (iWaitingBuffers.IsEmpty())
			{
			iWaitingBuffers.AddLast(*aBuffer);
			}
		else
			{
			TVideoFrameBuffer* buf = iWaitingBuffers.Last();
			if (aTime >= buf->PresentationTime())
				{
				iWaitingBuffers.AddLast(*aBuffer);
				}
			else
				{
				// client tried to insert a older frame, search for the right position to insert
				iWaitingBuffersIter.SetToFirst();
				while ((buf = iWaitingBuffersIter++) != NULL)
					{
					if (aTime < buf->PresentationTime())
						{
						// Found the slot
						if (iWaitingBuffers.IsFirst(buf))
							{
							iWaitingBuffers.AddFirst(*aBuffer);
							headChanged = ETrue;
							}
						else
							{
							aBuffer->DblQueLink().AddBefore(&(buf->DblQueLink()));
							}
						}
					}
				}
			}
		}
	else
		{
		DEBUGPRINT1(_L("CRendererBufferManager::UpdateBuffer receive buffer not usable by client"));
		__ASSERT_DEBUG(EFalse, User::Panic(_L("CRBM::UpdateBuf"), KErrBadHandle));
		}
	
	Unlock();
	
	return headChanged;
	}
