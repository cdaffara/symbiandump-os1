// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 
#include "appfwk_sysstart_test_appUtils.h"
#include <e32debug.h>
#include <s32file.h>

_LIT(KSysStartTestAppUtilsPanic, "SysStartTestAppUtils");

/**
  This function reads in the results contained within the specified result file,
  and populates the struct passed in.
  
  If the result file is not found, then it initialises the struct to the default
  values.
  
  The expected format of the result file is:
  App Start count=value1
  App Rendezvous count=value2
  App Rendezvous fail count=value3
  '=' is used as the delimiter.
  
  @param aFileName - the file which contains results
  @param aTestAppResults - struct to be populated with results from file
  */
EXPORT_C void CAppfwkSysstartTestAppUtils::ReadTestAppResultsFileL(const TDesC& aFileName, TAppfwkSysstartTestAppResults& aTestAppResults)
	{
	RDebug::Print(_L("CAppfwkSysstartTestAppUtils::ReadTestAppResultsFileL - entry"));
	RFs myFs;
	User::LeaveIfError(myFs.Connect());
	
	CleanupClosePushL(myFs);
	
	RFileReadStream reader;
		
	reader.PushL();
	
	TInt err = KErrGeneral;
	
	do
		{
		err = reader.Open(myFs,aFileName,EFileWrite);
		}
	while (err == KErrInUse);
	
	TChar delimChar('=');
	TBuf8<255> aDes;
	
	switch (err)
		{
		case KErrNotFound:
			InitTestAppResults(aTestAppResults);
			break;
		case KErrNone:
			reader.ReadL(aDes, delimChar);
			aTestAppResults.iStartCount = reader.ReadInt16L();
			reader.ReadL(aDes, delimChar);
			aTestAppResults.iRendCount = reader.ReadInt16L();
			reader.ReadL(aDes, delimChar);
			aTestAppResults.iRendFailCount = reader.ReadInt16L();
			
			// print out results for debug purposes.
			RDebug::Print(_L("CAppfwkSysstartTestAppUtils::ReadTestAppResultsFileL - aTestAppResults.iStartCount - %d"),     aTestAppResults.iStartCount);
			RDebug::Print(_L("CAppfwkSysstartTestAppUtils::ReadTestAppResultsFileL - aTestAppResults.iRendCount  - %d"),     aTestAppResults.iRendCount);
			RDebug::Print(_L("CAppfwkSysstartTestAppUtils::ReadTestAppResultsFileL - aTestAppResults.iRendFailCount  - %d"), aTestAppResults.iRendFailCount);
			break;
		default:
			User::Panic(KSysStartTestAppUtilsPanic,err);
			break;
		}

	CleanupStack::PopAndDestroy(&reader);
	CleanupStack::PopAndDestroy(&myFs);
	RDebug::Print(_L("CAppfwkSysstartTestAppUtils::ReadTestAppResultsFileL - exit"));
	}

/**
  Takes a result struct to be initialised to its default values.
  
  @param aTestAppResults - struct to be initialised
  */
EXPORT_C void CAppfwkSysstartTestAppUtils::InitTestAppResults(TAppfwkSysstartTestAppResults& aTestAppResults)
	{
	RDebug::Print(_L("CAppfwkSysstartTestAppUtils::InitTestAppResults - entry"));
	aTestAppResults.iStartCount=0;
	aTestAppResults.iRendCount=0;
	aTestAppResults.iRendFailCount=0;
	RDebug::Print(_L("CAppfwkSysstartTestAppUtils::InitTestAppResults - exit"));
	}

/**
  This function takes the passed in results and writes to the specified
  result file.
  
  The format of the result file is:
  App Start count=value1
  App Rendezvous count=value2
  App Rendezvous fail count=value3
  '=' is used as the delimiter.
  
  @param aFileName - the file to be replaced/written
  @param aTestAppResults - struct which contains results to be written
  */
EXPORT_C void CAppfwkSysstartTestAppUtils::ReplaceTestAppResultsFileL(const TDesC& aFileName, const TAppfwkSysstartTestAppResults& aTestAppResults)
	{
	RDebug::Print(_L("CAppfwkSysstartTestAppUtils::ReplaceTestAppResultsFileL - entry"));
	RFs myFs;
	User::LeaveIfError(myFs.Connect());
	
	RFileWriteStream writer;
	writer.PushL(); // writer on cleanup stack
	
	User::LeaveIfError(writer.Replace(myFs, aFileName, EFileWrite));
	
	// write data to file
	writer << _L("App Start count=");
	writer.WriteInt16L(aTestAppResults.iStartCount);
	writer << _L("\n");
	writer << _L("App Rendezvous count=");
	writer.WriteInt16L(aTestAppResults.iRendCount);
	writer << _L("\n");
	writer << _L("App Rendezvous fail count=");
	writer.WriteInt16L(aTestAppResults.iRendFailCount);
	writer << _L("\n");
	
	// print out results for debug purposes
	RDebug::Print(_L("CAppfwkSysstartTestAppUtils::ReplaceTestAppResultsFileL - aTestAppResults.iStartCount - %d"),     aTestAppResults.iStartCount);
	RDebug::Print(_L("CAppfwkSysstartTestAppUtils::ReplaceTestAppResultsFileL - aTestAppResults.iRendCount  - %d"),     aTestAppResults.iRendCount);
	RDebug::Print(_L("CAppfwkSysstartTestAppUtils::ReplaceTestAppResultsFileL - aTestAppResults.iRendFailCount  - %d"), aTestAppResults.iRendFailCount);
				
	writer.CommitL();
	CleanupStack::PopAndDestroy(&writer); // writer
	
	myFs.Close();
	RDebug::Print(_L("CAppfwkSysstartTestAppUtils::ReplaceTestAppResultsFileL - exit"));
	}
	
