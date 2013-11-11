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
// This file initiates the "Data Calls with diff params(speed)
// The client (application) side must start the emulator and drive the ETel API.
// The emulator side runs the designated script.
// 
//

/**
 @file
 @note  There are mulitple classes implemented in this file.
 @note  These classes are CTestDriveCbstParse and CTestCbstParse.
*/


#include <e32test.h>
#include "Te_LoopBackSLOGGER.H"
#include "Te_LoopBackCCbstParse.h"

#include "../../hayes/TSYCONFG.H" // for KInternetAccessPoint

_LIT(KMmPhoneName,"GsmPhone1");
 

//
// Test-side class
// With the assistance of the base class, this class must start the emulator
// and drive the ETel API.
//
CTestDriveCbstParse* CTestDriveCbstParse::NewL(const TScriptList aScriptListEntry,
															 TInt aVarDelay)
/**
 * 2 Phase Constructor
 *
 * This method creates an instance of CTestDriveCbstParse.  The ConstructL for
 * for CTestDriveCbstParse is inherited from and implemented in CTestBase. The
 * ConstructL uses the CTestDriveCbstParse object to load ond open the two iPhone
 * objects, one from each server.
 *
 * @param aScriptListEntry: enum indicating which script to use for this test.
 * @param aVarDelay: integer indicating variable delay value, in seconds, to be used with an EWait script
 * @leave Leaves if a failure occurs during connect or open of the iPhone by ConstructL.
 * @return pointer to the instance of CTestDriveCbstParse.
 */
	{
	CTestDriveCbstParse* aA=new(ELeave) CTestDriveCbstParse(aScriptListEntry, aVarDelay);
	CleanupStack::PushL(aA);
	aA->ConstructL();
	CleanupStack::Pop();
	return aA;
	}


CTestDriveCbstParse::CTestDriveCbstParse(const TScriptList aScriptListEntry,
													   TInt aVarDelay
													  ) : iScriptListEntry(aScriptListEntry),
													      iVarDelay(aVarDelay)
/**
 * This method is the constructor for CTestDriveCbstParse.
 *
 * @param aScriptListEntry: enum indicating which script to use for this test.
 * @param aVarDelay: integer indicating variable delay value, in seconds, to be used with an EWait script
 * @note Initializes private data "aScriptListEntry" to received parameter.
 * @note Initializes private data "aVarDelay" to received parameter.
 */
	{}

TInt CTestDriveCbstParse::RunTestL()
/**
 * This method is invoked to start a "Data Call answer and remote hang-up closely followed by
 * a dial" test.  This method sets the CTestBase current script to run and the CTestBase variable
 * delay value to use for an EWait
 * script and then starts the modem emulator side of the test.
 *
 * @return KErrNone when no error exists.
 * @return KErrAlreadyExists is returned if modem emulator exists and retry limit expires.
 * @return Variable depending on return value from test's DriveETelApiL method and thread monitoring.
 */
	{
	iCurrentScript=iScriptListEntry;
	iVariableDelay=iVarDelay;
	return StartEmulatorL();
	}

TInt CTestDriveCbstParse::DriveETelApiL()
/**
 * This method contains the real meat of the 
 * @return KErrNone.
 */
	{
	INFO_PRINTF1(_L("Wrong CBST string +CBST:  \n")) ;
	DriveTestL() ; 
	INFO_PRINTF1(_L("Wrong CBST string +CBST: (3,4), \n")) ;
	DriveTestL() ; 
	INFO_PRINTF1(_L("Wrong CBST string +CBST: (3,4),(3),  \n")) ;
	DriveTestL() ; 
	INFO_PRINTF1(_L("Wrong speed string +CBST: (3,4, \n")) ;
	DriveTestL() ;
	INFO_PRINTF1(_L("Wrong name string +CBST: (3,4), (3\n ")) ;
	DriveTestL() ; 
	INFO_PRINTF1(_L("Wrong CE string+CBST: (3,4), (3,2), (1 \n")) ;
	DriveTestL() ; 
	return KErrNone;
	}

void CTestDriveCbstParse::DriveTestL()
	{
	// Open iPhone
	iPhone3.Open(iServer,KMmPhoneName);
	
	TRequestStatus	reqStatus;
	INFO_PRINTF1(_L("Initialising the Phone...\n"));

	RMobilePhone::TMMTableSettings tableSettings;
	tableSettings.iLocId=KInternetAccessPoint;
	RMobilePhone::TMMTableSettingsPckg tableSettingsPckg(tableSettings);
	iPhone3.InitialiseMM(reqStatus , tableSettingsPckg); 	

	User::WaitForRequest(reqStatus);

	TESTL(reqStatus == KErrNone);
	INFO_PRINTF1(_L("Attempting to force TSY into EPhoneNotInitialised state\n"));
	TRequestStatus status(KErrNone);
	RMobilePhone::TMobilePhoneNetworkInfoV1 networkInfo;
	TPckg<RMobilePhone::TMobilePhoneNetworkInfoV1> networkInfoPckg(networkInfo);
	RMobilePhone::TMobilePhoneLocationAreaV1 area;
	iPhone3.GetCurrentNetwork(status,networkInfoPckg,area);
	User::WaitForRequest(status);
	TESTL(status==KErrTimedOut);
	// Close iPhone
	iPhone3.Close();		
	}


//
// Emulator-side class
// With the assistance of the base class, this class must run the designated script
//
CTestCbstParse* CTestCbstParse::NewL(const TScript* aScript)
/**
 * 2 Phase Constructor
 *
 * This method creates an instance of CTestCbstParse.
 *
 * @param aScript: pointer to the specifics of the script to run.
 * @leave Leaves if out of memory when attempting to create.
 * @return pointer to the instance of CTestCbstParse.
 */
	{
	CTestCbstParse* aA=new(ELeave) CTestCbstParse(aScript);
	CleanupStack::PushL(aA);
	aA->ConstructL();
	CleanupStack::Pop();
	return aA;
	}

CTestCbstParse* CTestCbstParse::NewL(const TScript* aScript, const TInt aVarDelay)
/**
 * 2 Phase Constructor
 *
 * This method creates an instance of CTestCbstParse.
 *
 * @param aScript: pointer to the specifics of the script to run.
 * @param aVarDelay: time delay
 * @leave Leaves if out of memory when attempting to create.
 * @return pointer to the instance of CTestCbstParse.
 */
	{
	CTestCbstParse* aA=new(ELeave) CTestCbstParse(aScript, aVarDelay);
	CleanupStack::PushL(aA);
	aA->ConstructL();
	CleanupStack::Pop();
	return aA;
	}

CTestCbstParse::CTestCbstParse(const TScript* aScript) : iScript(aScript)
/**
 * This method is the constructor for CTestCbstParse.
 *
 * @param aScript: pointer to the specifics of the script to run.
 * @note Initializes private data "aScript" to received parameter.
 */
	{}

CTestCbstParse::CTestCbstParse(const TScript* aScript, const TInt aVarDelay) : CATScriptEng(aVarDelay), iScript(aScript)
/**
 * This method is the constructor for CTestCbstParse.
 *
 * @param aScript: pointer to the specifics of the script to run.
 * @param aVarDelay: user specified time delay
 * @note Initializes private data "aScript" to received parameter.
 */
	{}

void CTestCbstParse::ConstructL()
/**
 * This method is used to implement the 2 Phase Constructor for CTestCbstParse.
 * This method uses the CATBase ConstructL to configure the port to be used.
 *
 * @leave Leaves if CATBase leaves.
 */
	{
	CATScriptEng::ConstructL();
	}

TInt CTestCbstParse::Start()
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

void CTestCbstParse::SpecificAlgorithmL(TInt /* aParam */)
/**
 * This method is defined as a pure virtual function that must be implemented.
 * This method is currently not used by the Etel regression test harness.
 * Instead of using this method to perform an algorithm specific to this test,
 * the CTestTxMess::SpecificAlgorithm() method is used.  The CTestTxMess::SpecificAlgorithm()
 * is called by the scripting engine to perform the test specific algorithm.
 */
	{
    }

void CTestCbstParse::Complete(TInt aError)
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

