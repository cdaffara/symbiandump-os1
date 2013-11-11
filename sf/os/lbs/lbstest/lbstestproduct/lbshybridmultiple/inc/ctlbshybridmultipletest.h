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
// Runs the actual multiple test step
// 
//

#ifndef __CT_LBS_HYBRID_MULTIPLE_H__
#define __CT_LBS_HYBRID_MULTIPLE_H__

// LBS includes.
#include <lbs.h>
#include <lbscommon.h>
#include <lbs/lbsnetcommon.h>
#include <lbs/lbsprivacycontroller.h>

// LBS test includes.
#include "ctlbshybridmultiplestep.h"
#include "clbstestngmessagehandler.h"

// Literals used
_LIT(KLbsHybridMultipleTest,"LbsHybridMultipleTest");

/** Inherits from the main Multiple Test Step.  It actually runs the test and contains all the
 *   callback implementations.
 */
class CT_LbsHybridMultipleTest: public CT_LbsHybridMultipleStep, public MLbsPrivacyObserver
{
public:
    static CT_LbsHybridMultipleTest* New(CT_LbsHybridMultipleServer& aParent);
    virtual TVerdict doTestStepL();
    ~CT_LbsHybridMultipleTest();

protected:
	//from MLbsPrivacyObserver
	void ProcessNetworkLocationRequest(TUint aRequestId, const TLbsExternalRequestInfo& aRequestInfo, const TNotificationType& aNotificationType);
	void ProcessNetworkPositionUpdate(TUint aRequestId, const TPositionInfo& aPosInfo);
	void ProcessRequestComplete(TUint aRequestId, TInt aReason);
	
	//from MLbsTestNgMessageHandler
	virtual void OnHandleNGMessage(TUint aPmId, TInt aMessageType);
	virtual void LogTestStatement(const TDesC& aMessage);
	virtual void SequenceFinished();
	virtual void SignalCheckForNewSequences();
	
	//Handle call back methods used for checking for new sequences
	static TInt HandlerCompleteCallback(TAny* aAny);
	void HandlerCompleteNotify();
	
	//Called to stop the test
	virtual void StopTest(TInt aError);
	
	//Used to check to see whether the message was expected by any of the sequences
	// The two different functions are required, as on some NG Messages there is no SessionID
	virtual TInt WasMessageExpectedSessionId(TInt aSessionId, TNetProtocolResponseType aMessageType);
	virtual TInt WasMessageExpectedPmId(TInt aPmId, TNetProtocolResponseType aMessageType);
	
	//Used to check to see if any new sequences need to be started
	virtual void CheckForNewSequenceToStart();
	
protected:
	CT_LbsHybridMultipleTest(CT_LbsHybridMultipleServer& aParent);
    void ConstructL();

private:
	//Contains the number of currently active sequences.  When this reaches 0, the test will end.
	// This contains the total number of sequences from all Protocol Modules
	TInt iActiveSequences;
	
	//LBS Privacy Controller
	CLbsPrivacyController* iController;
	
	//Idle call back handler used to check to see if any other sequences should be started
	CIdle* iCheckNewSequenceIdle;
	
	//Allows the test to know whether it should check for new sequences to start
	TBool iCheckForNewSequencesToStart;
};

#endif //__CT_LBS_HYBRID_MULTIPLE_H__
