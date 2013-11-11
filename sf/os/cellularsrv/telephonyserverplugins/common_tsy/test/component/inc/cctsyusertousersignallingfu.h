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
// The TEFUnit header file which tests the UserToUserSignalling
// functional unit of the Common TSY.
// 
//

#ifndef CCTSYUSERTOUSERSIGNALLINGFU_H
#define CCTSYUSERTOUSERSIGNALLINGFU_H

#include <test/tefunit.h>

#include <etelmm.h>
#include <etelmmcs.h>

#include "cctsycomponenttestbase.h"

class CCTsyUserToUserSignallingFU : public CCtsyComponentTestBase
	{
public:
	// Create a suite of all the tests
	static CTestSuite* CreateSuiteL(const TDesC& aName);

public:
	// Individual test steps

	void TestGetUUSCaps0001L();
	void TestGetUUSCaps0002L();
	void TestGetUUSCaps0003L();
	void TestReceiveUUI0001L();
	void TestReceiveUUI0001bL();
	void TestReceiveUUI0002L();
	void TestReceiveUUI0003L();
	void TestActivateUUS0001L();
	void TestActivateUUS0002L();
	void TestActivateUUS0003L();
	void TestHangupWithUUI0001L();
	void TestHangupWithUUI0002L();
	void TestHangupWithUUI0003L();
	void TestNotifyUUSCapsChange0001L();
	void TestNotifyUUSCapsChange0002L();
	void TestNotifyUUSCapsChange0003L();
	void TestAnswerIncomingCallWithUUI0001L();
	void TestAnswerIncomingCallWithUUI0002L();
	void TestAnswerIncomingCallWithUUI0003L();
	void TestSendUUI0001L();
	void TestSendUUI0002L();
	void TestSendUUI0003L();
	void TestGetUUSSetting0001L();
	void TestSetUUSSetting0001L();
	void TestNotifyUUSSettingChange0001L();


private:

	void AuxGetUUSCapsL(const TDesC& aLineName);
	void AuxActivateUUSDataAndFaxL(const TDesC& aLineName);
	void AuxHangupWithUUIL(const TDesC& aLineName);
	void AuxNotifyUUSCapsChangeL(const TDesC& aLineName);
	void AuxAnswerIncomingCallWithUUIL(const TDesC& aLineName);
	void AuxSendUUIL(const TDesC& aLineName);
	void AuxReceiveUUIDataAndFaxL(const TDesC& aLineName);
	void AuxReceiveUUIVoiceL(TBool aWaitForComplete = EFalse);

	void OpenNewLineLC(RLine& aLine, const TDesC& aLineName);
	void OpenNewCallLC(RMobileCall& aCall, RLine& aLine, TDes& aCallName);
	void CreateAndOpenCallForVoiceLC( RLine& aLine, RCall& aCall, 
									TInt aCallId, TName& aCallName,
									RMobilePhone::TMobileService& aMobileService,
									RMobileCall::TMobileCallStatus aMobileCallStatus = RMobileCall::EStatusRinging);

	}; // class CCTsyUserToUserSignallingFU

#endif // CCTSYUSERTOUSERSIGNALLINGFU_H

