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
// This is the base class for the integration and state transition SUPL Push API unit test steps
// 
//

#if (!defined __TE_LBSSUPLPUSHBASE_STEP__)
#define __TE_LBSSUPLPUSHBASE_STEP__

#include <test/testexecutestepbase.h>
#include <lbs/lbssuplpushcommon.h>




/**
The base class for the integration and state transition SUPL Push API unit test steps.
It provides better support for asynchronous functionality and memory leaks check.
*/
class CTe_LbsSuplPushBaseStep : public CTestStep
	{
private:
	/**
	The main active object in the RunL method of which the test code is run asynchronously.
	
	@see CTe_LbsSuplPushBaseStep::ReallyDoTestStepL
	*/
	class CMainActive : public CActive
		{
	public:
		CMainActive(CTe_LbsSuplPushBaseStep& aStep);
		virtual ~CMainActive();
		
		void Start();
		
		TVerdict Verdict();
		
		
	protected:
		virtual void RunL();
		virtual void DoCancel();
		
	private:
		/** A reference on the object-owner */
		CTe_LbsSuplPushBaseStep& iStep;
		/** The result of the test step */
		TVerdict iVerdict;
		};

/** To give it opportunity to run the protected ReallyDoTestStepL method. */
friend class CMainActive;		
	
public:
	virtual ~CTe_LbsSuplPushBaseStep();
	
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();
	
	TUint BusyPropKey();
	TUint AckPropKey();
	TUid PropOwnerSecureId();
	
	inline CActiveSchedulerWait& ActiveSchedulerWait();
	
protected:
	/** The replacement for doTestStepL. The test code itself must be placed here.*/
	virtual void ReallyDoTestStepL() = 0;
	
protected:
	/** The active scheduler created, installed and started for the test step.*/
	CActiveScheduler* iScheduler;
	/** The main active object in the RunL method of which the test code is run asynchronously. */
	CMainActive* iMainActive;
	/** The CActiveSchedulerWait object used to convert async calls to sync. */
	CActiveSchedulerWait* iSchedulerWait;
	
	/** The channel to use for tests */
	TLbsSuplPushChannel iChannel;
	
	/** The BUSY property key */
	TUint iBusyPropKey;
	
	/** The ACK property key */
	TUint iAckPropKey;
			
	/** The security id of the process that defines the P&S props used*/
	TUid iPropOwnerSecureId;
	
	};

/**
Returns a reference on the CActiveSchedulerWait object.

@return A reference on the CActiveSchedulerWait object.
*/
inline CActiveSchedulerWait& CTe_LbsSuplPushBaseStep::ActiveSchedulerWait()
	{
	return *iSchedulerWait;
	}

#endif //__TE_LBSSUPLPUSHBASE_STEP__
