// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file The TEFUnit header file which tests the Sim
*/

/**
 @file
 @internalAll 
*/

#ifndef CCTSYSATNEGATIVEFU_H_
#define CCTSYSATNEGATIVEFU_H_

#include <test/tefunit.h>

#include <etelmm.h>
#include <etelmmcs.h>
#include <etelsat.h>

#include "cctsycomponenttestbase.h"

class RSat;

class CCTsySatFUNegative : public CCtsyComponentTestBase
	{
public:
	// Create a suite of all the tests
	static CTestSuite* CreateSuiteL(const TDesC& aName);
	~CCTsySatFUNegative();

public:
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
	void TestUnit0013L();
	void TestUnit0014L();


private:
	void OpenSatL(TUint8 aEnabled = 0x00);
	void SetupEventListL(TUint8 aPCommandNumber, TUint8 aEvent);
	void SetupLocalInfoL(TUint16 aAreaCode, TUint16 aCellId, const TDesC8& aOperatorCode, TUint8 aLocationStatus);
	void GenerateAndWaitForTermRspL(TUint8 aPCmdNumber,TUint8 aTypeOfCommand, TUint8 aCommandQualifier, RSat::TPCmd aRSatTypeOfCommand, RSat::TPCmdResult aGeneralResult, const TDesC8& aRsp, TInt aCompError);
	void GenerateAndWaitForTermRspL(TUint8 aPCmdNumber,TUint8 aTypeOfCommand, TUint8 aCommandQualifier, RSat::TPCmd aRSatTypeOfCommand, RSat::TPCmdResult aGeneralResult, const TDesC8& aGeneralResultAddInfo, const TDesC8& aRsp, const TDesC8& aAdditionalInfo, TInt aCompError);
	void GenerateTermRspTlv(RBuf8& buf, TUint8 aPCmdNumber,TUint8 aTypeOfCommand, TUint8 aCommandQualifier,RSat::TPCmdResult aResult, const TDesC8& aGeneralResultAddInfo, const TDesC8& aAdditionalInfo);
	void RefreshNoTerminalRspL(TUint8 aPCmdNumber, TUint8 aCommandQualifier, TInt aRefreshAllowedCompleteResult);
	TUint8 TonAndNpi(RMobilePhone::TMobileTON aTon, RMobilePhone::TMobileNPI aNpi);
	void DoCleanup();

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
		
		};
	
private:
	RSat iSat;
	CCallTypeContainer* iCallContainer;
	}; // class CCTsySatFUNegative


#endif /* CCTSYSATNEGATIVEFU_H_ */
