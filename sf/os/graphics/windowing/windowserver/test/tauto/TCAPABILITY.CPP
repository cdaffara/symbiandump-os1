// Copyright (c) 1996-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Capability test
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#include "TCAPABILITY.H"

_LIT(KTestResultsFile, "C:\\DATA\\TestResult.Dat");
_LIT(KSpace, " ");
_LIT(KTest0Name, "All capabilities");
_LIT(KTest0Exe, "TCAP_ALL.exe");
_LIT(KTest0Cap, "CAPABILITY_ALL");
_LIT(KTest1Name, "No capabilities");
_LIT(KTest1Exe, "TCAP_NONE.exe");
_LIT(KTest1Cap, "CAPABILITY_NONE");
_LIT(KTest2Name, "WriteDeviceData and SwEvent capabilities");
_LIT(KTest2Exe, "TCAP_ONE.exe");
_LIT(KTest2Cap, "WRITEDATA+SWEVENT");
_LIT(KTest3Name, "WriteDeviceData and PowerMgnt capabilities");
_LIT(KTest3Exe, "TCAP_TWO.exe");
_LIT(KTest3Cap, "WRITEDATA+POWERMGMT");
_LIT(KTest4Name, "PowerMgnt and SwEvent capabilities");
_LIT(KTest4Exe, "TCAP_THREE.exe");
_LIT(KTest4Cap, "POWERMGMT+SWEVENT");


//CCapabilityTest 
CTCapability::CTCapability(CTestStep* aStep):
	CTWsGraphicsBase(aStep)
	{
	}

CTCapability::~CTCapability()
	{
	}

void CTCapability::ConstructL()
	{
	//Empty function ConstructL is pure virtual function and this 
	//definition required to satisfy the compiler
	}

//Reads the shared files which contains the total tests	run and tests passed.
void CTCapability::GetCapabilityTestResultsL(TInt& aNoOfTests, TInt& aNoOfTestsPass)
	{	
	TBuf<256> noOfTest;
	TBuf<256> noOfTestPass;
	RFs fileSession;
	RFile file;
	TFileText textFile;
	User::LeaveIfError(fileSession.Connect());
	CleanupClosePushL(fileSession);
	User::LeaveIfError(file.Open(fileSession,KTestResultsFile,EFileRead));
	CleanupClosePushL(file);
	textFile.Set(file);
	if(textFile.Read(noOfTest)==KErrNone)
		{
		TLex lexVar(noOfTest);
		lexVar.Val(aNoOfTests);
		if(textFile.Read(noOfTestPass)==KErrNone)
			{
			lexVar=noOfTestPass;
			lexVar.Val(aNoOfTestsPass);
			}
		else
			{
			aNoOfTestsPass=0;
			}
		}
	CleanupStack::PopAndDestroy(&file);
	CleanupStack::PopAndDestroy(&fileSession);
	}
	
 void CTCapability::LaunchNewProcess(const TDesC& aExecutable,const TDesC& aCapability)
	{
	TBuf<128> args;
 	args.Append(aCapability);
 	args.Append(KSpace);
 	args.AppendNum(TheClient->iGroup->GroupWin()->Identifier());
	RProcess pr;
//	TInt noOfTest,noOfTestPass;
	if (pr.Create(aExecutable,args)==KErrNone)
		{
		TRequestStatus status=NULL;
		pr.Logon(status);
		pr.Resume();
		User::WaitForRequest(status);
		//Close all the panic windows to avoid "Hangs the H4" 
		//PDEF100501: TEF Migrated Test TCapability Hangs the H4 
	   	if (iTest->iScreenNumber == 0)
			iTest->CloseAllPanicWindows();
		pr.Close();
//		GetCapabilityTestResultsL(noOfTest,noOfTestPass);
//		UpdateTestResults(noOfTest,noOfTestPass);
		}
	}
	
void CTCapability::RunTestCaseL(TInt /*aCurTestCase*/)
	{
	((CTCapabilityStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);
	switch(++iTest->iState)
		{
/**

  @SYMTestCaseID GRAPHICS-WSERV-0305
  
  @SYMDEF             DEF081259
 
  @SYMTestCaseDesc Capability test with a process with all capability
    
  @SYMTestPriority High
 
  @SYMTestStatus Implemented
 
  @SYMTestActions The security threat API's are called with all 
 				 capability to test whether the API's are accessible or not.
  
  @SYMTestExpectedResults If the required capability is defined to test code then API should 
 							 accessible, otherwise it should panic the test.
 
*/
		case 1:
			((CTCapabilityStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0305"));
			iTest->LogSubTest(KTest0Name);
 			LaunchNewProcess(KTest0Exe, KTest0Cap);
			break;
/**

  @SYMTestCaseID GRAPHICS-WSERV-0306
  
  @SYMDEF             DEF081259
 
  @SYMTestCaseDesc Capability test with a process with no capability
    
  @SYMTestPriority High
 
  @SYMTestStatus Implemented
 
  @SYMTestActions The security threat API's are called with no 
 				 capability to test whether the API's are accessible or not.
  
  @SYMTestExpectedResults If the required capability is defined to test code then API should 
 							 accessible, otherwise it should panic the test.
 
*/
		case 2:
			((CTCapabilityStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0306"));
			//create process with no capability 
			RDebug::Print(KPlatsecBegin);
			iTest->LogSubTest(KTest1Name);
 			LaunchNewProcess(KTest1Exe,KTest1Cap);
			break;
/**

  @SYMTestCaseID GRAPHICS-WSERV-0307
  
  @SYMDEF             DEF081259
 
  @SYMTestCaseDesc Capability test with a process with WriteDevicedata and SwEvent capability
    
  @SYMTestPriority High
 
  @SYMTestStatus Implemented
 
  @SYMTestActions The security threat API's are called with WriteDevicedata and SwEvent 
 				 capability to test whether the API's are accessible or not.
  
  @SYMTestExpectedResults If the required capability is defined to test code then API should 
 							 accessible, otherwise it should panic the test.
 
*/
		case 3:
			((CTCapabilityStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0307"));
			//create process with WriteDevicedata and SwEvent capability 
			iTest->LogSubTest(KTest2Name);
 			LaunchNewProcess(KTest2Exe,KTest2Cap);
			break;
/**

  @SYMTestCaseID GRAPHICS-WSERV-0308
  
  @SYMDEF             DEF081259
 
  @SYMTestCaseDesc Capability test with a process with WriteDevicedata and PowerMgmt capability
    
  @SYMTestPriority High
 
  @SYMTestStatus Implemented
 
  @SYMTestActions The security threat API's are called with WriteDevicedata and PowerMgmt 
 				 capability to test whether the API's are accessible or not.
  
  @SYMTestExpectedResults If the required capability is defined to test code then API should 
 							 accessible, otherwise it should panic the test.
 
*/
		case 4:
			((CTCapabilityStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0308"));
			//create process with WriteDevicedata and PowerMgmt capability 
			iTest->LogSubTest(KTest3Name);
 			LaunchNewProcess(KTest3Exe,KTest3Cap);
			break;
/**

  @SYMTestCaseID GRAPHICS-WSERV-0309
  
  @SYMDEF             DEF081259
 
  @SYMTestCaseDesc Capability test with a process with PowerMgmt and SwEvent capability
    
  @SYMTestPriority High
 
  @SYMTestStatus Implemented
 
  @SYMTestActions The security threat API's are called with PowerMgmt and SwEvent 
 				 capability to test whether the API's are accessible or not.
  
  @SYMTestExpectedResults If the required capability is defined to test code then API should 
 							 accessible, otherwise it should panic the test.
 
*/
		case 5:
			((CTCapabilityStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0309"));
			//create process with PowerMgmt and SwEvent capability 
			iTest->LogSubTest(KTest4Name);
 			LaunchNewProcess(KTest4Exe, KTest4Cap);
			break;
		default :
			((CTCapabilityStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
			((CTCapabilityStep*)iStep)->CloseTMSGraphicsStep();
			RDebug::Print(KPlatsecEnd);
			RFs fileSession;
			User::LeaveIfError(fileSession.Connect());
			CleanupClosePushL(fileSession);
			CFileMan *fileMan=CFileMan::NewL(fileSession);
			fileMan->Delete(KTestResultsFile);
			delete fileMan;
			CleanupStack::PopAndDestroy(&fileSession);
			TestComplete();		
			break;
		}
	((CTCapabilityStep*)iStep)->RecordTestResultL();
	}
	
__WS_CONSTRUCT_STEP__(Capability)
