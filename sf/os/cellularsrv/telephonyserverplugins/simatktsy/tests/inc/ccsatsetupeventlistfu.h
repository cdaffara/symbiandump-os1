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
 @file The TEFUnit header file which tests the SetUpEventList
*/

#ifndef CCSATSETUPEVENTLISTFU_H
#define CCSATSETUPEVENTLISTFU_H

#include <test/tmockltsydata.h>

#include "ccsatcomponenttestbase.h"

class CCSatSetUpEventListFU : public CCSatComponentTestBase
	{
public:
	// Create a suite of all the tests
	static CTestSuite* CreateSuiteL(const TDesC& aName);

public:
	// Individual test steps

	void TestNotifySetUpEventListPCmd0001L();
	void TestNotifySetUpEventListPCmd0002L();
	void TestNotifySetUpEventListPCmd0004L();
	void TestEventDownload0001L();	
	
	void TestNotifySetUpEventListPCmd0001bL();
	void TestNotifySetUpEventListPCmd0001cL();
	void TestNotifySetUpEventListPCmd0001dL();
	void TestNotifySetUpEventListPCmd0001eL();
	void TestNotifySetUpEventListPCmd0001fL();
	void TestNotifySetUpEventListPCmd0001gL();
	void TestNotifySetUpEventListPCmd0001hL();
	void TestNotifySetUpEventListPCmd0001iL();
	void TestNotifySetUpEventListPCmd0001jL();

private:
	void PrepareMockLDataWithExpL(TUint8 aCommand);
		
	static void PrepareTlv( TTlv& aTlv, 
							const TUint8* aEventList, 
							TUint8 aNumberOfEvents);	
	
	void CompleteMockDispatchSatPcmdL( TPtrC8 aCompleteTlv, TInt aError );		
	
	void PrepareTerminalResponseMockDataWithExpL(TUint8 aPCmdNumber,		
												RSat::TPCmdResult aGeneralResult,
												const TDesC8& aResultAddInfo,
												TInt aErrorForExpect = KErrNone);	

	void TerminalResponseL(	const TDesC8& aRspPckg, TInt aExpectedError = KErrNone);
	
	void PrepareAndCallEventDownloadL(const TUint8 aEvent);	
	
	void PrepareAndCallEventDownloadL(const TUint8 aEvent,									
									const RSat* aClientSat,
									TBool aIsUnsolicited,
									TInt aErrorForExpect);
	
	
	
	void EnvelopeFromTsyL(const TUint8 aEvent, TBool aIsEnvelopeSent = ETrue);
	
	void CompleteMTCallL(TUint8 aTransactionId, 
						TDesC8* aAddress,
						TDesC8* aSubAddress,
						TBool aIsEnvelopeSent = ETrue);
	
	void CompleteCallConnectedL(TUint8 aTransactionId,
								TBool aNearEnd,
								TBool aIsEnvelopeSent);
	
	void CompleteCallDisconnectedL(TUint8 aTransactionId, 
									TBool aNearEnd,
									TDesC8* aCause, 
									TBool aIsEnvelopeSent);
	
	void CompleteLocationStatusL(TUint8 aLocationStatus,
								TDes8& 	aCountryCode,
								TUint16 aLocationAreaCode,
								TUint16 aCellId,
								TBool aIsEnvelopeSent);
	
	void CompleteAccessTechnologyChangeL(TUint8 aAccTechChange,
										TBool aIsEnvelopeSent);
	
	void DoTestTerminalResponseL();	
	
	
	}; // class CCSatSetUpEventListFU

#endif // CCSATSETUPEVENTLISTFU_H

