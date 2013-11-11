// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This file contains an example Test step implementation 
// This demonstrates the various functions provided
// by the CTestStep base class which are available within
// a test step 
// 
//

// EPOC includes
#include <e32base.h>
#include <e32test.h>
#include <e32keys.h>
#include <c32comm.h>
#include <f32file.h>
#include <etel.h>
#include <etelmm.h>
#include <testframework.h>

//#include <MdaAudioSamplePlayer.h>
#include <mdaaudiosampleeditor.h>
//#include <MdaAudioTonePlayer.h>

// Test system includes
#include <testframework.h>

// Specific includes for this test suite
#include "TestStepUnitMMFAudClient.h"
#include "TestSuiteUnitMMFAudClient.h"

// Specific includes for these test steps
#include "TestStepRecorder.h"


#include "MmfAudioController.h"
#include "mmfclientaudiorecorder.h"
//#include "MmfClientAudioPlayer.h"
//#include <mmffourcc.h>
//#include <mmfpaniccodes.h>
//#include <mmfFormatImplementationUIDs.hrh>
//
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepRecorderGetNumMeta.
 *
 *
 * @return	"CTestStepRecorderGetNumMeta*"
 *			The constructed CTestStepRecorderGetNumMeta
 *
 * @xxxx
 * 
 */
CTestStepRecorderGetNumMeta* CTestStepRecorderGetNumMeta::NewL()
	{
	CTestStepRecorderGetNumMeta* self = new(ELeave) CTestStepRecorderGetNumMeta;
	return self;
	}

/**
 *
 * Test step constructor.
 * Each test step initialises its own name.
 *
 * @xxxx
 * 
 */
CTestStepRecorderGetNumMeta::CTestStepRecorderGetNumMeta()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0260-HP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepRecorderGetNumMeta::~CTestStepRecorderGetNumMeta()
	{
	}

/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepRecorderGetNumMeta::DoTestStepPreambleL(void)
	{
	 enum TVerdict verdict;
	 // this installs the scheduler
	 verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();

	// Printing to the console and log file
	INFO_PRINTF1(_L("MM-MMF-ACLNT-U-0260-HP"));
	INFO_PRINTF1(_L("this is a test of CMdaAudioRecorderUtility::GetNumberOfMetaDataEntries()"));

	if(!GetStringFromConfig(_L("SectionOne"), _L("AudioFNameMeta"), iFileNames[0])) // wav file
		{
		//INFO_PRINTF2(_L("file name %s not found..."), fileptr);
		return EInconclusive;
		}
	/*
	if(!GetStringFromConfig(_L("SectionOne"), _L("AudioPlayFName3"), iFileNames[1])) // au file
		{
		//INFO_PRINTF2(_L("file name %s not found..."), fileptr);
		return EInconclusive;
		}
	if(!GetStringFromConfig(_L("SectionOne"), _L("AudioPlayFName5"), iFileNames[2])) // raw file
		{
		//INFO_PRINTF2(_L("file name %s not found..."), fileptr);
		return EInconclusive;
		}
	*/
	return verdict;

	}

/**
 *
 * Test step Postamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepRecorderGetNumMeta::DoTestStepPostambleL(void)
	{
	delete iRecorder;
	iRecorder = NULL;
	//[ Destroy the scheduler ]
	return CTestStepUnitMMFAudClient::DoTestStepPostambleL();
	}

/**
 *
 * Callback Handle.
 *
 * @xxxx
 * 
 */
void CTestStepRecorderGetNumMeta::MoscoStateChangeEvent(CBase* /*aObject*/,
														  TInt /*aPreviousState*/,
														  TInt /*aCurrentState*/,
														  TInt aErrorcCode)
	{
	iError = aErrorcCode;
	CActiveScheduler::Stop();
	}

void CTestStepRecorderGetNumMeta::OpenAndStartSchedulerL(TInt i)
	{
	iRecorder->OpenFileL( iFileNames[i] );
	CActiveScheduler::Start(); 
	User::LeaveIfError(iError);
	}
	
/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 * @return	"TVerdict"
 *			The result of the test step
 *
 * @xxxx
 * 
 */
TVerdict CTestStepRecorderGetNumMeta::DoTestStepL()
	{
	iTestStepResult = EPass;
    TInt err       = KErrNone;

	//for (TInt type=0; type<3; type++)
	// just one type for the time being...
	for (TInt type=0; type<1; type++)
		{
		// create the Recorder utility
		if ( (iRecorder = CMMFMdaAudioRecorderUtility::NewL(*this)) == NULL )
			return EInconclusive;

		TRAP( err, OpenAndStartSchedulerL(type) );

		if (iRecorder == NULL ||
			err != KErrNone ||
			iError != KErrNone ||
			iRecorder->State() != CMdaAudioRecorderUtility::EOpen)
			return EInconclusive;

		TInt num;
		err = iRecorder->GetNumberOfMetaDataEntries(num);

#ifdef _TYPHOON_TEST_
		if (err != KErrNotSupported) 
			return EFail;
#endif

#ifndef _TYPHOON_TEST_
		if (err != KErrNone 
			// num == <known number of meta data>
			) // check a known number of meta data
			return EFail;
#endif

		delete iRecorder;
		iRecorder = NULL;
		}

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

//
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepRecorderGetMeta.
 *
 *
 * @return	"CTestStepRecorderGetNumMeta*"
 *			The constructed CTestStepRecorderGetNumMeta
 *
 * @xxxx
 * 
 */
CTestStepRecorderGetMeta* CTestStepRecorderGetMeta::NewL()
	{
	CTestStepRecorderGetMeta* self = new(ELeave) CTestStepRecorderGetMeta;
	return self;
	}

/**
 *
 * Test step constructor.
 * Each test step initialises its own name.
 *
 * @xxxx
 * 
 */
CTestStepRecorderGetMeta::CTestStepRecorderGetMeta()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0261-HP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepRecorderGetMeta::~CTestStepRecorderGetMeta()
	{
	}

/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepRecorderGetMeta::DoTestStepPreambleL(void)
	{
	 enum TVerdict verdict;
	 // this installs the scheduler
	 verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();

	// Printing to the console and log file
	INFO_PRINTF1(_L("MM-MMF-ACLNT-U-0261-HP"));
	INFO_PRINTF1(_L("this is a test of CMdaAudioRecorderUtility::GetMetaDataEntry()"));

	if(!GetStringFromConfig(_L("SectionOne"), _L("AudioFNameMeta"), iFileNames[0])) // wav file
		{
		//INFO_PRINTF2(_L("file name %s not found..."), fileptr);
		return EInconclusive;
		}

	// create the Recorder utility
	if ( (iRecorder = CMMFMdaAudioRecorderUtility::NewL(*this)) == NULL )
		return EInconclusive;

	TInt err;
	TRAP( err, OpenAndStartSchedulerL(0) );

	if (iRecorder == NULL ||
		err != KErrNone ||
		iError != KErrNone ||
		iRecorder->State() != CMdaAudioRecorderUtility::EOpen)
		return EInconclusive;

	err = iRecorder->GetNumberOfMetaDataEntries(iNum);

#ifdef _TYPHOON_TEST_
		if (err != KErrNotSupported) 
			return EInconclusive;
		iNum = 0;
#endif

#ifndef _TYPHOON_TEST_
	if (err != KErrNone ||
		iNum == 0
		) // check a known number of meta data
		return EInconclusive;
#endif

		
	return verdict;

	}

/**
 *
 * Test step Postamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepRecorderGetMeta::DoTestStepPostambleL(void)
	{
	delete iRecorder;
	iRecorder = NULL;
	//[ Destroy the scheduler ]
	return CTestStepUnitMMFAudClient::DoTestStepPostambleL();
	}

/**
 *
 * Callback Handle.
 *
 * @xxxx
 * 
 */
void CTestStepRecorderGetMeta::MoscoStateChangeEvent(CBase* /*aObject*/,
														  TInt /*aPreviousState*/,
														  TInt /*aCurrentState*/,
														  TInt aErrorcCode)
	{
	iError = aErrorcCode;
	CActiveScheduler::Stop();
	}

void CTestStepRecorderGetMeta::OpenAndStartSchedulerL(TInt i)
	{
	iRecorder->OpenFileL( iFileNames[i] );
	CActiveScheduler::Start(); 
	}
	
/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 * @return	"TVerdict"
 *			The result of the test step
 *
 * @xxxx
 * 
 */
TVerdict CTestStepRecorderGetMeta::DoTestStepL()
	{
	iTestStepResult = EPass;
    TInt err       = KErrNone;
	CMMFMetaDataEntry* meta = NULL;

	for (TInt i = 0; i< iNum; i++)
		{
		TRAP(err, meta = iRecorder->GetMetaDataEntryL(i) );
		if (meta == NULL || err != KErrNone)
			return EFail;
		else // maybe check some known meta data names and values
			{
			delete meta;
			meta = NULL;
			}
		}


	delete iRecorder;
	iRecorder = NULL;

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}
//
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepRecorderAddDelMeta.
 *
 *
 * @return	"CTestStepRecorderAddDelMeta*"
 *			The constructed CTestStepRecorderAddDelMeta
 *
 * @xxxx
 * 
 */
CTestStepRecorderAddDelMeta* CTestStepRecorderAddDelMeta::NewL()
	{
	CTestStepRecorderAddDelMeta* self = new(ELeave) CTestStepRecorderAddDelMeta;
	return self;
	}

/**
 *
 * Test step constructor.
 * Each test step initialises its own name.
 *
 * @xxxx
 * 
 */
CTestStepRecorderAddDelMeta::CTestStepRecorderAddDelMeta()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0262-HP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepRecorderAddDelMeta::~CTestStepRecorderAddDelMeta()
	{
	}

/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepRecorderAddDelMeta::DoTestStepPreambleL(void)
	{
	 enum TVerdict verdict;
	 // this installs the scheduler
	 verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();

	// Printing to the console and log file
	INFO_PRINTF1(_L("MM-MMF-ACLNT-U-0262-HP"));
	INFO_PRINTF1(_L("this is a test of CMdaAudioRecorderUtility::[ADD|Remove]MetaDataEntry()"));

	if(!GetStringFromConfig(_L("SectionOne"), _L("AudioFNameMeta"), iFileName)) // wav file
		{
		//INFO_PRINTF2(_L("file name %s not found..."), fileptr);
		return EInconclusive;
		}

	// create the Recorder utility
	if ( (iRecorder = CMMFMdaAudioRecorderUtility::NewL(*this)) == NULL )
		return EInconclusive;

	TInt err;
	TRAP( err, OpenAndStartSchedulerL() );

	if (iRecorder == NULL ||
		err != KErrNone ||
		iError != KErrNone ||
		iRecorder->State() != CMdaAudioRecorderUtility::EOpen)
		return EInconclusive;

	err = iRecorder->GetNumberOfMetaDataEntries(iNum);

#ifdef _TYPHOON_TEST_
		if (err != KErrNotSupported) 
			return EInconclusive;
		iNum = 0;
#endif

#ifndef _TYPHOON_TEST_
	if (err != KErrNone ||
		iNum == 0
		) // check a known number of meta data
		return EInconclusive;
#endif
		
	return verdict;

	}

/**
 *
 * Test step Postamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepRecorderAddDelMeta::DoTestStepPostambleL(void)
	{
	delete iRecorder;
	iRecorder = NULL;
	//[ Destroy the scheduler ]
	return CTestStepUnitMMFAudClient::DoTestStepPostambleL();
	}

/**
 *
 * Callback Handle.
 *
 * @xxxx
 * 
 */
void CTestStepRecorderAddDelMeta::MoscoStateChangeEvent(CBase* /*aObject*/,
														  TInt /*aPreviousState*/,
														  TInt /*aCurrentState*/,
														  TInt aErrorcCode)
	{
	iError = aErrorcCode;
	CActiveScheduler::Stop();
	}

void CTestStepRecorderAddDelMeta::OpenAndStartSchedulerL()
	{
	iRecorder->OpenFileL( iFileName );
	CActiveScheduler::Start(); 
	}
	
/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 * @return	"TVerdict"
 *			The result of the test step
 *
 * @xxxx
 * 
 */
TVerdict CTestStepRecorderAddDelMeta::DoTestStepL()
	{
	iTestStepResult = EPass;
    //TInt err       = KErrNone;
    //TInt err2       = KErrNone;
	//TInt num;

#ifdef _TYPHOON_TEST_
	return iTestStepResult; //  ;-)
#endif
/*
#ifndef _TYPHOON_TEST_
	_LIT(KMetaName, "Author"); // "Test MetaData Name");
	_LIT(KMetaValue, "Vangelis"); //"Test MetaData Value");
	CMMFMetaDataEntry* meta = CMMFMetaDataEntry::NewL(KMetaName, KMetaValue);
	CleanupStack::PushL(meta);

	// add the metadata
	iRecorder->AddMetaDataEntryL(*meta);

	err = iRecorder->GetNumberOfMetaDataEntries(num);
	if (err != KErrNone) 
		return EInconclusive;
	if (num != iNum+1 ||
		iRecorder->GetMetaDataEntryL(num-1)->Name() != KMetaName || // check if they count from 0
		iRecorder->GetMetaDataEntryL(num-1)->Value() != KMetaValue)
		return EFail;

	//remove the added metadata entry now
	err = iRecorder->RemoveMetaDataEntry(num-1);
	err2 = iRecorder->GetNumberOfMetaDataEntries(num);
	if (err2 != KErrNone) 
		return EInconclusive;
	if (err != KErrNone || iNum != num )
		return EFail;

	CleanupStack::PopAndDestroy(meta);


	delete iRecorder;
	iRecorder = NULL;

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
#endif
	*/
	}

//
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepRecorderReplaceMeta.
 *
 *
 * @return	"CTestStepRecorderReplaceMeta*"
 *			The constructed CTestStepRecorderReplaceMeta
 *
 * @xxxx
 * 
 */
CTestStepRecorderReplaceMeta* CTestStepRecorderReplaceMeta::NewL()
	{
	CTestStepRecorderReplaceMeta* self = new(ELeave) CTestStepRecorderReplaceMeta;
	return self;
	}

/**
 *
 * Test step constructor.
 * Each test step initialises its own name.
 *
 * @xxxx
 * 
 */
CTestStepRecorderReplaceMeta::CTestStepRecorderReplaceMeta()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0263-HP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepRecorderReplaceMeta::~CTestStepRecorderReplaceMeta()
	{
	}

/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepRecorderReplaceMeta::DoTestStepPreambleL(void)
	{
	 TVerdict verdict;
	 // this installs the scheduler
	 verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();

	// Printing to the console and log file
	INFO_PRINTF1(_L("MM-MMF-ACLNT-U-0263-HP"));
	INFO_PRINTF1(_L("this is a test of CMdaAudioRecorderUtility::ReplaceMetaDataEntry()"));

	if(!GetStringFromConfig(_L("SectionOne"), _L("AudioPlayFName7"), iFileName)) // wav file
		{
		//INFO_PRINTF2(_L("file name %s not found..."), fileptr);
		return EInconclusive;
		}

	return verdict;

	}

/**
 *
 * Test step Postamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepRecorderReplaceMeta::DoTestStepPostambleL(void)
	{
#ifndef _TYPHOON_TEST_
	TInt err = KErrNone;
	TInt err2 = KErrNone;
	TInt num;

	//remove the added and replaced metadata entry now
	err = iRecorder->RemoveMetaDataEntry(iNum);
	err2 = iRecorder->GetNumberOfMetaDataEntries(num);
	if (err2 != KErrNone) 
		return EInconclusive;
	if (err != KErrNone || iNum != num )
		return EInconclusive;
#endif

	delete iRecorder;
	iRecorder = NULL;

	//[ Destroy the scheduler ]
	return CTestStepUnitMMFAudClient::DoTestStepPostambleL();
	}

/**
 *
 * Callback Handle.
 *
 * @xxxx
 * 
 */
void CTestStepRecorderReplaceMeta::MoscoStateChangeEvent(CBase* /*aObject*/,
														  TInt /*aPreviousState*/,
														  TInt /*aCurrentState*/,
														  TInt aErrorcCode)
	{
	iError = aErrorcCode;
	CActiveScheduler::Stop();
	}

void CTestStepRecorderReplaceMeta::OpenAndStartSchedulerL()
	{
	iRecorder->OpenFileL( iFileName );
	CActiveScheduler::Start(); 
	}
	
/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 * @return	"TVerdict"
 *			The result of the test step
 *
 * @xxxx
 * 
 */
TVerdict CTestStepRecorderReplaceMeta::DoTestStepL()
	{
	iTestStepResult = EPass;

	TInt err       = KErrNone;


	// create the Recorder utility
	if ( (iRecorder = CMMFMdaAudioRecorderUtility::NewL(*this)) == NULL )
		return EInconclusive;


	TRAP( err, OpenAndStartSchedulerL() );

	if (iRecorder == NULL ||
		err != KErrNone ||
		iError != KErrNone ||
		iRecorder->State() != CMdaAudioRecorderUtility::EOpen)
		return EInconclusive;

	err = iRecorder->GetNumberOfMetaDataEntries(iNum);

#ifdef _TYPHOON_TEST_
	iNum = 0;
	if (err == KErrNotSupported) 
		return iTestStepResult;
	else if (err != KErrNone) // check a known number of meta data
		return EInconclusive;
#else
	if (err != KErrNone) // check a known number of meta data
		return EInconclusive;
#endif

	// add a metadata entry to be replaced in the DoTestStepL
	_LIT(KMetaName, "Test MetaData Name");
	_LIT(KMetaValue, "Test MetaData Value");
	CMMFMetaDataEntry* meta = CMMFMetaDataEntry::NewL(KMetaName, KMetaValue);
	CleanupStack::PushL(meta);

	// add the metadata
	iRecorder->AddMetaDataEntryL(*meta);

	// check if it has been added correctly
	TInt num;
	err = iRecorder->GetNumberOfMetaDataEntries(num);
	if (err != KErrNone) 
		return EInconclusive;
	if (num != iNum+1 ||
		iRecorder->GetMetaDataEntryL(num-1)->Name() != KMetaName || // check if they count from 0
		iRecorder->GetMetaDataEntryL(num-1)->Value() != KMetaValue)
		return EInconclusive;

	CleanupStack::PopAndDestroy(meta);
		
	_LIT(KMetaName2, "Test MetaData Name2");
	_LIT(KMetaValue2, "Test MetaData Value2");
	meta = CMMFMetaDataEntry::NewL(KMetaName2, KMetaValue2);
	CleanupStack::PushL(meta);

	// replace the metadata
	TRAP(err, iRecorder->ReplaceMetaDataEntryL(iNum, *meta));

#ifdef _TYPHOON_TEST_
	if (err != KErrNotSupported)
		{
		iTestStepResult = EFail;
		goto EndTest;
		}
#endif
#ifndef _TYPHOON_TEST_
	TInt num;
	err = iRecorder->GetNumberOfMetaDataEntries(num);
	if (err != KErrNone)
		{
		iTestStepResult = EInconclusive;
		goto EndTest;
		}
	if (num != iNum+1 ||
		iRecorder->GetMetaDataEntryL(num-1)->Name() != KMetaName2 || // check if they count from 0
		iRecorder->GetMetaDataEntryL(num-1)->Value() != KMetaValue2)
		{
		iTestStepResult = EFail;
		goto EndTest;
		}
#endif
	// metadata to be removed in the postambleL

EndTest:
	CleanupStack::PopAndDestroy(meta);

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}



