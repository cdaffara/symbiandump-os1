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
// The TEFUnit header file which tests the DTMF
// functional unit of the Common TSY.
// 
//

#ifndef CCTSYDTMFFU_H
#define CCTSYDTMFFU_H

#include <test/tefunit.h>

#include <etelmm.h>
#include <etelmmcs.h>

#include "cctsycomponenttestbase.h"

class CCTsyDTMFFU : public CCtsyComponentTestBase
	{
public:
	// Create a suite of all the tests
	static CTestSuite* CreateSuiteL(const TDesC& aName);

public:
	// Individual test steps

	void TestGetDTMFCaps0001L();
	void TestGetDTMFCaps0006L();
	void TestGetDTMFCaps00011L();
	void TestStopDTMFTone0001L();
	void TestReadDTMFTones0001L();
	void TestSendDTMFTones0001L();
	void TestSendDTMFTones0002L();
	void TestSendDTMFTones0003L();
	void TestSendDTMFTones0004L();
	void TestSendDTMFTones0005L();
	void TestStartDTMFTone0001L();
	void TestNotifyDTMFEvent0001L();
	void TestNotifyDTMFCapsChange0001L();
	void TestNotifyDTMFCapsChange0002L();
	void TestNotifyDTMFCapsChange0004L();
	void TestNotifyStopInDTMFString0001L();
	void TestNotifyStopInDTMFString0002L();
	void TestNotifyStopInDTMFString0004L();
	void TestContinueDTMFStringSending0001L();
	void TestContinueDTMFStringSending0005L();


private:

	void AuxTestGetDTMFCaps0001L(
			RMobilePhone::TMobileService aMobileService,
			const TDesC& aLineName,
			TUint32 aExpCaps);

	}; // class CCTsyDTMFFU

#endif // CCTSYDTMFFU_H

