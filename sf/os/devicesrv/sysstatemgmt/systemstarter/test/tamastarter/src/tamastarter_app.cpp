// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/

#include <e32base.h>
#include <e32debug.h>
#include <f32file.h>
#include <e32const.h>
#include <bautils.h>
#include <bacline.h>
#include <apacmdln.h>
#include <utf.h>
#include <test/testexecutestepbase.h>
#include <cinidata.h>
#include <sysmonclisess.h>

#include "tamastarter_constants.h"

_LIT(KAmaStarterTestAppPanic, "AmaStarterTestApp");


//Function to create the new file if it does not exists.
void CreateResultFileL(const TDesC& aFileName)
	{
	RFs fs;
	User::LeaveIfError(fs.Connect());	
	CleanupClosePushL(fs);
	
	if (!BaflUtils::FileExists(fs, aFileName))
		{
		// Create the folder if it does not exist
		TInt ret = fs.MkDirAll(KTestAppResultsPath);
		if (ret != KErrAlreadyExists)
			{
			User::LeaveIfError(ret);
			}
		
		RFile	resultFile;
		ret = resultFile.Create(fs, aFileName, EFileShareExclusive); //CIniData isn't thread safe
		resultFile.Close();
		User::LeaveIfError(ret);
		}
	
	CleanupStack::PopAndDestroy(&fs);
	}



// Set the counter value to '0' in case it does not exists.
void InitCounterL(CIniData& aIniData, const TDesC& aSection, const TDesC& aKey)
	{
	TInt value = 0;
	TBool readResult = aIniData.FindVar(aSection, aKey, value);
	if (!readResult)
		{
		TBuf<2> descVal;
		descVal.Num(value);
		User::LeaveIfError(aIniData.AddValue(aSection, aKey, descVal));		
		aIniData.WriteToFileL();
		}
	}

// Get the counter value from the data (log) file.
TInt GetCounter(CIniData& aIniData, const TDesC& aSection, const TDesC& aKey)
	{
	TInt value = 0;
	aIniData.FindVar(aSection, aKey, value);
	return value;
	}


// IncrementCounter increment the counter value by '1'
void IncrementCounterL(CIniData& aIniData, const TDesC& aSection, const TDesC& aKey)
	{
	TInt value = 1 + GetCounter(aIniData, aSection, aKey);
	TBuf<2> descVal;
	descVal.Num(value);
	User::LeaveIfError(aIniData.SetValue(aSection, aKey, descVal));
	aIniData.WriteToFileL();
	}


//StartTestL processes the command line, and appropriately updates the various counters
//(Sucessful, failure, panic etc) depending on the arguments provided during the launch.
//The EXE name is used as section name where all the counters will be added/updated.
//This way each EXE launched will update its own result in the section in the result
//file. The result file name is expected to be provided as an command line argument for the
//result of the launch of this test EXE to be logged.
void StartTestL(void)
	{
	//Get the executable filename
	RProcess testApp;
	TParse parse;
	parse.Set(testApp.FullName(), NULL, NULL);
	
	TTime starttime;
	starttime.UniversalTime();
	TFileName fileForLog(parse.Name());
	RDebug::Print( _L("*** %S::StartTestL - entry at time %d:%d.%d"), &fileForLog, starttime.DateTime().Minute(), starttime.DateTime().Second(), starttime.DateTime().MicroSecond());

	HBufC* commandLineDes = NULL;	
	//EXE is launched as new process
	if ( User::CommandLineLength() > 0 )
		{
		commandLineDes = HBufC::New(User::CommandLineLength());
		TPtr cmd = commandLineDes->Des();
		User::CommandLine(cmd);
		}
	//EXE is launched as APP
	else
		{
		CApaCommandLine*	apaCmdLine = NULL;
		CApaCommandLine::GetCommandLineFromProcessEnvironment(apaCmdLine);
		CleanupStack::PushL(apaCmdLine);		
		commandLineDes = CnvUtfConverter::ConvertToUnicodeFromUtf8L(apaCmdLine->TailEnd());
		CleanupStack::PopAndDestroy(apaCmdLine);
		}
	
	CleanupStack::PushL(commandLineDes);
	TLex cmdLine = commandLineDes->Des();
	
	TInt	timeout = 0;
	TInt	retriedCount = 0;
	
	TInt	err = KErrNone;
	TBool	toLeave = EFalse;
	
	TInt	panic = 0;
	TBool	toPanic = EFalse;
	
	TInt	rend = KErrNone;
	TBool	toRend = EFalse;
	
	TInt	cancelMonLaunch = 0;
	TBool	cancelMon = EFalse;
	
	TBuf<KMaxFileName>	fileName(KTestAppResultsPath);
	TPtrC ptr;

	for(ptr.Set(cmdLine.NextToken()) ; ptr.Length() ; ptr.Set(cmdLine.NextToken()))
		{
		//Result file name
		if ( !ptr.CompareF(KLog) )
			{
			fileName.Append(cmdLine.NextToken());
			fileName.Append(KTestLogFileExtension);
			CreateResultFileL(fileName);
			}
		//Rendezvous with
		else if ( !ptr.CompareF(KRendezvous) )
			{
			TLex	lex(cmdLine.NextToken());
			lex.Val(rend);
			toRend = ETrue;
			}
		//Timeout
		else if ( !ptr.CompareF(KTimeout) )
			{
			TLex	lex(cmdLine.NextToken());
			lex.Val(timeout);
			}
		//Retry count
		else if ( !ptr.CompareF(KRetry) )
			{
			TLex	lex(cmdLine.NextToken());
			lex.Val(retriedCount);
			}
		//Leave Error value
		else if ( !ptr.CompareF(KLeaveErrorValue) )
			{
			TLex	lex(cmdLine.NextToken());
			lex.Val(err);
			toLeave = ETrue;
			}
		//Panic
		else if ( !ptr.CompareF(KPanicValue) )
			{
			TLex	lex(cmdLine.NextToken());
			lex.Val(panic);
			toPanic = ETrue;
			}
		//Cancel monitoring after N launches/restarts
		else if ( !ptr.CompareF(KCancelMonitoring) )
			{
			TLex	lex(cmdLine.NextToken());
			lex.Val(cancelMonLaunch);
			cancelMon = ETrue;
			}	
		}
		
	CIniData* appResults = CIniData::NewL(fileName);
	CleanupStack::PushL(appResults);

	// Set all the counters to 0
	//  NOTE: This is done if the counter does not exist (otherwise, it keeps the old value)
	InitCounterL(*appResults, parse.Name(), KLaunchCntr);
	InitCounterL(*appResults, parse.Name(), KRetryCntr);
	InitCounterL(*appResults, parse.Name(), KSuccessCntr);
	InitCounterL(*appResults, parse.Name(), KPanicCntr);
	InitCounterL(*appResults, parse.Name(), KLeaveCntr);
	InitCounterL(*appResults, parse.Name(), KFailedCntr);

	// Update the Launch counter	
	IncrementCounterL(*appResults, parse.Name(), KLaunchCntr);

	// Update the Retry counter	
	if (retriedCount > 0)
		{
		TInt retryCntr = GetCounter(*appResults, parse.Name(), KRetryCntr);	

		//Check if app is to be timedout in case retry counter is set > 0
		if (retryCntr < retriedCount && timeout > 0)
			{
			IncrementCounterL(*appResults, parse.Name(), KRetryCntr);
			User::After(TTimeIntervalMicroSeconds32(timeout*1000));
			}
		}		

	// Update the Success counter (or Panic, or Leave)
	if (toPanic == EFalse && toLeave == EFalse && rend == KErrNone)
		{
		IncrementCounterL(*appResults, parse.Name(), KSuccessCntr);
		}
	else
		{
		if (toPanic)
			{
			IncrementCounterL(*appResults, parse.Name(), KPanicCntr);
			User::SetJustInTime(EFalse);
			User::Panic(KAmaStarterTestAppPanic,panic);			
			}
		if (toLeave)
			{
			IncrementCounterL(*appResults, parse.Name(), KLeaveCntr);
			User::Leave(err);
			}
		}

	// Update the Failed counter	
	if (toRend)
		{
		if (rend != KErrNone)
			{
			IncrementCounterL(*appResults, parse.Name(), KFailedCntr);
			}
		RProcess::Rendezvous(rend);
		}
	
	CleanupStack::PopAndDestroy(2); //cmdLine, iAppResults
	
	//Check if this app/process was asked to cancel its monitoring, after it has been
	//restarted/launched 'N' (cancelMonLaunch) times
	if (cancelMon)
		{
		TInt launchCntr = GetCounter(*appResults, parse.Name(), KLaunchCntr);
		if (launchCntr >= cancelMonLaunch)
			{
			//Wait for 25 secs to let process execution time get passed throttle time
			User::After(TTimeIntervalMicroSeconds32(25000000));
			RSysMonSession	sysMonSession;
			sysMonSession.CancelMonitorSelfL();
			}
		}
	
	TTime endtime;
	endtime.UniversalTime();
	RDebug::Print( _L("*** %S::StartTestL - exit at time %d:%d.%d"), &fileForLog, endtime.DateTime().Minute(), endtime.DateTime().Second(), endtime.DateTime().MicroSecond());
	}

TInt E32Main()
	{	
	__UHEAP_MARK;

	CTrapCleanup* theTrapCleanup = CTrapCleanup::New();

	TRAPD(err, StartTestL());
	if (err)
		{
		User::Panic(KAmaStarterTestAppPanic,err);
		}
    delete theTrapCleanup;

	__UHEAP_MARKEND;
	
	return KErrNone;
	}
