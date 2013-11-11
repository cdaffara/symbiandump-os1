 // Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @test
 @internalComponent - Internal Symbian test code 
*/

#ifndef CGLPLANETS_H
#define CGLPLANETS_H

#include "eglenvironment.h"
#include "model.h"

#include <e32std.h>
#include <e32base.h>
#include <e32msgqueue.h> 

class CGLPlanetsAppUi;

class CGLPlanets : public CBase
	{
public:
	~CGLPlanets();
	static CGLPlanets* NewL(RWindow& aWindow, TPtrC aBackgroundColor, CGLPlanetsAppUi* aApplication, TPtrC aParentQueueName);
	void Start();
	void Stop();

private:
	CGLPlanets(RWindow& aWindow, CGLPlanetsAppUi* aApplication);
	void ConstructL(TPtrC aBackgroundColor, TPtrC aParentQueueName);

	void Render();
	static TInt TimerCallBack(TAny* aGLPlanets);

private:
	CPeriodic* iTimer;
	CEglEnvironment* iEglEnvironment;
	CModel* iModel;
	TInt iTime;
	RWindow& iWindow;
	RMsgQueue<TInt> iMsgQueue;
	RMsgQueue<TInt> iParentMsgQueue;
	CGLPlanetsAppUi* iApplication;
	};

#endif // CGLPLANETS_H
