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

#include "cshutdowntimer.h"
#include <e32debug.h>
// Lbs
#include "lbsdevloggermacros.h"
/** 
Default constructor, set the timer to standard priority by default
add timer to active scheduler

@internalTechnology
@released
 */	
CShutdownTimer::CShutdownTimer() : CTimer(CActive::EPriorityStandard)
	{
	CActiveScheduler::Add(this);
	}

/** 
Default destructor, cancel any outstanding request of timer active object

@internalTechnology
@released
 */	
CShutdownTimer::~CShutdownTimer()
	{
	LBSLOG(ELogP1,"CShutdownTimer::~CShutdownTimer");
	Cancel();
	}

/** 
Call CTimer base class to construct the shut down timer

@internalTechnology
@released
 */	
void CShutdownTimer::ConstructL()
	{
	// call the base class version
	CTimer::ConstructL();
	}

/** 
start the timer by specifying the shut down delay to issue a time request 

@param aDelay Time interval in micro seconds
@internalTechnology
@released
 */	
void CShutdownTimer::Start(const TTimeIntervalMicroSeconds32 aDelay)
	{
	LBSLOG(ELogP1,"->CShutdownTimer::Start");
	After(aDelay);
	LBSLOG(ELogP1,"<-CShutdownTimer::Start");
	}
	
/** 
stop the timer by cancelling outstanding time request

@internalTechnology
@released
 */	
void CShutdownTimer::Stop()
	{
	LBSLOG(ELogP1,"->CShutdownTimer::Stop");
	Cancel();
	LBSLOG(ELogP1,"<-CShutdownTimer::Stop");
	}
	
/** 
inherited from CActive. Main message loop to deal with dispatched request
stop the active scheduler whenever a request is received. 
 this stops the AS that owns the CPolicyServer AO. So it stops the server.

@internalTechnology
@released
 */	
void CShutdownTimer::RunL()
	{
	LBSLOG(ELogP1,"->CShutdownTimer::RunL");
	CActiveScheduler::Stop();
	LBSLOG(ELogP1,"<-CShutdownTimer::Stop");

	}
