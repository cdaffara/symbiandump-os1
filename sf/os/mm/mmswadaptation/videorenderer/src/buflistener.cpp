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

#include "buflistener.h"
#include "buffermanager.h"
#include "videorenderer.h"
#include "rendererrelay.h"
#include "rendererutil.h"

const TInt CBufAvailListener::iOffset = _FOFF(CBufAvailListener, iDblQueLink);

/** Private constructor */
CBufAvailListener::CBufAvailListener(MVideoRendererObserver& aObserver, RSurfaceUpdateSession& aSession)
: CActive(EPriorityStandard), iObserver(aObserver), iSurfaceUpdateSession(aSession)
	{
	CActiveScheduler::Add(this);
	}

/** Two-phased constructor */
CBufAvailListener* CBufAvailListener::NewL(MVideoRendererObserver& aObserver, RSurfaceUpdateSession& aSession)
	{
	CBufAvailListener* self = new (ELeave) CBufAvailListener(aObserver, aSession);
	return self;
	}

CBufAvailListener::~CBufAvailListener()
	{
	Cancel(); // Cancel any request, if outstanding
	}

void CBufAvailListener::DoCancel()
	{
	iSurfaceUpdateSession.CancelAllUpdateNotifications();
	}

/** Set pointer to buffer manager */
void CBufAvailListener::SetBufferManager(CRendererBufferManager* aBufferManager)
	{
	iBufferManager = aBufferManager;
	}

/** Start listener */
void CBufAvailListener::Start(TInt aBufferId)
	{
	iBufferId = aBufferId;
	iSurfaceUpdateSession.NotifyWhenAvailable(iStatus);
	SetActive();
	}

void CBufAvailListener::RunL()
	{
	DEBUGPRINT2(_L("CBufAvailListener::RunL entered with status=%d"), iStatus.Int());
	
	if (iStatus != KErrCancel)
		{
		__ASSERT_DEBUG(iBufferManager != NULL, User::Panic(_L("CBAL::RunL"), KErrCorrupt));
		
		// A submitted buffer become available
		iBufferManager->BufferAvailable(iBufferId);
		iObserver.MvroVideoBufferAvailable();
		}
	}



/** Private constructor */
CBufDisplayListener::CBufDisplayListener(MVideoRendererObserver& aObserver, 
											RSurfaceUpdateSession& aSession, 
											CRendererRelay& aRenderer, 
											TInt aFastCounterFrequency, 
											TBool aFastCounterCountsUp)
: CActive(EPriorityStandard),
  iObserver(aObserver), 
  iSurfaceUpdateSession(aSession), 
  iRenderer(aRenderer), 
  iFastCounterFrequency(aFastCounterFrequency),
  iFastCounterCountsUp(aFastCounterCountsUp)
	{
	CActiveScheduler::Add(this);
	}

/** Two-phased constructor */
CBufDisplayListener* CBufDisplayListener::NewL(MVideoRendererObserver& aObserver, 
												RSurfaceUpdateSession& aSession, 
												CRendererRelay& aRenderer, 
												TInt aFastCounterFrequency, 
												TBool aFastCounterCountsUp)
	{
	CBufDisplayListener* self = new (ELeave) CBufDisplayListener(aObserver, aSession, aRenderer, aFastCounterFrequency, aFastCounterCountsUp);
	return self;
	}

CBufDisplayListener::~CBufDisplayListener()
	{
	Cancel(); // Cancel any request, if outstanding
	}

void CBufDisplayListener::DoCancel()
	{
	iSurfaceUpdateSession.CancelAllUpdateNotifications();
	}

/** Start listener */
void CBufDisplayListener::Start(TInt aBufferId, const TTime& aStartTTime, TUint32 aStartTimeStamp)
	{
	iBufferId = aBufferId;
	iStartTTime = aStartTTime;
	iStartTimeStamp = aStartTimeStamp;
	iSurfaceUpdateSession.NotifyWhenDisplayed(iStatus, iDisplayTimeStamp);
	SetActive();
	}

void CBufDisplayListener::RunL()
	{
	DEBUGPRINT2(_L("CBufDisplayListener::RunL entered with status=%d"), iStatus.Int());

	if (iStatus == KErrNone)
		{
		TInt64 delta = iDisplayTimeStamp() - iStartTimeStamp;
		if (!iFastCounterCountsUp)
			{
			delta = -delta;
			}
		TInt64 microsecond = (delta * 1000000LL) / (TInt64) iFastCounterFrequency;
		TTime displayTime = iStartTTime + TTimeIntervalMicroSeconds(microsecond);
		
		iObserver.MvroBufferDisplayed(iBufferId, displayTime);

		// notify renderer that the next buffer update can be submitted
		iRenderer.BufferDisplayed(ETrue, microsecond);
		}
	else if (iStatus != KErrCancel)
		{
		iObserver.MvroBufferSkipped(iBufferId);

		// notify renderer that the next buffer update can be submitted
		iRenderer.BufferDisplayed(EFalse, 0);
		}
	}

