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

#ifndef RENDERERTIMER_H
#define RENDERERTIMER_H

#include <e32base.h>

class CRendererRelay;

/** Renderer timer for timed mode */
NONSHARABLE_CLASS(CRendererTimer) : public CTimer
	{
public:
	~CRendererTimer();
	static CRendererTimer* NewL(CRendererRelay& aRenderer);
	void Start(TTimeIntervalMicroSeconds32 aDelay);

private:
	CRendererTimer(CRendererRelay& aRenderer);
	void RunL();

private:
	CRendererRelay& iRenderer;
	};

/** This class monitor the renderer sub thread for panics */
NONSHARABLE_CLASS(CThreadUndertaker) : public CActive
	{
public:
	static CThreadUndertaker* NewL(RThread& aSubThread);
	~CThreadUndertaker();

private:
	CThreadUndertaker(RThread& aSubThread);
	void RunL();
	void DoCancel();

private:
	RThread& iSubThread;
	};

#endif // RENDERERTIMER_H
