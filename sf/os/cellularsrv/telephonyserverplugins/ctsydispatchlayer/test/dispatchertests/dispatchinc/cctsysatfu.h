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
// functional unit of the Common TSY Dispatcher.
//



/**
 @file The TEFUnit header file which tests the Sat
*/

/**
 @file
 @internalAll 
*/

#ifndef CCTSYSATFU_H
#define CCTSYSATFU_H

#include <test/tefunit.h>

#include <etelmm.h>
#include <etelmmcs.h>
#include <etelsat.h>

#include "cctsycomponenttestbase.h"


class CCTsySatFU : public CCtsyComponentTestBase
	{
public:
	// Create a suite of all the tests
	static CTestSuite* CreateSuiteL(const TDesC& aName);

public:
	// use-case tests
	void TestUseCase0001L();
	void TestUseCase0002L();
	void TestUseCase0003L();
	void TestUseCase0004L();
	void TestUseCase0005L();
	void TestUseCase0006L();
	void TestUseCase0007L();
	void TestUseCase0008L();
	void TestUseCase0009L();
	void TestUseCase0010L();
	
	//other unit tests
	void TestUnit0001L();
	void TestUnit0002L();
	void TestUnit0003L();
	void TestUnit0004L();
	void TestUnit0005L();
	void TestUnit0006L();
	void TestUnit0007L();
	void TestUnit0008L();
	void TestUnit0009L();
	void TestUnit0010L();
	void TestUnit0011L();
	void TestUnit0012L();
	void TestUnit0013L();
	void TestUnit0014L();
	void TestUnit0015L();
	void TestUnit0016L();
	void TestUnit0017L();
		
private:
	//helper class to hold large call control T-Types, this is to prevent tests failing due
	// to test execute creating the threads for each TEST STEP with insufficient stack 
	class CCallTypeContainer : public CBase
		{
	public:
		
		CCallTypeContainer();
		
		RSat::TCallControlV6 iCallControl;
		RSat::TCallControlV6Pckg iCallControlPckg;
		RSat::TSetUpCallV6 iSetUpCall;
		RSat::TSetUpCallV6Pckg iSetUpCallPckg;
		RSat::TAlphaId iAlphaId;
		
		};
	
private:
	void OpenSatL(TUint8 aEnabled = 0x00);
	void SetupEventListL(TUint8 aPCommandNumber, TUint8 aEvent);
	void SetupLocalInfoL(TUint16 aAreaCode, TUint16 aCellId, const TDesC8& aOperatorCode, TUint8 aLocationStatus);
	void WaitForEventDownloadL(RSat::TEventList aEventList, const TDesC8& aEventDownloadEnvelope);
	void GenerateAndWaitForTermRspL(TUint8 aPCmdNumber,TUint8 aTypeOfCommand, TUint8 aCommandQualifier, RSat::TPCmd aRSatTypeOfCommand, RSat::TPCmdResult aGeneralResult, const TDesC8& aRsp, TInt aCompError);
	void GenerateAndWaitForTermRspL(TUint8 aPCmdNumber,TUint8 aTypeOfCommand, TUint8 aCommandQualifier, RSat::TPCmd aRSatTypeOfCommand, RSat::TPCmdResult aGeneralResult, const TDesC8& aGeneralResultAddInfo, const TDesC8& aRsp, const TDesC8& aAdditionalInfo, TInt aCompError);
	void GenerateTermRspTlv(RBuf8& buf, TUint8 aPCmdNumber,TUint8 aTypeOfCommand, TUint8 aCommandQualifier,RSat::TPCmdResult aResult, const TDesC8& aGeneralResultAddInfo, const TDesC8& aAdditionalInfo);
	void RefreshNoTerminalRspL(TUint8 aPCmdNumber, TUint8 aCommandQualifier, TInt aRefreshAllowedCompleteResult);
	TUint8 TonAndNpi(RMobilePhone::TMobileTON aTon, RMobilePhone::TMobileNPI aNpi);

	void DoCleanup();
private:
	RSat iSat;
	
	CCallTypeContainer* iCallContainer;

	}; // class CCTsySatFU

#endif // CCTSYSATFU_H

