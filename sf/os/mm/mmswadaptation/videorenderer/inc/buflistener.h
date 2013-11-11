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

#ifndef BUFLISTENER_H
#define BUFLISTENER_H

#include <e32base.h>
#include <graphics/surfaceupdateclient.h>

class CRendererBufferManager;
class MVideoRendererObserver;
class CRendererRelay;

/** Listener for buffer available notification from surface update session */
NONSHARABLE_CLASS(CBufAvailListener) : public CActive
	{
public:
	static CBufAvailListener* NewL(MVideoRendererObserver& aObserver, RSurfaceUpdateSession& aSession);
	~CBufAvailListener();
	void SetBufferManager(CRendererBufferManager* aBufferManager);
	void Start(TInt aBufferId);

private:
	CBufAvailListener(MVideoRendererObserver& aObserver, RSurfaceUpdateSession& aSession);
	
	// From CActive
	void RunL();
	void DoCancel();

public:
	static const TInt iOffset;
	TDblQueLink iDblQueLink;

private:
	MVideoRendererObserver& iObserver;
	RSurfaceUpdateSession& iSurfaceUpdateSession;
	CRendererBufferManager* iBufferManager;	// not owned
	TInt iBufferId;
	};

/** Listener for buffer display notification from surface update session */
NONSHARABLE_CLASS(CBufDisplayListener) : public CActive
	{
public:
	static CBufDisplayListener* NewL(MVideoRendererObserver& aObserver, RSurfaceUpdateSession& aSession, CRendererRelay& aRenderer, TInt aFastCounterFrequency, TBool aFastCounterCountsUp);
	~CBufDisplayListener();
	void Start(TInt aBufferId, const TTime& aStartTTime, TUint32 aStartTimeStamp);

private:
	CBufDisplayListener(MVideoRendererObserver& aObserver, RSurfaceUpdateSession& aSession, CRendererRelay& aRenderer, TInt aFastCounterFrequency, TBool aFastCounterCountsUp);
	
	// From CActive
	void RunL();
	void DoCancel();

private:
	MVideoRendererObserver& iObserver;
	RSurfaceUpdateSession& iSurfaceUpdateSession;
	CRendererRelay& iRenderer;
	TInt iBufferId;
	TTime iStartTTime;
	TUint32 iStartTimeStamp;
	TTimeStamp iDisplayTimeStamp;
	TInt iFastCounterFrequency;
	TInt iFastCounterCountsUp;
	};
	
#endif // BUFLISTENER_H
