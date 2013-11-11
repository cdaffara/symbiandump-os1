//
// * Copyright 2004 Neusoft America Inc.
// * All rights reserved.
// * This component and the accompanying materials are made available
// * under the terms of the Eclipse Public License v1.0
// * which accompanies this distribution, and is available
// * at the URL "http://www.eclipse.org/legal/epl-v10.html".
// *
// * Contributors:
// * Keith Collins (Neusoft America Inc.)  original software development and additional code and modifications.
// * Thomas Gahagen (Neusoft America Inc.)  additional code and modifications.
// * Zhen Yuan (Neusoft America Inc.)  additional code and modifications.
// *
// * Description:  This file contains the implementation a very simple timeout class.
//

// timeouter.cpp

/** @file timeouter.cpp
 *
 */
 
#include "timeouter.h"
#include "CsyDebugLogger.h"


CActiveTimeouter* CActiveTimeouter::NewL(MTimeoutObserver& aObserver)
/**
 * 2 phase constructor. Call this function to create a new 
 * active timeouter object.
 *
 * @param aObserver reference to the Observer object
 * @return a newly created CActiveTimeouter object
 */
	{
	CActiveTimeouter* p = new (ELeave) CActiveTimeouter(aObserver);
	CleanupStack::PushL(p);
	p->ConstructL(); 
	CleanupStack::Pop(p);
	return p;
	}

CActiveTimeouter::~CActiveTimeouter()
/**
 * Destructor
 */
	{
	Cancel();
	}

void CActiveTimeouter::Start(const TTimeIntervalMicroSeconds32& aTimeoutMs)
/**
 * Start a timeout
 *
 * @param aTimeoutMs The timeout period in microseconds
 */
	{
	Stop(); //Cancel current request if there is one
	iStatus = KRequestPending;
	After(aTimeoutMs);
	}
  
void CActiveTimeouter::Stop()
/**
 * Stop the timer (if running)
 */
	{

	if (IsActive())
	      Cancel();
	}

CActiveTimeouter::CActiveTimeouter(MTimeoutObserver& aObserver)
/**
 * Constructor
 *
 * @param aObserver reference to the Observer object
 */
 :	CTimer(CActive::EPriorityStandard),
	iTheTimeoutObserver(aObserver)
	{
	CLASSNAMEINIT(CActiveTimeouter)

	CActiveScheduler::Add(this);
	}

void CActiveTimeouter::RunL()
/**
 * Called by the Active Scheduler when the timeout completes.
 * This function calls the TimedOut() function implemented 
 * by the observer.
 */
	{
	iTheTimeoutObserver.TimedOut();
	}

