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
 @internalComponent
*/

#include <s32file.h>
#include "t_wservintegstepbase.h"
#include "t_wservconsts.h"

/**
 CT_WServIntegStep constructor.
 @return N/A
 @pre None
 @post CT_WServIntegStep 1st stage constructor
*/
CT_WServIntegStep::CT_WServIntegStep()
	{
	SetTestStepName(KT_WServIntegStep);
	}

CT_WServIntegStep::~CT_WServIntegStep()
	{
	iFs.Close();
	}

/**
Starts test step
@internalComponent
@return TVerdict pass / fail
@pre N/A
@post N/A
*/
enum TVerdict CT_WServIntegStep::doTestStepL()
	{
	__UHEAP_MARK;

	ERR_PRINTF1(_L("Executing Base class"));

	__UHEAP_MARKEND;

	return TestStepResult();
	}

void CT_WServIntegStep::WaitForTestL(const TDesC& aFileName, const TInt& aMaxTimeSeconds)
	{
	RFile myFile;
	TInt err = KErrNotFound;

	TInt endTimer = aMaxTimeSeconds*1000000; // convert to milli secs
	TInt myTimer = 0;

	err = myFile.Open(iFs,aFileName,EFileRead);

	while (err != KErrNone && myTimer < endTimer)
		{
		myTimer += 500000;
		User::After(500000);
		err = myFile.Open(iFs,aFileName,EFileRead);
		}


	if (err != KErrNotFound)
		{
		myFile.Close();
		iFileList.AppendL(&aFileName);
		}
	}

void CT_WServIntegStep::GetConfigDataL(const TDesC& aData, const TDesC& aName)
	{
	TBool								moreData=ETrue;
	TInt								index=0;
	TBuf<KMaxTestExecuteCommandLength>	tempStore;
	TPtrC								data;
	TPtrC								name;

	// populate config data with that contained in [default] section
	while ( moreData )
		{
		tempStore.Format(aData, ++index);
		moreData=GetStringFromConfig(KDefaultSectionName(), tempStore, data);
		if ( moreData )
			{
			tempStore.Format(aName, index);
			moreData=GetStringFromConfig(KDefaultSectionName(), tempStore, name);

			if ( moreData )
				{
				iWServConfigData.AddDataL(name, data);
				}
			else
				{
				ERR_PRINTF2(_L("data %S has no name"), &data);
				SetTestStepResult(EFail);
				}
			}
		}

	// update config data with that contained in the config section
	// appropriate to the test case being executed
	moreData=ETrue;
	index = 0;
	while ( moreData )
		{
		tempStore.Format(aData, ++index);
		moreData=GetStringFromConfig(ConfigSection(), tempStore, data);
		if ( moreData )
			{
			tempStore.Format(aName, index);
			moreData=GetStringFromConfig(ConfigSection(), tempStore, name);

			if ( moreData )
				{
				TRAPD(err,iWServConfigData.AddDataL(name, data));
				if (err==KErrAlreadyExists)
					{
					iWServConfigData.ReplaceData(name, data);
					}
				else
					{
					if (err != KErrNone)
						{
						User::Leave(err);
						}
					}
				}
			else
				{
				ERR_PRINTF2(_L("data %S has no name"), &data);
				SetTestStepResult(EFail);
				}
			}
		}
	}

void CT_WServIntegStep::PrintConfigDataL(const TDesC& aFileName)
	{
	RFileWriteStream writer;
	writer.PushL(); // writer on cleanup stack
	TInt err = iFs.MkDirAll(aFileName);
	TEST(err==KErrNone||err==KErrAlreadyExists);
	INFO_PRINTF2(_L("Create Config File: %S"), &aFileName);
	User::LeaveIfError(writer.Replace(iFs, aFileName, EFileStreamText|EFileWrite));
	writer.CommitL();
	CleanupStack::PopAndDestroy(&writer); // writer

	CIniData* myData=CIniData::NewL(aFileName);
	CleanupStack::PushL(myData);

	for (TInt index=0;index<iWServConfigData.NoOfEntriesL();index++)
		{
		TPtrC name = iWServConfigData.GetName(index);
		TPtrC data = iWServConfigData.GetData(index);
		INFO_PRINTF3(_L("Config Name: %S, \t\tConfig Data: %S"), &name, &data);

		TInt err = myData->AddValue(KDefaultSectionName, name, data);
		INFO_PRINTF3(_L("AddValue - Expected: %d, Actual: %d"), KErrNone, err);
		TEST(err==KErrNone);
		}

	myData->WriteToFileL();
	iFileList.AppendL(&aFileName);
	CleanupStack::PopAndDestroy(myData);
	}

void CT_WServIntegStep::CheckResultsL(const TDesC& aData, const TDesC& aName,const TDesC& aFileName)
	{
	GetConfigDataL(aData,aName);

	CIniData* myData=CIniData::NewL(aFileName);
	iFileList.AppendL(&aFileName);
	CleanupStack::PushL(myData);

	for (TInt index=0;index<iWServConfigData.NoOfEntriesL();index++)
		{
		TPtrC name = iWServConfigData.GetName(index);
		TPtrC data = iWServConfigData.GetData(index);

		TPtrC actData;
		TBool found = myData->FindVar(KDefaultSectionName, name, actData);
		if (found)
			{
			INFO_PRINTF4(_L("Result: %S, Expected: %S, Actual: %S"), &name, &data, &actData);
			TEST(actData==data);
			}
		else
			{
			#ifdef __WINS__
			ERR_PRINTF3(_L("NOT FOUND: Result: %S, \t\tExpected Value: %S"), &name, &data);
			#else
			// in the case of a non-debug build then KDsaRotationAbortTimeoutCount cannot
			// be measured, and hence is ignored.
			if (name.Compare(_L("KDsaRotationAbortTimeoutCount"))!=KErrNone)
				{
				ERR_PRINTF3(_L("NOT FOUND: Result: %S, \t\tExpected Value: %S"), &name, &data);
				}
			#endif
			}
		}

	CleanupStack::PopAndDestroy(myData);

	ClearConfigData();
	}

void CT_WServIntegStep::PrintResultsL(const TDesC& aFileName)
	{
	RFile resultFile;
	CleanupClosePushL(resultFile);
	User::LeaveIfError(resultFile.Open(iFs,aFileName,EFileRead));

	TInt size = 0;
	User::LeaveIfError(resultFile.Size(size));
	TText* data=(TText*)User::AllocL(size);
	CleanupStack::PushL(data);
	TPtr8 resultData((TUint8*)data, 0, size);;
	resultFile.Read(resultData);
	TUint8* ptr = (TUint8*)data;

	// NON-UNICODE so convert to UNICODE
	TText* newdata = (TText*)User::AllocL(size*sizeof(TText));
	CleanupStack::PushL(newdata);
	TPtr printData(newdata, size, size);
	for(TInt i=0 ; i<size ; ++i)
		{
		printData[i]=ptr[i];
		}

	INFO_PRINTF2(_L("Printing result file: %S"), &aFileName);
	INFO_PRINTF2(_L("%S"), &printData);

	CleanupStack::PopAndDestroy(3);

	//resultFile.Close();
	}

TInt CT_WServIntegStep::CreateTestProcessL(const TDesC& aApp, const TProcessPriority& aPriority)
	{
	INFO_PRINTF2(_L("Starting App: %S"), &aApp);

	RProcess process;
	TInt createErr = process.Create(aApp, KNullDesC);

	if (createErr == KErrNone)
		{
		process.SetPriority(aPriority);
		TEST(process.Priority()==aPriority);
		INFO_PRINTF3(_L("Process Priority: Actual: %d, Expected: %d"), process.Priority(), aPriority);
		process.Resume();
		iProcessList.AppendL(process);
		}

	return createErr;
	}

void CT_WServIntegStep::ClearConfigData()
	{
	iWServConfigData.Empty();
	}

void CT_WServIntegStep::CleanUp()
	{
	INFO_PRINTF1(_L("Test clean up"));

	TInt count = iProcessList.Count();

	TInt index;
	for (index = 0; index < count; index++)
		{
		// check process
		INFO_PRINTF3(_L("Process Check: Actual: %d, Expected: %d"), iProcessList[index].ExitReason(), KErrNone);
		TEST(iProcessList[index].ExitReason()==KErrNone);
		// kill process
		iProcessList[index].Kill(KErrGeneral);
		INFO_PRINTF3(_L("Process Exit Reason: Actual: %d, Expected: %d"), iProcessList[index].ExitReason(), KErrGeneral);
		TEST(iProcessList[index].ExitReason()==KErrGeneral);
		}

	iProcessList.Reset();

	count = iFileList.Count();
	for (index = 0; index < count; index++)
		{
		INFO_PRINTF2(_L("Deleting Config File Name: %S"), iFileList[index]);
		TInt err = iFs.Delete(*iFileList[index]);
		TEST(err==KErrNone);
		}

	iFileList.Reset();

	ClearConfigData();
	}

void CT_WServIntegStep::PrintTestStepName()
	{
	//Print out the test step name in epocwind.out
	RDebug::Print(_L("************************************************************"));
	RDebug::Print(_L("*** Starting Test Step %S"), &(ConfigSection()));
	// debug statement to indicate progress of test suite by
	// printing the sub-test that is currently active
	TTime timeStamp;
	timeStamp.HomeTime();
	TBuf<50> dateStr;
	_LIT(KDateString3,"%-B%:0%J%:1%T%:2%S%:3 %Cms");
	timeStamp.FormatL(dateStr, KDateString3);
	RDebug::Print(_L("*** Time Stamp - %S"), &dateStr);
	RDebug::Print(_L("************************************************************"));
	}

enum TVerdict CT_WServIntegStep::doTestStepPreambleL()
	{
	TVerdict	ret=CTestStep::doTestStepPreambleL();

	User::LeaveIfError(iFs.Connect());

	return ret;
	}

enum TVerdict CT_WServIntegStep::doTestStepPostambleL()
	{
	return TestStepResult();
	}
