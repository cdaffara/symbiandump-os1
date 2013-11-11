// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This file initiates the "Data Call Test" scenario for the :q
// Test Harness.
// This file handles both the test side and the emulator side of the GSM
// Test Harness for this scenario.  The test side must start the emulator and 
// drive the ETel API.  The emulator side runs the designated script.
// 
//

/**
 @file
 @note  There are mulitple classes implemented in this file.
 @note  These classes are CTestDriveDataCall and CTestDataCall.
*/

#include <e32test.h>
#include <etelmm.h>
#include "Te_LoopBackcdatacall.h"
#include "../../hayes/TSYCONFG.H"	// for KInternetAccessPoint

//
// Test-side class
// With the assistance of the base class, this class must start the emulator
// and drive the ETel API.
//
CTestDriveDataCall* CTestDriveDataCall::NewL(const TScriptList aScriptListEntry)
/**
 * 2 Phase Constructor
 *
 * This method creates an instance of CTestDriveDataCall.  The ConstructL for
 * for CTestDriveDataCall is inherited from and implemented in CTestBase. The 
 * ConstructL uses the CTestDriveDataCall object to load ond open the two iPhone 
 * objects, one from each server. 
 *
 * @param aScriptListEntry: enum indicating which script to use for this test.
 * @leave Leaves if a failure occurs during connect or open of the iPhone by ConstructL.
 * @return pointer to the instance of CTestDriveDataCall.
 */
	{
	CTestDriveDataCall* aA=new(ELeave) CTestDriveDataCall(aScriptListEntry);
	CleanupStack::PushL(aA);
	aA->ConstructL();
	CleanupStack::Pop();
	return aA;
	}

CTestDriveDataCall::CTestDriveDataCall(const TScriptList aScriptListEntry) : iScriptListEntry(aScriptListEntry)
/**
 * This method is the constructor for CTestDriveDataCall.
 *
 * @param aScriptListEntry: enum indicating which script to use for this test.
 * @note Initializes private data "aScriptListEntry" to received parameter.
 */
	{}

TInt CTestDriveDataCall::RunTestL()
/**
 * This method is invoked to start a Data Call Test. 
 * This method starts the modem emulator side of the test.
 *
 * @return KErrNone when no error exists.
 * @return KErrAlreadyExists is returned if modem emulator exists and retry limit expires.
 * @return Variable depending on return value from test's DriveETelApiL method and thread monitoring.
 */
	{
	iCurrentScript=iScriptListEntry;
	return StartEmulatorL();
	}

TInt CTestDriveDataCall::DriveETelApiL()
/**
 * This method contains the real meat of the Client-side Data Call Test code.
 * This method sets up a write test for both an outgoing and incoming call for
 * a Data Call Test.
 *
 * @return KErrNone.
 */
	{
	_LIT(KDataLineName,"Data");
	RLine line;
	INFO_PRINTF1(_L("Opening Data Line\n"));
	TESTL(line.Open(iPhone,KDataLineName)==KErrNone);

	INFO_PRINTF1(_L("Opening New Data Call\n"));
	RCall call;
	TESTL(call.OpenNewCall(line)==KErrNone);

	TRequestStatus reqStatus;
	RMobilePhone::TMMTableSettings tableSettings;
	tableSettings.iLocId=KInternetAccessPoint;
	RMobilePhone::TMMTableSettingsPckg tableSettingsPckg(tableSettings);
	iPhone.InitialiseMM(reqStatus , tableSettingsPckg); 	
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus == KErrNone);

	_LIT(KDialString,"+1234");
	TRequestStatus stat;
	TESTL(call.Dial(KDialString)==KErrNone);

	RCall::TCommPort commPort;
	TESTL(call.LoanDataPort(commPort)==KErrNone);

	RCommServ cs;
	TESTL(cs.Connect()==KErrNone);

	RComm port;
	TESTL(port.Open(cs,commPort.iPort,ECommShared)==KErrNone);

	port.Write(stat,KWriteTestData);
	User::WaitForRequest(stat);
	TESTL(stat.Int()==KErrNone);

    //-- a small delay between successive writes to the COM port
    //-- I had to insert it to fix mistiming between script execution and sending AT-commands to modem
    User::After(500000);		
	port.Write(stat,KWriteTestData);
	User::WaitForRequest(stat);
	TESTL(stat.Int()==KErrNone);

	port.Close();
	cs.Close();
	TESTL(call.RecoverDataPort()==KErrNone);

	TESTL(call.HangUp()==KErrNone);

	User::After(1000000);		

// Now wait for an incoming call...
	TESTL(call.AnswerIncomingCall()==KErrNone);

	TESTL(call.LoanDataPort(commPort)==KErrNone);

	TESTL(cs.Connect()==KErrNone);
	TESTL(port.Open(cs,commPort.iPort,ECommShared)==KErrNone);

	port.Write(stat,KWriteTestData2);
	User::WaitForRequest(stat);
	TESTL(stat.Int()==KErrNone);

    //-- a small delay between successive writes to the COM port
    //-- I had to insert it to fix mistiming between script execution and sending AT-commands to modem
    User::After(500000);		

	port.Write(stat,KWriteTestData2);
	User::WaitForRequest(stat);
	TESTL(stat.Int()==KErrNone);

	port.Close();
	cs.Close();
	TESTL(call.RecoverDataPort()==KErrNone);

	TESTL(call.HangUp()==KErrNone);
	
	call.Close();
	line.Close();
	return KErrNone;
	}

//
// Emulator-side class
// With the assistance of the base class, this class must run the designated script
//
CTestDataCall* CTestDataCall::NewL(const TScript* aScript)
/**
 * 2 Phase Constructor
 *
 * This method creates an instance of CTestDataCall.
 *
 * @param aScript: pointer to the specifics of the script to run. 
 * @leave Leaves if out of memory when attempting to create. 
 * @return pointer to the instance of CTestDataCall.
 */
	{
	CTestDataCall* aA=new(ELeave) CTestDataCall(aScript);
	CleanupStack::PushL(aA);
	aA->ConstructL();
	CleanupStack::Pop();
	return aA;
	}

CTestDataCall::CTestDataCall(const TScript* aScript) : iScript(aScript)
/**
 * This method is the constructor for CTestDataCall.
 *
 * @param aScript: pointer to the specifics of the script to run. 
 * @note Initializes private data "aScript" to received parameter.
 */
	{}

void CTestDataCall::ConstructL()
/**
 * This method is used to implement the 2 Phase Constructor for CTestDataCall.
 * This method uses the CATBase ConstructL to configure the port to be used.
 *
 * @leave Leaves if CATBase leaves.
 */
	{
	CATScriptEng::ConstructL();
	}

TInt CTestDataCall::Start()
/**
 * This method is defined as a pure virtual function that must be implemented.
 * This method is currently not used by the Etel regression test harness.
 * Instead of using this method to start the scripts, the CTestTxMess::Start()
 * method is used to start the scripts.  The CTestTxMess::Start() is called by
 * the responder thread of the scripting engine to start the execution of the script.
 *
 * @return KErrNone.
 */
	{
	StartScript(iScript);
	return KErrNone;
	}

void CTestDataCall::SpecificAlgorithmL(TInt /* aParam */)
/**
 * This method is defined as a pure virtual function that must be implemented.
 * This method is currently not used by the Etel regression test harness.
 * Instead of using this method to perform an algorithm specific to this test,
 * the CTestTxMess::SpecificAlgorithm() method is used.  The CTestTxMess::SpecificAlgorithm() 
 * is called by the scripting engine to perform the test specific algorithm.
 */
	{
	}

void CTestDataCall::Complete(TInt aError)
/**
 * This method is defined as a pure virtual function that must be implemented.
 * This method is currently not used by the Etel regression test harness.
 * Instead of using this method to end the scripts, the CTestTxMess::Complete()
 * method is used to end the scripts.  The CTestTxMess::Complete() is called by
 * the scripting engine to end the execution of the script.
 */
	{
	iReturnValue=aError;
	CActiveScheduler::Stop();
	}
