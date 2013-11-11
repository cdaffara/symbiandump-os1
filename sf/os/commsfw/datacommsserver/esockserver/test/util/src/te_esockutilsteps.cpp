// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "te_esockutilsteps.h"
#include <test/testexecutelog.h>
#include "esocktestutils.h"
#include <es_sock.h>
#include <comms-infras/es_availability.h>

#include <e32base.h>
#include <cinidata.h>
#include <e32cmn.h>
#include <f32file.h>

#include <elements/sd_roles.h>

class TTruncateOverflow16 : public TDes16Overflow
	{
public:
	virtual void Overflow(TDes16&)
		{
		}
	};

class TTruncateOverflow8 : public TDes8Overflow
	{
public:
	virtual void Overflow(TDes8&)
		{
		}
	};

void CTestStepESockUtil::Log(TRefByValue<const TDesC16> aFormat, ...)
	{
    VA_LIST vaList;
	VA_START( vaList, aFormat );

	TTruncateOverflow16 truncateOverflow;
	TBuf<512> buf;
	buf.AppendFormatList( aFormat, vaList, &truncateOverflow );
	Logger().Write(buf);
	}

void CTestStepESockUtil::Log(TRefByValue<const TDesC8> aFormat, ...)
	{
    VA_LIST vaList;
	VA_START( vaList, aFormat );

	TTruncateOverflow8 truncateOverflow;
	TBuf8<512> buf;
	buf.AppendFormatList( aFormat, vaList, &truncateOverflow );
	Logger().Write(buf);
	}



CTestStepLoadESock::CTestStepLoadESock()
	{
	SetTestStepName(KTestStepLoadESock);
	}

/* virtual */ TVerdict CTestStepLoadESock::doTestStepL()
	{
	SetTestStepResult(EFail); 

	TInt err(ESockLoader::LoadESock(this));
	if(err!=KErrNone)
		{
		INFO_PRINTF2(_L("LoadESock returned error %d"), err);
		}
	else
		{
		SetTestStepResult(EPass);		
		}
	return TestStepResult();
	}

	CTestStepUnloadESock::CTestStepUnloadESock(CommsFW::TCFShutdownType aType, TLeak aLeak, 
												TBool aExpectTimeout):
	iType(aType),
	iLeak(aLeak),
	iExpectTimeout(aExpectTimeout)
	{
	SetTestStepName(KTestStepUnloadESock);
	}

/* virtual */ TVerdict CTestStepUnloadESock::doTestStepL()
	{
	SetTestStepResult(EFail); 

	TInt err(ESockLoader::UnloadESock(iType, this));
#ifdef ALWAYS_PASS_UNLOAD
	SetTestStepResult(EPass);	
#else
	switch (err)
		{
		case KErrNone:
			{
			if (iLeak == ELeaks)
				{
				INFO_PRINTF1(_L("We were expecting a leak and non occurred : check that the leak-detecion machnism is working."));
				}
			else if (iExpectTimeout)
				{
				INFO_PRINTF1(_L("We were expecting a timeout and non occurred"));
				}
			else
				{
				SetTestStepResult(EPass);	
				}
			break;
			}
		case KErrSessionClosed:
			{
			INFO_PRINTF2(_L("UnloadESock returned %d. ESock had still had unclosed sessions when exiting."), err);
			break;
			}
		case ESockLoader::KModuleLeaked:
			{
			INFO_PRINTF1(_L("A memory leak was detected on shutdown."));
			INFO_PRINTF1(_L("Expect leaks if a panic occurred - Check the epocwind.out."));
			if (iLeak == EMayLeak || iLeak == ELeaks)
				{
				SetTestStepResult(EPass);	
				}
			break;		
			}
		case KErrTimedOut:
			if (iExpectTimeout)
				{
				SetTestStepResult(EPass);
				}
			break;
		default:
			{
			INFO_PRINTF2(_L("UnloadESock returned error %d"), err);
			}
		}
#endif
		
	// Explicitly reset pubsub keys used for controlling bearer availability during testing
	const TInt KMaxLikelyAvailabilityAccessPoints = 32;
	const TInt KDummyNifTestingPubSubUid = 0x10272F42;
	INFO_PRINTF1(_L("Resetting availability pubsub keys"));
	for(TInt i = 0; i < KMaxLikelyAvailabilityAccessPoints; i++)
		{
		// Harmless if the key doesn't exist - so we can ignore the return code
		RProperty::Set(TUid::Uid(KDummyNifTestingPubSubUid), i, ESock::TAvailabilityStatus::EMaxAvailabilityScore);
		}
	
	return TestStepResult();
	}

CTestStepStopAllInterfaces::CTestStepStopAllInterfaces()
	{
	SetTestStepName(KTestStepStopAllInterfaces);
	}

/* virtual */ TVerdict CTestStepStopAllInterfaces::doTestStepL()
	{
	INFO_PRINTF2(_L("CTestStepStopAllInterfaces::doTestStepL %d"), 0);
	SetTestStepResult(EFail);

	TBuf<1000> errDesc;
	TInt err(ESockTestUtils::StopAllInterfaces(errDesc));
	if(err!=KErrNone)
		{
		INFO_PRINTF3(_L("StopAllInterfaces returned error %d:\n%S"), err, &errDesc);
		}
	else
		{
		SetTestStepResult(EPass);
		}
	return TestStepResult();
	}


CTestStepDefaultToConfig1::CTestStepDefaultToConfig1()
	{
	SetTestStepName(KTestStepDefaultToConfig1);
	}

/* virtual */ TVerdict CTestStepDefaultToConfig1::doTestStepL()
	{
	SetTestStepResult(EFail);	
	ESockTestUtils::DefaultToConfig1L();
	SetTestStepResult(EPass);
	return TestStepResult();
	}




//IsNetworkNotActive - at first seemingly strange func name-
//the reason is that the "pass" return is wanted if the network
//is not active - this is in keeping with the previous genconn func
//that this replaces called "networknotactive". 
//The call in this step to IsNetworkActiveL is a useful func now part of Utils.
CTestStepIsNetworkNotActive::CTestStepIsNetworkNotActive()
	{
	SetTestStepName(KTestStepIsNetworkNotActive);
	}


/* virtual */ TVerdict CTestStepIsNetworkNotActive::doTestStepL()
	{
	SetTestStepResult(EFail);
	TBool active = true;

	TRAPD(err, active = ESockTestUtils::IsNetworkActiveL());
	
	if(err)
		{
		INFO_PRINTF2(_L("IsNetworkActive() returned error %d"), err);
		}
	else
		{
		//IsNetworkActive() didn't give back error code let's check the
		//network status
		if (active)
		    INFO_PRINTF1(_L("Network is active"));
		else
		    SetTestStepResult(EPass);
		}
		
	return TestStepResult();
	}


CTestStepDelay::CTestStepDelay()
	{
	SetTestStepName(KTestStepDelay);
	}

/* virtual */ TVerdict CTestStepDelay::doTestStepL()
	{
	SetTestStepResult(EFail);

	//get val from config ini file
	TInt GuardTimerValue;
	TInt returnValue = GetIntFromConfig(_L("Common"),_L("Delay"),GuardTimerValue);


	TInt err(ESockTestUtils::Delay(GuardTimerValue));
	if(err!=KErrNone)
		{
		INFO_PRINTF2(_L("Delay() returned error %d"), err);
		}
	else
		{
		SetTestStepResult(EPass);
		}
	return TestStepResult();
	}

CTestStepAlwaysFail::CTestStepAlwaysFail()
	{
	SetTestStepName(KTestStepAlwaysFail);
	}

TVerdict CTestStepAlwaysFail::doTestStepL()
	{
	SetTestStepResult(EFail);
  	return TestStepResult();
	}

CTestStepResetAllocFails::CTestStepResetAllocFails()
	{
	SetTestStepName(KTestStepResetAllocFails);
	}

/* virtual */ TVerdict CTestStepResetAllocFails::doTestStepL()
	{
	SetTestStepResult(EFail);
	RSocketServ ss;

	TInt err = KErrGeneral;
	// This loop may repeat until the test step's timeout is exhausted.
	do {
		err = ss.Connect();
	} while (KErrNone != err);
	
	err = ss.__DbgFailNext(Den::KResetAllocFails);
	if (KErrNone == err)
		{
		SetTestStepResult(EPass);
		}
	ss.Close();
	
	return TestStepResult();
	}

CTestStepLoadConfig::CTestStepLoadConfig()
	{
	SetTestStepName(KTestStepLoadConfig);
	}

/* virtual */ TVerdict CTestStepLoadConfig::doTestStepL()
	{
	TVerdict verdict = EPass;
	// Fetch details of requested changes
	TPtrC fileSpec;
	TESTL(GetStringFromConfig(ConfigSection(), _L("fileSpec"), fileSpec));
	
	TPtrC sectName;
	TESTL(GetStringFromConfig(ConfigSection(), _L("sectName"), sectName));
	
	TPtrC keyName;
	TESTL(GetStringFromConfig(ConfigSection(), _L("keyName"), keyName));
	
	TPtrC value;
	TESTL(GetStringFromConfig(ConfigSection(), _L("value"), value));

	// Process files matching supplied file spec
	RFs fs;
	CleanupClosePushL(fs);
	TInt err = fs.Connect();
	TESTEL(KErrNone == err, err);

	CDir* dirList;
	err = fs.GetDir(fileSpec, KEntryAttNormal, ESortByName, dirList);
	CleanupStack::PushL(dirList);
	TESTEL(KErrNone == err, err);
	
	TParse fullSpec;
	fullSpec.Set(fileSpec, NULL, NULL);
	TBuf<256> curFileSpec;

	for (TInt i = 0; i < dirList->Count(); i++)
		{
		curFileSpec = fullSpec.DriveAndPath();
		curFileSpec.Append((*dirList)[i].iName);
		Logger().WriteFormat(_L("Processing %S..."), &curFileSpec);
		
		err = UpdateIniL(curFileSpec, sectName, keyName, value);
		TESTEL(KErrNone == err, err);
		}

	CleanupStack::PopAndDestroy(2, &fs);
	return verdict;
	}

TInt CTestStepLoadConfig::UpdateIniL(const TDesC &aFileSpec, const TDesC &aSectName, const TDesC &aKeyName, const TDesC &aValue)
	{
	// Ensure file is writeable.
	RFs fs;
	CleanupClosePushL(fs);
	User::LeaveIfError(fs.Connect());
	User::LeaveIfError(fs.SetAtt(aFileSpec, 0, KEntryAttReadOnly));

	TInt err;

	// Attempt to update the value for the specified key.
	CIniData* myIni = CIniData::NewL(aFileSpec);
	CleanupStack::PushL(myIni);
	
	TPtrC result;
	if (myIni->FindVar(aSectName, aKeyName, result))
		{
		err = myIni->SetValue(aSectName, aKeyName, aValue);
		}
	else
		{
		err = myIni->AddValue(aSectName, aKeyName, aValue);
		}
	User::LeaveIfError(err);
	myIni->WriteToFileL();
	CleanupStack::PopAndDestroy(1, myIni);

	// Replace all " =" with "=".
	
	RFile myFile;
	CleanupClosePushL(myFile);
	User::LeaveIfError(myFile.Open(fs, aFileSpec, EFileStreamText | EFileWrite));

	TInt fileSize;
	User::LeaveIfError(myFile.Size(fileSize));

	RBuf8 myBuf;
	CleanupClosePushL(myBuf);
	User::LeaveIfError(myBuf.Create(fileSize));
	User::LeaveIfError(myFile.Read(myBuf));

	_LIT8(KSpaceEquals, " =");
	TInt pos = 0;
	while (pos >= 0)
		{
		pos = myBuf.Find(KSpaceEquals);
		if (pos >= 0)
			{
			myBuf.Delete(pos, 1);
			}
		}

	pos = 0;
	User::LeaveIfError(myFile.Write(pos, myBuf));
	User::LeaveIfError(myFile.SetSize(myBuf.Length()));
	User::LeaveIfError(myFile.Size(fileSize));

	CleanupStack::PopAndDestroy(3, &fs);
	return err;
	}

CTestStepCopyFiles::CTestStepCopyFiles()
	{
	SetTestStepName(KTestStepCopyFiles);
	}

TVerdict CTestStepCopyFiles::doTestStepL()
	{
	TVerdict verdict = EPass;
	// Fetch details of files to be copied
	TPtrC sourceSpec;
	TESTL(GetStringFromConfig(ConfigSection(), _L("sourceSpec"), sourceSpec));

	TPtrC destPath;
	TESTL(GetStringFromConfig(ConfigSection(), _L("destPath"), destPath));

	RFs fs;
	CleanupClosePushL(fs);
	TInt err = fs.Connect();
	TESTEL(KErrNone == err, err);
	
	CFileMan *fm = CFileMan::NewL(fs);
	CleanupStack::PushL(fm);
	
	err = fm->Copy(sourceSpec, destPath);
	TESTEL(KErrNone == err, err);
	
	CleanupStack::PopAndDestroy(2, &fs);
	return verdict;
	}

