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
// Description: Stores information for a test sequence
// 
//

#ifndef __CT_LBS_TEST_SESSION_SEQUENCE_H__
#define __CT_LBS_TEST_SESSION_SEQUENCE_H__

#include <e32base.h>
#include <lbs.h>
#include <lbscommon.h>
#include <lbs/lbsloccommon.h>
#include <lbs/lbsnetcommon.h>
#include <lbs/lbsnetprotocolbase.h>
#include <lbs/lbsassistancedatabuilderset.h>
#include <iniparser.h>


#include "clbstestngmessagehandler.h"
#include "ctlbshybridmultipledefs.h"
#include "argutils.h"

/** Class for representing a session sequence
*/
class CTestSessionSequence : public CBase
{
public:
	static CTestSessionSequence* NewL(CLbsTestNgMessageHandler& aNgProxy, MHybridMultipleTestCallBack& aTestCallback,
			TInt aSequenceNum, const TDesC& aConfigFileName, const TDesC& aConfigSection);
	~CTestSessionSequence();
	
	//Test Getters
	TLbsNetSessionId GetSessionId();
	TInt GetPmId();
	
	//Sequence Position Operators
	TInt GetCurrentPosition();
	TInt GetStartPosition();
	
	//Returns the current action of the sequence
	TInt GetCurrentAction();
	
	//Prints out the remaining actions in the sequence to the testexecute log
	void PrintRemainingActions();
	
	//CIdle Functions
	void SignalCallbackIdleStart();
	
	//NG Message Checkers
	virtual void CheckRespondPrivacyRequest(CLbsNetworkProtocolBase::TLbsPrivacyResponse aPrivacyResponse);
	virtual void CheckSelfLocationRequest(TLbsNetSessionId* aSessionId);
	virtual void CheckNetworkLocationRequest(TLbsNetSessionId* aSessionId);
	virtual void CheckRequestTransmitLocation(TLbsNetSessionId* aSessionId, TInt aPriority, const TDesC16& aThirdParty);
	
protected:
	//Main test function, works out what needs to be done with the current sequence state
	void HandleMessage();
	
	//Allows the test to increment the position in the sequence
	void IncrementPosition();
	
	//NG Message Handlers
	virtual void SendProcessPrivacyRequest();
	virtual void SendProcessStatusUpdate();
	virtual void SendProcessLocationUpdate();
	virtual void SendProcessAssistanceData();
	virtual void SendProcessLocationRequest();
	virtual void SendProcessSessionComplete(TInt aReason = KErrNone);
	
	//Used to tell the sequence to wait for a NG message
	void WaitForMessage();
	
	//MOLR Handler Functions
	virtual void RequestLocationUpdate();
	
	//X3P Handler Functions
	virtual void TransmitPosition();
	
	//Log statement to the testexecute log (all log statements take one string and one number)
	void LogTestStatement(const TDesC& aMessage, TInt aCount, ...);
	
protected:
	CTestSessionSequence(CLbsTestNgMessageHandler& aNgProxy, MHybridMultipleTestCallBack& aTestCallback);
	void ConstructL(TInt aSequenceNum, const TDesC& aConfigFileName, const TDesC& aConfigSection);
	
private:
	//CIdle Callback Functions
	static TInt HandlerCompleteCallback(TAny* aAny);
	void HandlerCompleteNotify();
	
protected:
	//ID of this sequence
	TInt iSequenceId;
	//ID of the PM this sequence belongs to
	TInt iPmId;
	//Session ID of the sequence
	TLbsNetSessionId iSessionId;
	//State Sequence
	RArray<TInt> iStateSequence;
	//Current Position in the sequence
	TInt iCurrentPosition;
	//Start position; when the sequence should be started
	TInt iStartPosition;
	//Reference to the NG Message Proxy, allowing these sequences to directly send messages
	CLbsTestNgMessageHandler& iNgProxy;
	//Test Framework callback
	MHybridMultipleTestCallBack& iTestCallback;
	//Idle callback used to process the next message in the sequence
	CIdle* iCallbackIdle;
	//Ini file used to read in the testdata config
	CIniData* iIniFile;
};

#endif	//__CT_LBS_TEST_SESSION_SEQUENCE_H__
