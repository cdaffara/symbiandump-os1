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
 @file The TEFUnit header file which tests the SendUSSD
*/

#ifndef CCSATSENDUSSDFU_H
#define CCSATSENDUSSDFU_H

#include <test/tefunit.h>

#include <etelmm.h>
#include <etelmmcs.h>

#include "ccsatcomponenttestbase.h"

class CCSatSendUSSDFU : public CCSatComponentTestBase
	{
public:
	// Create a suite of all the tests
	static CTestSuite* CreateSuiteL(const TDesC& aName);

public:
	// Individual test steps

	void TestNotifySendUssdPCmd0001L();
	void TestNotifySendUssdPCmd0002L();	
	void TestNotifySendUssdPCmd0004L();
	
	void TestNotifySendUssdPCmd0001bL();
	void TestNotifySendUssdPCmd0001cL();
	void TestNotifySendUssdPCmd0001dL();
	void TestNotifySendUssdPCmd0001eL();
	void TestNotifySendUssdPCmd0001fL();
	void TestNotifySendUssdPCmd0001gL();
	void TestNotifySendUssdPCmd0001hL();
	void TestNotifySendUssdPCmd0001iL();
	void TestNotifySendUssdPCmd0001jL();	
	void TestNotifySendUssdPCmd0001kL();
	
private:
	
	void PrepareMockLDataWithExpL(TUint8 aPCmdNumber);
		
	static void PrepareTlv( TTlv& aTlv, 
							TUint8 aDataCodingScheme, 
							const TDesC8& aUssdString,
							const TDesC8& aAlphaIdentifier, 
							TBool aIsAlphaIdentifierUsed = ETrue);	
	
	static void PrepareTlvWith7BitStr( TTlv& aTlv, 						
							const TDesC8& aAlphaIdentifier, 
							TBool aIsAlphaIdentifierUsed = ETrue);	

	void CompleteMockDispatchSatPcmdL( TPtrC8 aCompleteTlv, TInt aError );		
	
	void PrepareTerminalResponseMockDataWithExpL(TUint8 aPCmdNumber,																								
												TUint8 aDataCodingScheme, 	
												const TDesC8& aTextString,												
												RSat::TPCmdResult aGeneralResult = RSat::KSuccess,
												TUint8 aResultAddInfo = 0, 
												TBool  aIsResultAddInfoUsed = ETrue,
												TInt aErrorForExpect = KErrNone,
												TBool aUseEmptyText = EFalse);	

	void TerminalResponseL(	const TDesC8& aRspPckg, TInt aExpectedError = KErrNone);
	
	void DoTestTerminalResponseL();
	
	}; // class CCSatSendUSSDFU

#endif // CCSATSENDUSSDFU_H


