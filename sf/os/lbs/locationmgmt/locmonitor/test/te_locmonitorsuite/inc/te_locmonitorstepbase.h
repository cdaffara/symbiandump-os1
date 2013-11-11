/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/


/**
 @file te_locmonitorstepbase.h
*/

#ifndef TE_LOCMONITORSTEPBASE_H
#define TE_LOCMONITORSTEPBASE_H
#include <test/TestExecuteStepBase.h>
#include <e32property.h>
#include "lbstimer.h"
#include "rlbslocmonitorsession.h"
#include "rlbslocmonitorareapositioner.h"
#include "lbssatellite.h"
#include "lbsareainfo.h"
#include "lbslocmonitorserverdata.h"
#include "te_locmonitorsuitedefs.h"
#include "lbsinternalinterface.h"
#include "lbsnetinternalapi.h"

class CLocMonDbFeedbackWaiter;
class CPositionInjector;


/****************************************************************************
* Base Test Step class for te_locmonitorsuite test steps
* 
****************************************************************************/
class CTe_LocMonitorStepBase : public CTestStep
	{
	
public:

	virtual ~CTe_LocMonitorStepBase();
	CTe_LocMonitorStepBase();
	virtual TVerdict doTestStepPreambleL(); 
	virtual TVerdict doTestStepPostambleL();

protected:

	void SetShortDbResponseDelay();
	void SetLongDbResponseDelay();
	void SetDbFailResponse();
	void KillLocationMonitorProcess();

protected:
	
	// Used to request that the Loc Mon DB applies
	// a delay in the processing of the next request.
	RProperty iDbTestCommand;
		
	CLocMonDbFeedbackWaiter* iLocMonDbListener;
	
	CPositionInjector* iPositionInjector;
	
private:

	CActiveScheduler* iActiveScheduler;
	};


/**************************************************************************
* AO to wait for the Location Monitor database feedback (in a P&S property)
****************************************************************************/

class CLocMonDbFeedbackWaiter : public CActive, public MLbsCallbackTimerObserver
{
public:

	static CLocMonDbFeedbackWaiter* NewL();
	~CLocMonDbFeedbackWaiter();
	
	void ListenForLocMonDbFeedback();
	void WaitForDbFeedback();

public:

	TDbFeedback iDbData;
	
private:

	CLocMonDbFeedbackWaiter();
	void ConstructL();
	
	// From MLbsCallbackTimerObserver
	/** Called when the timer expires */
	void OnTimerEventL(TInt aTimerId);
	/** Called if OnTimerEventL leaves */
	TInt OnTimerError(TInt aTimerId, TInt aError);
	
	// From CActive
	void RunL();
	TInt RunError(TInt aError);
	void DoCancel();
	
private:
	
	CActiveSchedulerWait* iNestedScheduler;
	
	CLbsCallbackTimer* iTimer;
	
	RProperty iDbFeedbackProperty;
};


class CPositionInjector : public CBase
{
public:

	static CPositionInjector* NewL();
	~CPositionInjector();
	
	void InjectGpsPosition(const TPosition& aPosition);
	
public:
	
private:

	CPositionInjector();
	void ConstructL();
		
private:

	RLbsPositionUpdates 	iGpsPositionUpdates;// for publishing fake "GPS" positions
};


#endif
