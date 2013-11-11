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
// The TEFUnit header file which tests the CallControl
// functional unit of the Common TSY.
// 
//

#ifndef CCTSYCALLCONTROLFU_H
#define CCTSYCALLCONTROLFU_H

#include <test/tefunit.h>

#include <etelmm.h>
#include <etelmmcs.h>

#include "cctsycomponenttestbase.h"

class CCTsyCallControlFU : public CCtsyComponentTestBase
	{
public:
	// Create a suite of all the tests
	static CTestSuite* CreateSuiteL(const TDesC& aName);

public:
	// Individual test steps

	void TestAnswerIncomingCall0001L();
	void TestAnswerIncomingCall0002L();
	void TestAnswerIncomingCall0004L();
	void TestAnswerIncomingCall0005L();
	void TestAnswerIncomingCall0006L();
	void TestAnswerIncomingCall0007L();
	void TestAnswerIncomingCall0008L();
	void TestAnswerIncomingCall0009L();
	void TestAnswerIncomingCall00010L();
	void TestAnswerIncomingCall00011L();
	void TestDial0001L();
    void TestDial0001bL();
    void TestDial0001cL();
	void TestDial0002L();
	void TestDial0003L();
	void TestDial0004L();
	void TestDial0005L();
	void TestDial0006L();
	void TestDial0006bL();
	void TestDial0006cL();
	void TestDial0006dL();
	void TestDial0006eL();
	void TestDial0006fL();
	void TestDial0006gL();
	void TestDial0006hL();
	void TestDial0006iL();
	void TestDial0007L();
	void TestDial0008L();
	void TestDial0009L();
	void TestDial0009bL();
	void TestDial0009cL();
	void TestDial00010L();
	void TestDial00011L();
	void TestHangUp0001L();
	void TestHangUp0002L();
	void TestHangUp0004L();
	void TestHangUp0005L();
	void TestHangUp0006L();
	void TestHangUp0007L();
	void TestHangUp0009L();
	void TestHangUp00010L();
	void TestHangUp00011L();
	void TestHangUp00011bL();
	void TestHangUp00011cL();
	void TestHangUp00012L();
	void TestHangUp00014L();
	void TestHangUp00015L();
	void TestHold0001L();
	void TestHold0002L();
	void TestHold0004L();
	void TestHold0005L();
	void TestHold0006L();
	void TestHold00011L();
	void TestHold0001aL();
	void TestSwap0001L();
	void TestSwap0002L();
	void TestSwap0004L();
	void TestSwap0005L();
	void TestSwap0006L();
	void TestSwap00011L();
	void TestResume0001L();
	void TestResume0002L();
	void TestResume0004L();
	void TestResume0005L();
	void TestResume0006L();
	void TestResume00011L();
	void TestDeflect0001L();
	void TestDeflect0002L();
	void TestDeflect0003L();
	void TestTransfer0001L();
	void TestTransfer0002L();
	void TestTransfer0004L();
	void TestTransfer0005L();
	void TestTransfer0006L();
	void TestTransfer0009L();
	void TestTransfer00011L();
	void TestTerminateAllCalls0001L();
	void TestTerminateAllCalls0002L();
	void TestTerminateAllCalls0004L();
	void TestTerminateAllCalls0005L();
    void TestTerminateActiveCalls0001L();
    void TestTerminateActiveCalls0001bL();
    void TestTerminateActiveCalls0001cL();
    void TestTerminateActiveCalls0002L();
    void TestTerminateActiveCalls0004L();
    void TestTerminateActiveCalls0005L();
	void TestSetIncomingCallType0001L();
	void TestAnswerIncomingCallISV0001L();
	void TestAnswerIncomingCallISV0002L();
	void TestAnswerIncomingCallISV0004L();
	void TestAnswerIncomingCallISV0005L();
	void TestAnswerIncomingCallISV0006L();
	void TestAnswerIncomingCallISV00011L();
	void TestDialISV0001L();
	void TestDialISV0006L();
	void TestDialISV00011L();
	void TestDialCallControl0001L();
	void TestDialCallControl0006L();
	void TestDialCallControl00011L();
	void TestEnumerateCall0001L();
	void TestEnumerateCall0006L();
	void TestEnumerateCall00011L();
	void TestNotifyHookChange0001L();
	void TestNotifyHookChange0006L();
	void TestNotifyHookChange00011L();
	void TestCompleteNotifyStatusChange0001L();
	void TestUpdateLifeTime0001L();	

private:

	TInt OpenNewLineLC(RMobilePhone& aPhone, RLine& aLine, const TDesC& aLineName);
	TInt CreateIncomingCallL(RLine& aLine, const TInt aCallId, 
                            TName& aIncomingCallName,
                            RMobilePhone::TMobileService aMobileService,
                            RMobileCall::TMobileCallStatus aMobileCallStatus);
	void CloseIncomingCallL(RCall& aCall, const TInt aCallId, RMobilePhone::TMobileService aMobileService = RMobilePhone::EVoiceService);
	void CallGetMobileCallInfoL(TInt aCallId, 
	                           RMobilePhone::TMobileService aService, 
    	                       const TDesC &aTelNumber);
	
	void AuxEnumerateCallL(const TDesC& aName);
	void AuxNotifyHookChangeL(const TDesC& aName);
	void AuxCompleteNotifyStatusChangeL(const TDesC& aName);
	
	TInt TestAnsweringFailsBecauseHoldDoesNotWorkL( const TDesC& aLineName );
	
	void DialDataCallAndCancelL(RLine& aLine,const TDesC8& aCallData,RBuf8& expectData,RBuf8& completeData);
	
  
	}; // class CCTsyCallControlFU

#endif // CCTSYCALLCONTROLFU_H

