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
// functional unit of the SAT.
//



/**
 @file The TEFUnit header file which tests the ProvideLocalInfo
*/

#ifndef CCSATPROVIDELOCALINFOFU_H
#define CCSATPROVIDELOCALINFOFU_H

#include <test/tefunit.h>

#include <etelmm.h>
#include <etelmmcs.h>

#include "ccsatcomponenttestbase.h"

class CCSatProvideLocalInfoFU : public CCSatComponentTestBase
	{
public:
	// Create a suite of all the tests
	static CTestSuite* CreateSuiteL(const TDesC& aName);
	
public:
	// Test API
	void TestNotifyLocalInfoPCmd0001L();
	void TestNotifyLocalInfoPCmd0002L();	
	void TestNotifyLocalInfoPCmd0004L();	
		
	void TestNotifyLocalInfoPCmd0001bL();	
	void TestNotifyLocalInfoPCmd0001cL();	
	void TestNotifyLocalInfoPCmd0001dL();
	void TestNotifyLocalInfoPCmd0001eL();
	void TestNotifyLocalInfoPCmd0001fL();
	void TestNotifyLocalInfoPCmd0001gL();
	void TestNotifyLocalInfoPCmd0001hL();
	void TestNotifyLocalInfoPCmd0001iL();
	void TestNotifyLocalInfoPCmd0001jL();
	void TestNotifyLocalInfoPCmd0001kL();

private:	
	
	void PrepareMockLDataWithExpL(TUint8 aPCmdNumber);	
	void PrepareTlv(TTlv& aTlv, TUint8 aPCmdNumber, TUint8 aLocalInfoQualifier);		
	void CompleteMockDispatchSatPcmdL( TPtrC8 aCompleteTlv, TInt aError );	
	void PrepareTerminalResponseMockDataWithExpL (	TUint8 aPCmdNumber,												
												TUint8 aCommandQualifier,
												const TDesC& aGeneralResultAddInfo,
												RSat::TPCmdResult aGeneralResult,
												const TDesC8& aAdditionalInfoTlv);		
	void TerminalResponseL(	const TDesC8& aRspPckg, TInt aExpectedError = KErrNone);	
	
	void SetDateTimeTimeZone ( RSat::TLocalInfoRspV3& aLocalInfoRspV3, TUint8 aTimeZone);	
	
	void OpenSatWithZeroImeiL();
	}; // class CCSatProvideLocalInfoFU

#endif // CCSATPROVIDELOCALINFOFU_H

