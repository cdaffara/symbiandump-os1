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

#ifndef VIDEORENDERER_H
#define VIDEORENDERER_H

#include <e32base.h>
#include <e32cmn.h>
#include <graphics/surface.h>
#include <graphics/surfacemanager.h>
#include <mmf/devvideo/devvideobase.h>

// class declaration
class TVideoFrameBuffer;
class MVideoRendererRelay;
class CRendererBufferManager;
class CThreadUndertaker;

/** The interface implemented by clients using CVideoRenderer

An object of a class which implements this interface should be passed to CVideoRenderer::NewL().

@publishedPartner
@released
@see CVideoRenderer 
*/
class MVideoRendererObserver
	{
public:
	/**
	Called when the renderer has a new video buffer available. The renderer 
	will call this once for each surface buffer when a surface is created, and 
	subsequently for each buffer that becomes available after it has been 
	updated to the display or released.
	*/
	virtual void MvroVideoBufferAvailable() = 0;

	/**
	Called when a buffer has been displayed. The client can use these 
	callbacks to maintain rendering delay statistics.
	@param 	aBufferId Identifies the buffer, equal to TVideoFrameBuffer.iBufferId
	@param aTime The system clock time when the buffer was displayed
	*/
	virtual void MvroBufferDisplayed(TInt aBufferId, const TTime& aTime) = 0;

	/**
	Called when a buffer was skipped in the rendering process. Buffers can be 
	skipped if rendering gets delayed too far and a more recent buffer is 
	already available for rendering. The client can use these callbacks to 
	maintain video rendering statistics.
	@param 	aBufferId Identifies the buffer, equal to TVideoFrameBuffer.iBufferId
	*/
	virtual void MvroBufferSkipped(TInt aBufferId) = 0;
	};

/** 
CVideoRenderer is a utility class intended to be used to perform video 
rendering to graphics surfaces on the behalf of a client. CVideoRenderer has 
two modes: timed and non-timed.

In timed mode the CVideoRenderer will attempt to schedule buffer updates as 
close to the specified presentation time as possible. This is done by running 
a high priority thread which is in responsible for rendering the buffers. 

In non-timed mode the CVideoRenderer will not schedule buffer updates and will 
instead simply render the buffer as soon as the preceding buffer has been 
displayed one or more times. Non-timed mode requires the presence of a CActive
Scheduler in the calling thread.

@publishedPartner
@prototype
*/
NONSHARABLE_CLASS(CVideoRenderer) : public CBase
	{
public:

	IMPORT_C static CVideoRenderer* NewL(MVideoRendererObserver& aObserver, TBool aTimed);
	IMPORT_C ~CVideoRenderer();

	IMPORT_C void GetSupportedFormatsL(RArray<TUncompressedVideoFormat>& aArray);
	IMPORT_C void CreateSurfaceL(const TSize& aSize, TInt aNumBuffers, const TUncompressedVideoFormat& aFormat, TSurfaceId& aSurface);
	IMPORT_C void DestroySurface(const TSurfaceId& aSurface);
	IMPORT_C TVideoFrameBuffer* NextBuffer();
	IMPORT_C void UpdateBuffer(TVideoFrameBuffer* aBuffer, const TTime& aPresentationTime);
	IMPORT_C void ReleaseBuffer(TVideoFrameBuffer* aBuffer);

	void Release();

private:
	CVideoRenderer(MVideoRendererObserver& aObserver, TBool aTimed);
	void ConstructL();
	void RegisterSurfaceL();
	void UnregisterSurface();
	
	static TInt ThreadCreateFunction(TAny* aPtr);
	static void ThreadTrapFunctionL(TAny* aPtr);

private:
	MVideoRendererObserver& iObserver;
	TBool iTimed;
	RSurfaceManager iSurfaceManager;
	MVideoRendererRelay* iRendererRelay;
	RArray<TUncompressedVideoFormat> iSupportedFormat;
	TSurfaceId iSurfaceId;
	CRendererBufferManager* iBufferManager;
	RThread iRendererThread;
	TBool iThreadCreated;
	CThreadUndertaker* iRendererThreadUndertaker;
	RWsSession iWsSession;
	};

#endif  // VIDEORENDERER_H
