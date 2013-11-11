// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// AGPS Manager test observer base class definition
// 
//

#ifndef __CTESTOBSERVERBASE_H__
#define __CTESTOBSERVERBASE_H__

#include <e32cmn.h>
//#include <ecom/ecom.h>
//#include <lbslocdatasourcegpsbase.h>
//#include <lbs/lbsnetcommon.h>
//#include <lbs/lbsassistancedatabuilderset.h>
#include "lbstimer.h"


/** Base class for test observers.

This class is used to factor out functionality that is
common to all of the observer classes in the AGPS Manager
UNIT test framework.
*/
class CTestObserverBase : public CBase, public MLbsCallbackTimerObserver
	{

public:

	/** Callback method result enumeration.
	This is used to show which observer method was called.
	*/
	enum TCallBackError
		{
		ECallBackErrorNone,
		ECallBackTimeout,
		ECallBackTimerError
		};

public:

	virtual	~CTestObserverBase();
	
	// Reset the observer's called status
	void ResetWasObserverCalled();
	
	// Wait for an observer callback
	void WaitForCallBack();
	
	// Check if the observer was called
	TBool WasObserverCalled() const;

	// Check if more observer activity takes place
	TBool IsMoreActivityObserved();
	
	TInt NumberOfCallbacksReceived();
		
	// MTestTimerObserver methods
	void OnTimerEventL(TInt aTimerId);
	TInt OnTimerError(TInt aTimerId, TInt aError);

protected:

	// Signal that the observer has received a callback
	void SignalObserverCallBack(const TCallBackError aResult);
	
	void CallbackHandled();

	
	void ConstructL();

	CTestObserverBase();

private:

	/** Value relating to the observer method that was called.
	*/
	TCallBackError iCallBackError;
	
	/** Number of callbacks received and waiting to be looked
	at by the test
	**/
	TInt iNumberOfCallbacks;
	
	/** Local (nested) active scheduler used to wait for callbacks
	*/
	CActiveSchedulerWait* iScheduler;
	
	/** Test timer
	*/
	CLbsCallbackTimer* iTimer;
	};

#endif //__CTESTOBSERVERBASE_H__
