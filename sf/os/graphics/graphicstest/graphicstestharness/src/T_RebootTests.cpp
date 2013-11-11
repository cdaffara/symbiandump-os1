// T_RebootTests.cpp
// Copyright (c) 1995-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//
#include <e32base.h>
#include <e32cons.h>
#include <bacline.h>
#include <hal.h>
#include <hal_data.h>
#include <f32file.h>
#include <s32file.h>
#include "T_RebootTests.h"

GLDEF_C TInt E32Main();

const TInt8 KMemoryCardDrive = 0x65; //e

const TInt8 KSystemDrive = 0x63; //c

_LIT(KCurrTestFilePath, "%c:%Scurrtest.txt");

_LIT(KCurrentTestBatPath, "%c:\\thistest.script");

_LIT(KTestScriptList, "z:\\autoexec.txt");

_LIT(KScriptFileFlag, "cd test");

_LIT(KDummyLogFile, "z:\\reboot\\dummylog.htm");

_LIT(KRebootTestsPanic, "REBOOTTEST");

const TInt KPathBufLen = 36;

//Global variables to contain formatted paths for file locations.
TBuf<KPathBufLen> KCurrTestFile;
TBuf<KPathBufLen> KCurrentTestBat;

CTRebootTestHelper::CTRebootTestHelper()
	{
	}

CTRebootTestHelper::~CTRebootTestHelper()
	{
	iFs.Close();
	}

CTRebootTestHelper* CTRebootTestHelper::NewLC()
	{
	CTRebootTestHelper* self = new(ELeave) CTRebootTestHelper();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

void CTRebootTestHelper::ConstructL()
	{
	User::LeaveIfError(iFs.Connect());
	}

/**
 Generate a command script to run the specified test from the test script list.
 
 @param aFileIn The source script file
 @param aFileOut The target batch file
 @param aPos The test case from the source file to be placed in the destination
 
 @leave One of the system wide error codes
 */
void CTRebootTestHelper::BuildCommandScriptL(const TDesC& aFileIn, const TDesC& aFileOut, TInt aTestCase)
	{
	aTestCase = aTestCase+2;
	RFile f;
	User::LeaveIfError(f.Open(iFs, aFileIn, EFileRead));
	CleanupClosePushL(f);
	RFileReadStream frs;
	CleanupClosePushL(frs);
	frs.Attach(f);
	
	TInt err = KErrNone;
	TBuf<256> thisLine;
	
	TInt offset = -1; //Flag = 0, Init Script = 1, first script = 2
	
	TChar thisChar = ' ';

	while (err == KErrNone)
		{
		TRAP(err, thisChar = (TChar) frs.ReadInt8L());
		
		if ( (thisChar != '\n') && (thisChar != '\r') )
			thisLine.Append(thisChar);
		
		//This line is complete
		if (thisChar == '\n')
			{
			
			if ( (offset == -1) && (thisLine.CompareF(KScriptFileFlag) == 0) )
				{
				offset++;
				}
			
			if (offset >= 0)
				{
				offset++;
				}
			
			if (offset == aTestCase)
				{
				RDebug::Printf("The next test script to be run will be...");
				RDebug::RawPrint(thisLine);
				break;
				}
			thisLine.Delete(0, thisLine.Length());
			}
		}	

	RFile fo;
	TInt ret = fo.Replace(iFs, aFileOut, EFileWrite);
	if (ret != KErrNone)
			ret = fo.Open(iFs, aFileOut, EFileWrite);
	
	User::LeaveIfError(ret);
	
	TFileText fto;
	fto.Set(fo);
	//Add the TEF command to execute the test correctly
	thisLine.Insert(0,_L("RUN_PROGRAM -1 "));
	fo.Write(0, thisLine.Collapse(), thisLine.Length());
	
	CleanupStack::PopAndDestroy(2, &f);
	}
/**
 Retrieve the previous test run on the previous boot.
 
 @param aFile The full path of the file containing the test count
 @param aValue The current test being run (-1 if none have been).
 
 @return TInt8 The previous test cases index
 */
TInt8 CTRebootTestHelper::FindLastTestL(const TDesC& aFile)
	{
	RFileReadStream fr;
	CleanupClosePushL(fr);
	
	TInt8 currtest = -1;

	if (fr.Open(iFs, aFile, EFileRead) == KErrNone)
		{
		currtest = fr.ReadInt8L();
		RDebug::Printf("Restart Test Manager >> Retrieved %i from file", currtest);
		}
	else
		{
		RDebug::Printf("Restart Test Manager >> Beginning Tests");
		}

	CleanupStack::PopAndDestroy(&fr);
	
  return currtest;
	}
/**
Update the file containing the current test being run.

@param aFile The full path of the file containing the test count
@param aValue The value to be written to the file.
 */
void CTRebootTestHelper::WriteTestOutL(const TDesC& aFile, TInt8 aValue)
	{
	RFileWriteStream fw;
	CleanupClosePushL(fw);
	
	if (fw.Replace(iFs, aFile, EFileWrite) != KErrNone)
		{
		User::LeaveIfError(fw.Create(iFs, aFile, EFileWrite));
		}
	
	fw.WriteInt8L(aValue);
	RDebug::Printf("Wrote %i to file", aValue);
	fw.CommitL();
	CleanupStack::PopAndDestroy(&fw);
	
	}

/**
The rebooting harness prevents the script that initiates it from displaying
any results in DABS; this function generates this file so that "No Tests To Run"
is displayed in the ONB logs. The initiating script should not run any test code
that results are expected from.

@param aScriptName The name of the script file minus its file extension.
@param aLogFile The file containing the dummy log file. All instances of <!LOGFILE!> will be replaced with aScriptName
*/
void CTRebootTestHelper::WriteDummyLogFileL(const TDesC& aScriptName, const TDesC& aLogFile)
    {
    RFile f;
    User::LeaveIfError(f.Open(iFs, aLogFile, EFileRead));
    CleanupClosePushL(f);
    RFileReadStream frs;
    CleanupClosePushL(frs);
    frs.Attach(f);
    
    TInt err = KErrNone;
    TBuf<256> thisLine;
    TChar thisChar = ' ';

    while (err == KErrNone)
        {
        TRAP(err, thisChar = (TChar) frs.ReadInt8L());
        
        if (err == KErrNone)
            {
            thisLine.Append(thisChar);
            
            if (thisLine.Length() > 1)
                {
                //Replace occurrences of %S with aScriptName
                if (thisLine.Right(2).Compare(_L("%S")) == 0)
                    {
                    thisLine.Delete(thisLine.Length()-2, 2);
                    thisLine.Append(aScriptName);
                    thisLine.Append(_L(".htm"));
                    }
                }
            
            if (thisChar == '\n')
                {
                RDebug::RawPrint(thisLine);
                thisLine.Delete(0, thisLine.Length());
                }
            }
        }   

    CleanupStack::PopAndDestroy(2, &f);

    }

/**
Runs the rebooting test harness. Not providing any command line parameters will set up
the test runs; or execute the next test depening on the availability of the test data files.
Passing restart as the parameter will force a reboot. 
 */
LOCAL_C void MainL()
	{
	
	CCommandLineArguments* args = CCommandLineArguments::NewLC();
	CTRebootTestHelper* testHelper = CTRebootTestHelper::NewLC();

	//private path length (includes slashes)
	TBuf<21> privPath;
	testHelper->iFs.PrivatePath(privPath);
	
	KCurrTestFile.Format(KCurrTestFilePath, KMemoryCardDrive, &privPath);
	KCurrentTestBat.Format(KCurrentTestBatPath, KSystemDrive);
	
	if ((args->Count() == 2) && (args->Arg(1).CompareF(_L("r")) != 0))
		{
		TRAPD(dirErr,testHelper->iFs.MkDirAll(KCurrTestFile));
		
		if (dirErr != KErrAlreadyExists)
			{
			User::LeaveIfError(dirErr);
			}
		
		//Retrieve the previous test from the configuation file.
		TInt8 testCase = testHelper->FindLastTestL(KCurrTestFile);

		if (testCase == -1)
			{
			//If there was no configuration file set the test to be the first one
			testCase = 1;
			}

		//Write out the dummy log file for DABS
		testHelper->WriteDummyLogFileL(args->Arg(1), KDummyLogFile);
		
		//Construct the TEF script to run the current test case.
		testHelper->BuildCommandScriptL(KTestScriptList, KCurrentTestBat, testCase);
		
		//Write out the current state to MMC
		testCase++;
		testHelper->WriteTestOutL(KCurrTestFile, testCase);

		
		}
	else if (args->Count() == 2)
		{
		//If the app is run with restart in the command line then force a reboot
		if (args->Arg(1).CompareF(_L("r"))==0)
			{
			RDebug::Printf("Restart Test Manager >> Batch File Requested Restart");
			testHelper->iFs.Delete(KCurrentTestBat);
			HAL::Set(HALData::ECustomRestart, 25);
			}
		}
	CleanupStack::PopAndDestroy(2,args); 
	}

LOCAL_C void DoStartL()
	{
	CActiveScheduler* scheduler = new (ELeave) CActiveScheduler();
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install(scheduler);
	TRAPD(ret, MainL());
	
	//If a leave has occurred then prevent the tests from continuing
	if (ret != KErrNone)
		{
		RDebug::Printf("Restart Test Manager >> About to panic; MainL exited with error %i", ret);
		User::SetCritical(User::ESystemCritical);
		User::Panic(KRebootTestsPanic, 0);
		}

	CleanupStack::PopAndDestroy(scheduler);
	}


GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	TRAPD(mainError, DoStartL());
	if (mainError)
		RDebug::Printf("Restart Test Manager >> Error %i", mainError);
	delete cleanup;
	__UHEAP_MARKEND;
	return KErrNone;
	}

