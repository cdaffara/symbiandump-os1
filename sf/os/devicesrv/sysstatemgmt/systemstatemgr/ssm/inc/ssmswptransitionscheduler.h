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

#ifndef __SSMSWPTRANSITIONSCHEDULER_H__
#define __SSMSWPTRANSITIONSCHEDULER_H__

#include <e32base.h>

class CSsmSwpTransitionEngine;
class CSsmSwpIndividualTransitionScheduler;

/**
This helper Interface can be used for testing or to monitor the progress of the tranisitons
by wrapping the scheduler and registering the wrapper class as a monitor

@internalComponent
@released
*/
class MSwpSchedulerMonitor
	{
public:
	virtual void SwpTransitionQueued() = 0;
	virtual void SwpTransitionStarted() = 0;
	virtual void SwpTransitionCompleted(TUint numberCompleted) = 0;
	virtual void SwpTransitionFailed(TUint numberFailed) = 0;
	};

/**
 The scheduler class is responsible for submitting and scheduling transitions sequencially
 If  transition is active, the scheduler will queue the transition until all active 
 and queued transitions have been completed.
 
 @internalComponent
 @released
 */
class CSsmSwpTransitionScheduler : public CTimer
	{
	// Allow the individual transition schedulers to interact more
	// closely with this class
	friend class CSsmSwpIndividualTransitionScheduler;
	
public:
	// methods
	static CSsmSwpTransitionScheduler* NewL();
	static CSsmSwpTransitionScheduler* NewLC();
	~CSsmSwpTransitionScheduler();

	void SubmitL(CSsmSwpTransitionEngine* aEngine); // takes ownership
	void Cancel(CSession2* aSession);
		
	void SetTransactionMonitor(MSwpSchedulerMonitor* aMonitor)
		{
		iTransactionMonitor = aMonitor;
		}
	//used only for testing
#ifdef _DEBUG
	void CleanupIndividualSchedulerArray();
#endif

protected:
	// From CActive
	void RunL();
	TInt RunError(TInt aError);

private:
	// methods
	CSsmSwpTransitionScheduler(); // only accessible from factory methods
	void ConstructL();

	void NotifyTransitionQueued();
	void NotifyTransitionStarted();
	void NotifyTransitionCompleted();
	void NotifyTransitionFailed();
	
	CSsmSwpIndividualTransitionScheduler* FindScheduler(CSsmSwpTransitionEngine* aEngine);

private:
	// members
	RPointerArray<CSsmSwpIndividualTransitionScheduler> iIndividualTransitionSchedulers;

	TLinearOrder<CSsmSwpIndividualTransitionScheduler> iLinearOrder;
	
	MSwpSchedulerMonitor* iTransactionMonitor;
	TUint iNumberOfCompletedTransitions;
	TUint iNumberOfFailedTransitions;
	};

#endif
