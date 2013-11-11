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
//

#ifndef __SSMSWPINDIVIDUALTRANSITIONSCHEDULER_H__
#define __SSMSWPINDIVIDUALTRANSITIONSCHEDULER_H__

#include <e32base.h>

class CSsmSwpTransitionEngine;
class MSwpSchedulerMonitor;

class CSsmSwpTransitionScheduler;

/**
 The scheduler class is responsible for submitting and scheduling transitions sequencially
 If  transition is active, the scheduler will queue the transition until all active 
 and queued transitions have been completed.
 
 @internalComponent
 @released
 */
class CSsmSwpIndividualTransitionScheduler : public CActive
	{

public:
	// methods
	static CSsmSwpIndividualTransitionScheduler* NewLC(TUint aSwpKey, MCleSessionProxy* aCleSession, CSsmSwpTransitionScheduler& aParent);
	~CSsmSwpIndividualTransitionScheduler();

	void SubmitL(CSsmSwpTransitionEngine* aEngine); // takes ownership
	void Cancel(CSession2* aSession);

	TUint SwpKey() const;
	TBool IsIdle() const;
protected: 
	//from CActive
	void DoCancel();
	void RunL();
	TInt RunError(TInt aError);

private:
	// methods
	CSsmSwpIndividualTransitionScheduler(TUint aSwpKey, CSsmSwpTransitionScheduler& aParent); // only accessible from factory methods
	void ConstructL(MCleSessionProxy* aCleSession);
	void DoSubmitL(CSsmSwpTransitionEngine* aEngine);
	void ScheduleTransition();
	CSsmSwpTransitionEngine* GetNextFromQueue();
	void DestroyTransitionQueue();
	void CancelTransitionQueue(CSession2* aSession);
	TBool CurrentTransitionOriginatesFrom(CSession2* aSession);

	void NotifyTransitionQueued();
	void NotifyTransitionStarted();
	void NotifyTransitionCompleted();
	void NotifyTransitionFailed();

private:
	// constant types
	enum TSchedulerStates
		{
		EIdle,
		ERunningTransition
		};

private:
	// members
	RPointerArray<CSsmSwpTransitionEngine> iTransitionQueue;
	CSsmSwpTransitionEngine* iCurrentTransition;
	TSchedulerStates iSchedulerState;
	CSession2* iCancellingSession;

	MCleSessionProxy* iCleSession;
	CSsmSwpPolicyCliSession* iSsmSwpPolicySession;
	
	TUint iSwpKey;
	CSsmSwpTransitionScheduler& iParent;
	};

#endif
