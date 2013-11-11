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
// This file initiates the "Unsolicited Message" test
// The client (application) side must start the emulator and drive the ETel API.
// The emulator side runs the designated script.
// 
//

/**
 @file
 @note  There are mulitple classes implemented in this file.
 @note  These classes are CTestDriveUnsolicited and CTestUnsolicited.
*/


#include <e32test.h>
#include "Te_LoopBackSLOGGER.H"
#include "Te_LoopBackCUnsolicited.h"
#include "../../hayes/TSYCONFG.H" // for KInternetAccessPoint

_LIT(KMmPhoneName,"GsmPhone1");
 

//
// Test-side class
// With the assistance of the base class, this class must start the emulator
// and drive the ETel API.
//
CTestDriveUnsolicited* CTestDriveUnsolicited::NewL(const TScriptList aScriptListEntry,
															 TInt aVarDelay)
/**
 * 2 Phase Constructor
 *
 * This method creates an instance of CTestDriveUnsolicited.  The ConstructL for
 * for CTestDriveUnsolicited is inherited from and implemented in CTestBase. The
 * ConstructL uses the CTestDriveUnsolicited object to load ond open the two iPhone
 * objects, one from each server.
 *
 * @param aScriptListEntry: enum indicating which script to use for this test.
 * @param aVarDelay: integer indicating variable delay value, in seconds, to be used with an EWait script
 * @leave Leaves if a failure occurs during connect or open of the iPhone by ConstructL.
 * @return pointer to the instance of CTestDriveUnsolicited.
 */
	{
	CTestDriveUnsolicited* aA=new(ELeave) CTestDriveUnsolicited(aScriptListEntry, aVarDelay);
	CleanupStack::PushL(aA);
	aA->ConstructL();
	CleanupStack::Pop();
	return aA;
	}


CTestDriveUnsolicited::CTestDriveUnsolicited(const TScriptList aScriptListEntry,
													   TInt aVarDelay
													  ) : iScriptListEntry(aScriptListEntry),
													      iVarDelay(aVarDelay)
/**
 * This method is the constructor for CTestDriveUnsolicited.
 *
 * @param aScriptListEntry: enum indicating which script to use for this test.
 * @param aVarDelay: integer indicating variable delay value, in seconds, to be used with an EWait script
 * @note Initializes private data "aScriptListEntry" to received parameter.
 * @note Initializes private data "aVarDelay" to received parameter.
 */
	{}

TInt CTestDriveUnsolicited::RunTestL()
/**
 * This method is invoked to start a "Data Call answer and remote hang-up closely followed by
 * a dial" test.  This method sets the CTestBase current script to run and the CTestBase variable
 * delay value to use for an EWait
 * script and then starts the modem emulator side of the test.
 *
 * @return KErrNone when no error exists.
 * @return KErrAlreadyExists is returned if modem emulator exists and retry limit expires.
 * @return Variable depending on return value from test's DriveETelApi method and thread monitoring.
 */
	{
	iCurrentScript=iScriptListEntry;
	iVariableDelay=iVarDelay;
	return StartEmulatorL();
	}

TInt CTestDriveUnsolicited::DriveETelApiL()
/**
 * This method contains the real meat of the 
 *
 * @return KErrNone.
 */
	{
	INFO_PRINTF1(_L("Bludgeoning Initialisation with Unsolicited Messages\n"));
	TRAP_IGNORE(DriveTestL()) ; 
	return KErrNone;
	}

void CTestDriveUnsolicited::DriveTestL()
	{
	// Open iPhone
	TESTL(iPhone3.Open(iServer,KMmPhoneName) == KErrNone);

	TRequestStatus reqStatus;
	RMobilePhone::TMMTableSettings tableSettings;
	tableSettings.iLocId=KInternetAccessPoint;
	RMobilePhone::TMMTableSettingsPckg tableSettingsPckg(tableSettings);
	iPhone3.InitialiseMM(reqStatus, tableSettingsPckg); 	
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus == KErrNone);

//	test.Printf(_L("Attempting to force TSY into EPhoneNotInitialised state\n"));
//	TRequestStatus status(KErrNone);
//	RMobilePhone::TMobilePhoneNetworkInfoV1 networkInfo;
//	TPckg<RMobilePhone::TMobilePhoneNetworkInfoV1> networkInfoPckg(networkInfo);
//	RMobilePhone::TMobilePhoneLocationAreaV1 area;
//	iPhone3.GetCurrentNetwork(status,networkInfoPckg,area);
//	User::WaitForRequest(status);
//	test(status==KErrTimedOut);
	// Close iPhone
	iPhone3.Close();		
	}


//
// Emulator-side class
// With the assistance of the base class, this class must run the designated script
//
CTestUnsolicited* CTestUnsolicited::NewL(const TScript* aScript)
/**
 * 2 Phase Constructor
 *
 * This method creates an instance of CTestUnsolicited.
 *
 * @param aScript: pointer to the specifics of the script to run.
 * @leave Leaves if out of memory when attempting to create.
 * @return pointer to the instance of CTestUnsolicited.
 */
	{
	CTestUnsolicited* aA=new(ELeave) CTestUnsolicited(aScript);
	CleanupStack::PushL(aA);
	aA->ConstructL();
	CleanupStack::Pop();
	return aA;
	}

CTestUnsolicited* CTestUnsolicited::NewL(const TScript* aScript, const TInt aVarDelay)
/**
 * 2 Phase Constructor
 *
 * This method creates an instance of CTestUnsolicited.
 *
 * @param aScript: pointer to the specifics of the script to run.
 * @param aVarDelay: time delay
 * @leave Leaves if out of memory when attempting to create.
 * @return pointer to the instance of CTestUnsolicited.
 */
	{
	CTestUnsolicited* aA=new(ELeave) CTestUnsolicited(aScript, aVarDelay);
	CleanupStack::PushL(aA);
	aA->ConstructL();
	CleanupStack::Pop();
	return aA;
	}

CTestUnsolicited::CTestUnsolicited(const TScript* aScript) : iScript(aScript)
/**
 * This method is the constructor for CTestUnsolicited.
 *
 * @param aScript: pointer to the specifics of the script to run.
 * @note Initializes private data "aScript" to received parameter.
 */
	{}

CTestUnsolicited::CTestUnsolicited(const TScript* aScript, const TInt aVarDelay) : CATScriptEng(aVarDelay), iScript(aScript)
/**
 * This method is the constructor for CTestUnsolicited.
 *
 * @param aScript: pointer to the specifics of the script to run.
 * @param aVarDelay: user specified time delay
 * @note Initializes private data "aScript" to received parameter.
 */
	{}

void CTestUnsolicited::ConstructL()
/**
 * This method is used to implement the 2 Phase Constructor for CTestUnsolicited.
 * This method uses the CATBase ConstructL to configure the port to be used.
 *
 * @leave  Leaves if CATBase leaves.
 */
	{
	CATScriptEng::ConstructL();
	}

TInt CTestUnsolicited::Start()
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

void CTestUnsolicited::SpecificAlgorithmL(TInt /* aParam */)
/**
 * This method is defined as a pure virtual function that must be implemented.
 * This method is currently not used by the Etel regression test harness.
 * Instead of using this method to perform an algorithm specific to this test,
 * the CTestTxMess::SpecificAlgorithm() method is used.  The CTestTxMess::SpecificAlgorithm()
 * is called by the scripting engine to perform the test specific algorithm.
 */
	{
	}

void CTestUnsolicited::Complete(TInt aError)
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

