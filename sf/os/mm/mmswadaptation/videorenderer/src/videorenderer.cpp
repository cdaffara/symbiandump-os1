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

#include "videorenderer.h"
#include "rendererrelay.h"
#include "buffermanager.h"
#include "rendererutil.h"
#include "renderertimer.h"
#include "resourcefilereader.h"
#include "videoframebuffer.h"


_LIT(KVideoRendererThreadName, "VideoRendererThread");
_LIT(KResourceFileName, "Z:\\resource\\videorenderer\\videorenderer.rsc");

const TThreadPriority KSubThreadPriority = EPriorityRealTime;
const TInt KSubThreadStackSize = KDefaultStackSize;	// 0x2000 = 8k



/**
Factory method. Creates a new video renderer instance.

@param aObserver Video renderer observer
@param aTimed ETrue if CVideoRenderer should handle timing of buffer updates. 
              If ETrue CVideoRenderer launches its own high-priority thread to handle buffer update timing. If EFalse CVideoRenderer requires a CActiveScheduler to be present in calling thread.
@return  A pointer to the newly created CVideoRenderer object.
*/
EXPORT_C CVideoRenderer* CVideoRenderer::NewL(MVideoRendererObserver& aObserver, TBool aTimed)
	{
	CVideoRenderer* self = new (ELeave) CVideoRenderer(aObserver, aTimed);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/**
Private constructor
@internalComponent
*/
CVideoRenderer::CVideoRenderer(MVideoRendererObserver& aObserver, TBool aTimed)
:iObserver(aObserver), iTimed(aTimed)
	{
	}

/**
Second phase constructor
@internalComponent
*/
void CVideoRenderer::ConstructL()
	{
	User::LeaveIfError(iSurfaceManager.Open());

	User::LeaveIfError(iWsSession.Connect());
	
	CResourceFileReader* reader = CResourceFileReader::NewLC(KResourceFileName);
	reader->ReadSupportedFormatL(iSupportedFormat);

	if (iTimed)
		{
		// Create a high priority thread for timed mode

		// get timer info for timed mode
		TInt64 defaultDelay;
		TInt64 maxDelay;
		reader->ReadTimerInfoL(defaultDelay, maxDelay);

		//Get a reference to this thread's heap
		RHeap& thisHeap = User::Heap();

		//Parameters to send to the sub thread
		TThreadRelayParam param;
		param.iObserver = &iObserver;
		param.iThreadRelay = &iRendererRelay; // return pointer to allow direct calls

		//Get the id of this thread
		RThread thisThread;
		TThreadId thisThreadId = thisThread.Id();
		param.iMainThreadId = thisThreadId;

		//Get a request to signal for setup completion
		TRequestStatus setupComplete = KRequestPending;
		param.iSetupComplete = &setupComplete;
		
		//current time and the "this" pointer for a unique key
		_LIT(KFormatString,"%S.%020Lu.%08X");
	   	TName threadName;
		TTime now;
		now.UniversalTime();
		threadName.Format(KFormatString, &KVideoRendererThreadName, now.Int64(), reinterpret_cast<TUint>(this));

		//Create a new thread using the same heap as this thread
		TInt result = iRendererThread.Create(threadName,
										ThreadCreateFunction,
										KSubThreadStackSize,
										&thisHeap,
										&param);
		User::LeaveIfError(result);

		//Run the thread under high priority
		iRendererThread.SetPriority(KSubThreadPriority); 

		//Wait for thread startup to complete
		TRequestStatus threadStatus = KRequestPending;
		iRendererThread.Logon(threadStatus);

		//Start the thread
		iRendererThread.Resume();
		User::WaitForRequest(threadStatus, setupComplete);
		if(threadStatus != KRequestPending)
			{
			//Thread creation failed
			TInt reason = iRendererThread.ExitReason();
			DEBUGPRINT3(_L("Renderer thread died with type=%d, reason=%d"), iRendererThread.ExitType(), reason);
			User::Leave(reason);
			}

		// Thread creation was successfull
		TInt error = iRendererThread.LogonCancel(threadStatus);
		User::LeaveIfError(error); // There is no outstanding request
		User::WaitForRequest(threadStatus); // Consume the signal

		__ASSERT_DEBUG(iRendererRelay != NULL, User::Panic(_L("CVR::ConstructL"), KErrCorrupt));
		iRendererRelay->SetRendererThread(&iRendererThread);
		iRendererRelay->SetTimerInfo(defaultDelay, maxDelay);
		
		iThreadCreated = ETrue;
		User::LeaveIfError(setupComplete.Int());

		//Create a listener that will monitor the thread
		iRendererThreadUndertaker = CThreadUndertaker::NewL(iRendererThread);
		}
	else
		{
		iRendererRelay = CRendererRelay::NewL(iObserver);
		}

	CleanupStack::PopAndDestroy(reader);
	}

/**
Main thread entry point for the video renderer sub-thread.
Create a cleanup stack for the thread and process the codec
inside a trap for cleanup behaviour.
@internalComponent

@param aPtr Parameters to be used for creating the thread.
@return The error code for thread termination.
*/
TInt CVideoRenderer::ThreadCreateFunction(TAny* aPtr)
	{
	TInt error = KErrNone;

	// Create a cleanup stack for the thread
	CTrapCleanup* cleanupStack = CTrapCleanup::New();
	if (cleanupStack)
		{
		if(error == KErrNone)
			{
			TRAP(error, ThreadTrapFunctionL(aPtr));
			}
		}
	else
		{
		error = KErrNoMemory;
		}

	delete cleanupStack;
	return error;
	}

/**
Function for thread execution. Create an active scheduler for the thread
and start the function call listener. If the thread is successfully created signal
the main thread that the thread creation was successfull.
@internalComponent

@param aPtr A pointer to a TThreadRelayParam object containing the startup parameters
			for the thread.
*/
void CVideoRenderer::ThreadTrapFunctionL(TAny* aPtr)
	{
	//Create an active scheduler for the thread
	CActiveScheduler* scheduler = new (ELeave) CActiveScheduler;
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install(scheduler);

	//Call a Factory function to create a CSubThreadRelay derived object
	TThreadRelayParam* relayParam;
	relayParam = static_cast<TThreadRelayParam*>(aPtr);

	CRendererThreadRelay* threadRelay = CRendererThreadRelay::NewL(*relayParam->iObserver, relayParam->iMainThreadId);
	CleanupStack::PushL(threadRelay);
	
	//Send a pointer to the sub thread relay back to the main thread
	*relayParam->iThreadRelay = threadRelay;

	threadRelay->Start();
	threadRelay->SignalSetupComplete(relayParam->iSetupComplete);

	CActiveScheduler::Start();	

	CleanupStack::PopAndDestroy(2,scheduler); //threadRelay, scheduler
	}

/**
Destructor
*/
EXPORT_C CVideoRenderer::~CVideoRenderer()
	{
	iWsSession.Close();
	iSupportedFormat.Close();
	
	// first, stop all active objects that may use any buffers
	if(iThreadCreated)
		{
		delete iRendererThreadUndertaker;

		// this also delete CRendererThreadRelay
    	TRequestStatus terminateRequest = KRequestPending;
	    iRendererRelay->Terminate(terminateRequest);
    	User::WaitForRequest(terminateRequest);
        iThreadCreated = EFalse;

        iRendererThread.Close();
		}
	else if (iTimed == EFalse)
		{
		delete iRendererRelay;
		}
	
	// next, close surface if still open
	if (iBufferManager)
		{
		delete iBufferManager;
		iSurfaceManager.CloseSurface(iSurfaceId);
		}
	
	// close the rest
	iSurfaceManager.Close();
	}

/**
Retrieves the list of supported surface formats from the renderer.

@param aArray Supported video formats. The renderer will populate the array with supported formats.
@leave KErrNoMemory Out of memory
*/
EXPORT_C void CVideoRenderer::GetSupportedFormatsL(RArray<TUncompressedVideoFormat>& aArray)
	{
	aArray.Reset(); // first clear the old data
	
	TInt count = iSupportedFormat.Count();
	for (TInt i = 0; i < count; ++i)
		{
		aArray.AppendL(iSupportedFormat[i]);
		}
	}

/**
Creates a new surface for video rendering and registers it with the windows server for all displays.

@param aSize Surface size in pixels
@param aNumBuffers The minimum number of buffers required. The renderer can create a surface with more buffers.
@param aFormat Surface data format
@param aSurface Output: Surface ID for the new surface

@leave KErrNoMemory Out of memory
@leave KErrNotSupported The requested parameters are not supported
@leave KErrInUse Too many video surfaces are already in use
*/
EXPORT_C void CVideoRenderer::CreateSurfaceL(const TSize& aSize, TInt aNumBuffers, const TUncompressedVideoFormat& aFormat, TSurfaceId& aSurface)
	{
	if (iBufferManager != NULL)
		{
		User::Leave(KErrInUse);
		}
	else if (aNumBuffers <= 0)
		{
		User::Leave(KErrNotSupported);
		}
	
	RSurfaceManager::TSurfaceCreationAttributesBuf attribBuf;
	RSurfaceManager::TSurfaceCreationAttributes& attrib = attribBuf();
	attrib.iSize = aSize;
	attrib.iBuffers = aNumBuffers;
	attrib.iPixelFormat = VideoRendererUtil::ConvertUncompressedVideoFormatToUidPixelFormatL(aFormat);
	attrib.iStride = aSize.iWidth * VideoRendererUtil::BytesPerPixelL(attrib.iPixelFormat);
	attrib.iOffsetToFirstBuffer = 0;
	attrib.iAlignment = 4;
	attrib.iContiguous = ETrue;
	attrib.iMappable = ETrue;

	User::LeaveIfError(iSurfaceManager.CreateSurface(attribBuf, iSurfaceId));
	
	// Push surfaceId to cleanup stack in case buffer manager constructor leave.
	// SurfaceId needs to be pushed because the renderer state is inconsistent 
	// if buffer manager creation leave
	CleanupReleasePushL(*this);
	
	RChunk chunk;
	User::LeaveIfError(iSurfaceManager.MapSurface(iSurfaceId, chunk));
	CleanupClosePushL(chunk);
	
	RSurfaceManager::TInfoBuf infoBuf;
	User::LeaveIfError(iSurfaceManager.SurfaceInfo(iSurfaceId, infoBuf));

	// Register must take place after 'this' pushed onto cleanupstack just in case 
	// function leaves after registration
	RegisterSurfaceL();
	
	// prepare the renderer relay
	RSurfaceManager::TSurfaceInfoV01 info = infoBuf();
	if (iTimed)
		{
		TRequestStatus updateRequest = KRequestPending;
		TRequestStatus logonRequest = KRequestPending;

		// While a function call is in progress this thread is suspended
		// and the undertaker will not catch panics, listen for these here
		iRendererThread.Logon(logonRequest);

		// Send request to renderer thread
		iRendererRelay->PrepareL(iSurfaceId, info.iBuffers, &updateRequest);
		User::WaitForRequest(logonRequest, updateRequest);

		if(logonRequest != KRequestPending)
			{
			// renderer thread got panic from surface update session, so panic client
			TInt reason = iRendererThread.ExitReason();
			TExitCategoryName category = iRendererThread.ExitCategory();
			User::Panic(category,reason);
			}

		// Thread is still alive and well
		iRendererThread.LogonCancel(logonRequest);
		User::WaitForRequest(logonRequest); // Consume the signal
		
		// leave if memory allocation failed in renderer thread
		User::LeaveIfError(updateRequest.Int());

		__ASSERT_DEBUG(updateRequest != KRequestPending, User::Panic(_L("CVR::UpdateBuffer"), KErrCorrupt));
		}
	else
		{
		iRendererRelay->PrepareL(iSurfaceId, info.iBuffers, NULL);
		}
	
	// find the buffer offsets
	RArray<TInt> offsets;
	CleanupClosePushL(offsets);
	
	TInt offsetInChunk = 0;
	for (TInt i = 0; i < info.iBuffers; ++i)
		{
		User::LeaveIfError(iSurfaceManager.GetBufferOffset(iSurfaceId, i, offsetInChunk));
		offsets.AppendL(offsetInChunk);
		}
	
	// finally, create buffer manager
	iBufferManager = CRendererBufferManager::NewL(info, offsets, chunk, iTimed);

	CleanupStack::PopAndDestroy(&offsets);
	CleanupStack::Pop(&chunk);
	CleanupStack::Pop(this); // surfaceId
	
	aSurface = iSurfaceId;
	iRendererRelay->SetBufferManager(iBufferManager);
	
	for (TInt i = 0; i < info.iBuffers; ++i)
		{
		// notify observer once for each buffer created
		iObserver.MvroVideoBufferAvailable();
		}
	}

/** 
 Helper function to unregister and release surfaceId when CreateSurfaceL leave
 @internalComponent
 */
void CVideoRenderer::Release()
	{
	UnregisterSurface();
	iSurfaceManager.CloseSurface(iSurfaceId);
	}

/** 
 Helper function to register surfaces for all displays
 @internalComponent
 */
void CVideoRenderer::RegisterSurfaceL()
	{
	TInt screens = iWsSession.NumberOfScreens();
	TInt error;
	
	for(TInt i=0; i<screens; i++)
		{
		error = iWsSession.RegisterSurface(i, iSurfaceId);
		User::LeaveIfError(error);
		}
	}

/** 
 Helper function to unregister surafces for all displays
 @internalComponent
 */
void CVideoRenderer::UnregisterSurface()
	{
	TInt screens = iWsSession.NumberOfScreens();
	
	for(TInt i=0; i<screens; i++)
		{
		iWsSession.UnregisterSurface(i, iSurfaceId);
		}
	}
	
/**
Destroys a surface previously created with CreateSurfaceL() and unregisters it with the windows
server for all displays. The client must stop using the surface before calling this method. All
buffers retrieved with NextBuffer() will become invalid and can no longer be used.

@param aSurface The surface to delete
*/
EXPORT_C void CVideoRenderer::DestroySurface(const TSurfaceId& aSurface)
	{
	if (aSurface != iSurfaceId || iBufferManager == NULL)
		{
		// surface id is not expected or surface has not been created
		return;
		}
	
	// cancel the active objects that use the surface
	if (iTimed)
		{
		TRequestStatus request = KRequestPending;
		TRequestStatus logonRequest = KRequestPending;

		// While a function call is in progress this thread is suspended
		// and the undertaker will not catch panics, listen for these here
		iRendererThread.Logon(logonRequest);

		// Send request to renderer thread
		iRendererRelay->DestroySurface(&request);
		User::WaitForRequest(logonRequest, request);

		if(logonRequest != KRequestPending)
			{
			// renderer thread got panic from surface update session, so panic client
			TInt reason = iRendererThread.ExitReason();
			TExitCategoryName category = iRendererThread.ExitCategory();
			User::Panic(category,reason);
			}

		// Thread is still alive and well
		iRendererThread.LogonCancel(logonRequest);
		User::WaitForRequest(logonRequest); // Consume the signal

		__ASSERT_DEBUG(request != KRequestPending, User::Panic(_L("CVR::DestroySurface"), KErrCorrupt));
		}
	else
		{
		iRendererRelay->DestroySurface(NULL);
		}

	UnregisterSurface();
	
	delete iBufferManager;
	iBufferManager = NULL;
	iRendererRelay->SetBufferManager(NULL);
	
	TInt err = iSurfaceManager.CloseSurface(iSurfaceId);
	DEBUGPRINT2(_L("RSurfaceManager::CloseSurface returned with %d"), err);
	}

/**
Retrieves the next free buffer from the renderer. The client can write data to 
the buffer and update it to the screen using UpdateBuffer(). The buffer will 
remain accessible to the client until it returns it to the renderer with 
UpdateBuffer() or ReleaseBuffer(), the surface is destroyed (DestroySurface()), 
or the renderer object is deleted.

If no free buffers are available the client should wait for a 
MvroVideoBufferAvailable() callback instead of polling for free buffers continuously.

@return A pointer to the free buffer, or NULL if no free buffers are available.
*/
EXPORT_C TVideoFrameBuffer* CVideoRenderer::NextBuffer()
	{
	if (iBufferManager)
		{
		return iBufferManager->NextBuffer();
		}
	
	// surface had not been created, return null
	return NULL;
	}

/**
Updates a surface buffer on the display. The buffer must have previously been 
retrieved with NextBuffer() and must contain a valid video picture. The 
renderer uses statistics from previous video frame rendering times to render 
the picture as close to the requested presentation time as possible.

@param aBuffer The buffer to update
@param aPresentationTime The system clock time when the buffer should be 
			visible on the display. If the time is zero the renderer will 
			update the buffer as soon as possible without further synchronization.
*/
EXPORT_C void CVideoRenderer::UpdateBuffer(TVideoFrameBuffer* aBuffer, const TTime& aPresentationTime)
	{
	__ASSERT_DEBUG(aBuffer != NULL, User::Panic(_L("CVR::UpdateBuffer"), KErrArgument));
	DEBUGPRINT3(_L("CVideoRenderer::UpdateBuffer entered with bufId=%d, aPresentationTime=%Ld"), 
				aBuffer->BufferId() , aPresentationTime.Int64());

	// ignore request if a surface has not been created or has been destroyed
	if (!iBufferManager)
		{
		return;
		}

	if (iTimed)
		{
		TRequestStatus updateRequest = KRequestPending;
		TRequestStatus logonRequest = KRequestPending;

		// While a function call is in progress this thread is suspended
		// and the undertaker will not catch panics, listen for these here
		iRendererThread.Logon(logonRequest);

		// Send request to renderer thread
		iRendererRelay->UpdateBuffer(aBuffer, aPresentationTime, &updateRequest);
		User::WaitForRequest(logonRequest, updateRequest);

		if(logonRequest != KRequestPending)
			{
			// renderer thread got panic from surface update session, so panic client
			TInt reason = iRendererThread.ExitReason();
			TExitCategoryName category = iRendererThread.ExitCategory();
			User::Panic(category,reason);
			}

		// Thread is still alive and well
		iRendererThread.LogonCancel(logonRequest);
		User::WaitForRequest(logonRequest); // Consume the signal

		__ASSERT_DEBUG(updateRequest != KRequestPending, User::Panic(_L("CVR::UpdateBuffer"), KErrCorrupt));
		}
	else
		{
		iRendererRelay->UpdateBuffer(aBuffer, aPresentationTime, NULL);
		}
	}

/**
Releases a buffer without updating it to the display.
The buffer must have previously been retrieved with NextBuffer() and 
UpdateBuffer() is not called yet, otherwise the release request is ignored.

@param aBuffer The buffer to release
*/
EXPORT_C void CVideoRenderer::ReleaseBuffer(TVideoFrameBuffer* aBuffer)
	{
	// ignore if if a surface has not been created or has beed destroyed
	if (!iBufferManager || !aBuffer)
		{
		return;
		}

	if (iBufferManager->ReleaseBuffer(aBuffer))
		{
		// a buffer is releaseed, notify observer
		iObserver.MvroVideoBufferAvailable();
		}
	}
