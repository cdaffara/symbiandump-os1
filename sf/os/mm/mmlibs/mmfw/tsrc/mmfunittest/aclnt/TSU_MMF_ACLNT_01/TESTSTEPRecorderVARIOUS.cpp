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

#include <mdaaudiosampleeditor.h>
//#include <MdaAudioToneRecorder.h>

// Test system includes
#include <testframework.h>

// Specific includes for this test suite
#include "TestStepUnitMMFAudClient.h"
#include "TestSuiteUnitMMFAudClient.h"

// Specific includes for these test steps
#include "TestStepRecorder.h"

#include "mmfclientaudiorecorder.h"
#include "../../ctlfrm/TS_Codes.h"

_LIT(KAudioControllerName,"Symbian Audio Wav Test controller");

//
//
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepRecorderGetImplementationInfo.
 *
 *
 * @return	"CTestStepRecorderGetImplementationInfo*"
 *			The constructed CTestStepRecorderGetImplementationInfo
 *
 * @xxxx
 * 
 */
CTestStepRecorderGetImplementationInfo* CTestStepRecorderGetImplementationInfo::NewL()
	{
	CTestStepRecorderGetImplementationInfo* self = new(ELeave) CTestStepRecorderGetImplementationInfo;
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
CTestStepRecorderGetImplementationInfo::CTestStepRecorderGetImplementationInfo() 
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0271-HP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepRecorderGetImplementationInfo::~CTestStepRecorderGetImplementationInfo()
	{
	}

void CTestStepRecorderGetImplementationInfo::MoscoStateChangeEvent(CBase* /*aObject*/,
														  TInt aPreviousState,
														  TInt aCurrentState,
														  TInt aErrorcCode)
	{
	iError = aErrorcCode;
	if (aPreviousState == CMdaAudioClipUtility::ENotReady
		&& aCurrentState == CMdaAudioClipUtility::EOpen)
		iIsOpen = ETrue;
	CActiveScheduler::Stop();
	}
	

/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepRecorderGetImplementationInfo::DoTestStepPreambleL(void)
	{
	enum TVerdict verdict;
	// this installs the scheduler
	verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();
	if (verdict != EPass)
		return verdict;

	 if(!GetStringFromConfig(_L("SectionOne"), _L("AudioPlayFName1"), iFileName))
		{
		 //INFO_PRINTF2(_L("file name %s not found..."), fileptr);
		 return EInconclusive;
		}

	// Printing to the console and log file
	INFO_PRINTF1(iTestStepName);
	INFO_PRINTF1(_L("this test is a test of CMMFMdaAudioRecorderUtility::ControllerImplementationInformationL()"));

	return verdict;
	}

/**
 *
 * Test step Postamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepRecorderGetImplementationInfo::DoTestStepPostambleL(void)
	{
	delete iRecorder;
	iRecorder = NULL;
	//[ Destroy the scheduler ]
	return CTestStepUnitMMFAudClient::DoTestStepPostambleL();
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
TVerdict CTestStepRecorderGetImplementationInfo::DoTestStepL()
	{
	TInt err;
	err = KErrNone;
	iTestStepResult = EFail;

	iRecorder = CMMFMdaAudioRecorderUtility::NewL(*this);
	
	TRAP(err, iRecorder->OpenFileL(iFileName));
	
	if (err != KErrNone) 
		return EInconclusive;

	CActiveScheduler::Start();

	if (iError != KErrNone) 
		return EInconclusive;

	const CMMFControllerImplementationInformation* info = NULL;
	
	TRAP(err, info = &iRecorder->AudioPlayControllerImplementationInformationL());
	if (err != KErrNone)
		return EFail;

	if (info->DisplayName().Compare(KAudioControllerName)!=0)
		iTestStepResult = EFail;
	else
		iTestStepResult = EPass;
		
	TRAP(err, info = &iRecorder->AudioRecorderControllerImplementationInformationL());
	if (err != KErrNone)
		return EFail;

	if (info->DisplayName().Compare(KAudioControllerName)!=0)
		iTestStepResult = EFail;
	else
		iTestStepResult = EPass;

	
	delete iRecorder;
	iRecorder = NULL;

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

//
//
//
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepRecorderSendCustomCommandSync.
 *
 *
 * @return	"CTestStepRecorderSendCustomCommandSync*"
 *			The constructed CTestStepRecorderSendCustomCommandSync
 *
 * @xxxx
 * 
 */
CTestStepRecorderSendCustomCommandSync* CTestStepRecorderSendCustomCommandSync::NewL()
	{
	CTestStepRecorderSendCustomCommandSync* self = new(ELeave) CTestStepRecorderSendCustomCommandSync;
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
CTestStepRecorderSendCustomCommandSync::CTestStepRecorderSendCustomCommandSync()
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0272-HP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepRecorderSendCustomCommandSync::~CTestStepRecorderSendCustomCommandSync()
	{
	}

void CTestStepRecorderSendCustomCommandSync::MoscoStateChangeEvent(CBase* /*aObject*/,
														  TInt aPreviousState,
														  TInt aCurrentState,
														  TInt aErrorcCode)
	{
	iError = aErrorcCode;
	if (aPreviousState == CMdaAudioClipUtility::ENotReady
		&& aCurrentState == CMdaAudioClipUtility::EOpen)
		iIsOpen = ETrue;
		
	CActiveScheduler::Stop();
	}
	
/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepRecorderSendCustomCommandSync::DoTestStepPreambleL(void)
	{
	enum TVerdict verdict;
	// this installs the scheduler
	verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();
	if (verdict != EPass)
		return verdict;

	if(!GetStringFromConfig(_L("SectionOne"), _L("AudioFNameTestController"), iFileName))
		{
		 //INFO_PRINTF2(_L("file name %s not found..."), fileptr);
		 return EInconclusive;
		}

	// Printing to the console and log file
	INFO_PRINTF1(iTestStepName);
	INFO_PRINTF1(_L("this test is a test of CMMFMdaAudioRecorderUtility::CustomCommandSync()"));

	return verdict;
	}

/**
 *
 * Test step Postamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepRecorderSendCustomCommandSync::DoTestStepPostambleL(void)
	{
	delete iRecorder;
	iRecorder = NULL;
	//[ Destroy the scheduler ]
	return CTestStepUnitMMFAudClient::DoTestStepPostambleL();
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
TVerdict CTestStepRecorderSendCustomCommandSync::DoTestStepL()
	{
	TInt err;
	err = KErrNone;
	iTestStepResult = EFail;

	iRecorder = CMMFMdaAudioRecorderUtility::NewL(*this);
	
	TUid testController = TUid::Uid(KUidTestController);
	TRAP(err, iRecorder->OpenFileL(iFileName,
									testController,
									testController,
									KNullUid));
	
	if (err!=KErrNone)
		return EInconclusive;
		
	CActiveScheduler::Start();
	
	if (iError!=KErrNone)
		return EInconclusive;	

	TUid uid;
	uid = TUid::Uid(0x101f72B4);
	TMMFMessageDestinationPckg destPckg(uid);
	// test without a return value
	err = iRecorder->RecordControllerCustomCommandSync(destPckg,
								EMMFTestCustomCommandGeneralSynchronous,
								KFirstParameter,
								KSecondParameter);
								
	if (err!=KTestCustomCommandSuccess)
		return EFail;
		
	TBuf8<256> buf;
		
	err = iRecorder->RecordControllerCustomCommandSync(destPckg,
								EMMFTestCustomCommandGeneralSynchronousWithReturn,
								KFirstParameter,
								KSecondParameter,
								buf);
	
	if (err!=KTestCustomCommandSuccess)
		return EFail;
		
	if (buf.Compare(KReturnParameter)==0)
		iTestStepResult = EPass;
	else
		iTestStepResult = EFail;

	
	err = iRecorder->PlayControllerCustomCommandSync(destPckg,
								EMMFTestCustomCommandGeneralSynchronous,
								KFirstParameter,
								KSecondParameter);
								
	if (err!=KTestCustomCommandSuccess)
		return EFail;
		
	err = iRecorder->PlayControllerCustomCommandSync(destPckg,
								EMMFTestCustomCommandGeneralSynchronousWithReturn,
								KFirstParameter,
								KSecondParameter,
								buf);
	
	if (err!=KTestCustomCommandSuccess)
		return EFail;
		
	if (buf.Compare(KReturnParameter)==0)
		iTestStepResult = EPass;
	else
		iTestStepResult = EFail;
		
	delete iRecorder;
	iRecorder = NULL;


	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}
	
	
//
//
//
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepRecorderSendCustomCommandAsync.
 *
 *
 * @return	"CTestStepRecorderSendCustomCommandAsync*"
 *			The constructed CTestStepRecorderSendCustomCommandAsync
 *
 * @xxxx
 * 
 */
CTestStepRecorderSendCustomCommandAsync* CTestStepRecorderSendCustomCommandAsync::NewL()
	{
	CTestStepRecorderSendCustomCommandAsync* self = new(ELeave) CTestStepRecorderSendCustomCommandAsync;
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
CTestStepRecorderSendCustomCommandAsync::CTestStepRecorderSendCustomCommandAsync()
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0273-HP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepRecorderSendCustomCommandAsync::~CTestStepRecorderSendCustomCommandAsync()
	{
	}

/**
 *
 * Test step Close
 * handles all deletion
 * @xxxx
 * 
 */
void CTestStepRecorderSendCustomCommandAsync::Close()
	{
	delete iAsyncObserver;
	}


/**
 * Implementation of the MVideoRecorderUtilityObserver interface functions
 **/
void CTestStepRecorderSendCustomCommandAsync::MoscoStateChangeEvent(CBase* /*aObject*/,
														  TInt aPreviousState,
														  TInt aCurrentState,
														  TInt aErrorcCode)
	{
	iError = aErrorcCode;
	if (aPreviousState == CMdaAudioClipUtility::ENotReady
		&& aCurrentState == CMdaAudioClipUtility::EOpen)
		iIsOpen = ETrue;
		
	CActiveScheduler::Stop();
	}
	
/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepRecorderSendCustomCommandAsync::DoTestStepPreambleL(void)
	{
	enum TVerdict verdict;
	// this installs the scheduler
	verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();
	if (verdict != EPass)
		return verdict;

	if(!GetStringFromConfig(_L("SectionOne"), _L("AudioFNameTestController"), iFileName))
		{
		 //INFO_PRINTF2(_L("file name %s not found..."), fileptr);
		 return EInconclusive;
		}
		
	iAsyncObserver = new (ELeave) CAsyncObserver;

	// Printing to the console and log file
	INFO_PRINTF1(iTestStepName);
	INFO_PRINTF1(_L("this test is a test of CMMFMdaAudioRecorderUtility::CustomCommandAsync()"));

	return verdict;
	}

/**
 *
 * Test step Postamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepRecorderSendCustomCommandAsync::DoTestStepPostambleL(void)
	{
	delete iRecorder;
	iRecorder = NULL;
	//[ Destroy the scheduler ]
	return CTestStepUnitMMFAudClient::DoTestStepPostambleL();
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
TVerdict CTestStepRecorderSendCustomCommandAsync::DoTestStepL()
	{
	TInt err;
	err = KErrNone;
	iTestStepResult = EFail;

	iRecorder = CMMFMdaAudioRecorderUtility::NewL(*this);
	
	TUid testController= TUid::Uid(KUidTestController);
	
	TRAP(err, iRecorder->OpenFileL(iFileName,
									testController,
									testController,
									KNullUid));
	if (err!=KErrNone)
		return EInconclusive;
		
	CActiveScheduler::Start();
	
	if (iError!=KErrNone)
		return EInconclusive;	

	TUid uid;
	uid = TUid::Uid(0x101f72B4);
	// destinationPckg can be temporary as the asynchronous custom command returns before the temporary variable goes out of scope
	TMMFMessageDestinationPckg destinationPckg(uid);
	
	// test without a return value
	iRecorder->RecordControllerCustomCommandAsync(destinationPckg,
								EMMFTestCustomCommandGeneralAsynchronous,
								KFirstParameter,
								KSecondParameter,
								iAsyncObserver->ActiveStatus());

	CActiveScheduler::Start();	
	if (iAsyncObserver->iStatus.Int() !=KTestCustomCommandSuccess)
		return EFail;
								
	TBuf8<256> buf;
		
	iRecorder->RecordControllerCustomCommandAsync(destinationPckg,
								EMMFTestCustomCommandGeneralAsynchronousWithReturn,
								KFirstParameter,
								KSecondParameter,
								buf,
								iAsyncObserver->ActiveStatus());

	CActiveScheduler::Start();	
	if (iAsyncObserver->iStatus.Int() !=KTestCustomCommandSuccess)
		return EFail;
		
	if (buf.Compare(KReturnParameter)==0)
		iTestStepResult = EPass;
	else
		iTestStepResult = EFail;
		
		// test without a return value
	iRecorder->PlayControllerCustomCommandAsync(destinationPckg,
								EMMFTestCustomCommandGeneralAsynchronous,
								KFirstParameter,
								KSecondParameter,
								iAsyncObserver->ActiveStatus());

	CActiveScheduler::Start();	
	if (iAsyncObserver->iStatus.Int() !=KTestCustomCommandSuccess)
		return EFail;
								
	iRecorder->PlayControllerCustomCommandAsync(destinationPckg,
								EMMFTestCustomCommandGeneralAsynchronousWithReturn,
								KFirstParameter,
								KSecondParameter,
								buf,
								iAsyncObserver->ActiveStatus());

	CActiveScheduler::Start();	
	if (iAsyncObserver->iStatus.Int() !=KTestCustomCommandSuccess)
		return EFail;
		
	if (buf.Compare(KReturnParameter)==0)
		iTestStepResult = EPass;
	else
		iTestStepResult = EFail;
		
	delete iRecorder;
	iRecorder = NULL;


	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

//
//
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepRecorderReloadingTest.
 *
 *
 * @return	"CTestStepRecorderReloadingTest*"
 *			The constructed CTestStepRecorderReloadingTest
 *
 * @xxxx
 * 
 */
CTestStepRecorderReloadingTest* CTestStepRecorderReloadingTest::NewL()
	{
	CTestStepRecorderReloadingTest* self = new(ELeave) CTestStepRecorderReloadingTest;
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
CTestStepRecorderReloadingTest::CTestStepRecorderReloadingTest()
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0274-HP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepRecorderReloadingTest::~CTestStepRecorderReloadingTest()
	{
	}

void CTestStepRecorderReloadingTest::MoscoStateChangeEvent(CBase* /*aObject*/,
														  TInt aPreviousState,
														  TInt aCurrentState,
														  TInt aErrorcCode)
	{
	iError = aErrorcCode;
	if (aPreviousState == CMdaAudioClipUtility::ENotReady
		&& aCurrentState == CMdaAudioClipUtility::EOpen)
		iIsOpen = ETrue;
		
	CActiveScheduler::Stop();
	}
	
void CTestStepRecorderReloadingTest::MaloLoadingStarted()
	{
	iLoadingStarted = ETrue;
	CActiveScheduler::Stop();
	}
	
void CTestStepRecorderReloadingTest::MaloLoadingComplete()
	{
	iLoadingComplete = ETrue;
	CActiveScheduler::Stop();
	}




/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepRecorderReloadingTest::DoTestStepPreambleL(void)
	{
	enum TVerdict verdict;
	// this installs the scheduler
	verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();
	if (verdict != EPass)
		return verdict;

	if(!GetStringFromConfig(_L("SectionOne"), _L("AudioFNameTestController"), iFileName))
		{
		 //INFO_PRINTF2(_L("file name %s not found..."), fileptr);
		 return EInconclusive;
		}

	// Printing to the console and log file
	INFO_PRINTF1(iTestStepName);
	INFO_PRINTF1(_L("this test simulates Audio Reloading and tests CMMFMdaAudioRecorderUtility::GetAudioLoadingProgressL"));

	return verdict;
	}

/**
 *
 * Test step Postamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepRecorderReloadingTest::DoTestStepPostambleL(void)
	{
	delete iRecorder;
	iRecorder = NULL;
	//[ Destroy the scheduler ]
	return CTestStepUnitMMFAudClient::DoTestStepPostambleL();
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
TVerdict CTestStepRecorderReloadingTest::DoTestStepL()
	{
	TInt err;
	err = KErrNone;
	iTestStepResult = EFail;

	iRecorder = CMMFMdaAudioRecorderUtility::NewL(*this);

	TUid testController = TUid::Uid(KUidTestController);
	TRAP(err, iRecorder->OpenFileL(iFileName,
									testController,
									testController,
									KNullUid));
	if (err!=KErrNone)
		return EInconclusive;
		
	CActiveScheduler::Start();
	
	if (iError!=KErrNone)
		return EInconclusive;	
		
	iRecorder->RegisterForAudioLoadingNotification(*this);

	TUid uid;
	uid = TUid::Uid(0x101f72B4);
	TMMFMessageDestinationPckg destPckg(uid);
	// test without a return value
	err = iRecorder->PlayControllerCustomCommandSync(destPckg,
								EMMFTestCustomCommandSimulateReloading,
								KNullDesC8,
								KNullDesC8);
								
	if (err!=KErrNone)
		return EInconclusive;
		
	CActiveScheduler::Start();
	
	if (!iLoadingStarted)
		return EFail;
		
	TInt percentageComplete = 0;
	TInt oldPercentageComplete =0;
	while (percentageComplete!=100 && percentageComplete>=oldPercentageComplete)
		{
		oldPercentageComplete = percentageComplete;
		iRecorder->GetAudioLoadingProgressL(percentageComplete);
		INFO_PRINTF2(_L("Loading progress: %d"),percentageComplete);
		User::After(100);
		}

	CActiveScheduler::Start();
		
	if (iLoadingComplete && percentageComplete==100)
		iTestStepResult = EPass;
	else
		iTestStepResult = EFail;

	delete iRecorder;
	iRecorder = NULL;

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}


/**
 *
 * Static constructor for CTestStepRecorderCheckPlayImplementationInfo.
 *
 *
 * @return	"CTestStepRecorderCheckPlayImplementationInfo*"
 *			The constructed CTestStepRecorderCheckPlayImplementationInfo
 *
 * @xxxx
 * 
 */
CTestStepRecorderCheckPlayImplementationInfo* CTestStepRecorderCheckPlayImplementationInfo::NewL()
	{
	CTestStepRecorderCheckPlayImplementationInfo* self = new(ELeave) CTestStepRecorderCheckPlayImplementationInfo;
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
CTestStepRecorderCheckPlayImplementationInfo::CTestStepRecorderCheckPlayImplementationInfo() 
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-1006-HP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepRecorderCheckPlayImplementationInfo::~CTestStepRecorderCheckPlayImplementationInfo()
	{
	}

void CTestStepRecorderCheckPlayImplementationInfo::MoscoStateChangeEvent(CBase* /*aObject*/,
														  TInt aPreviousState,
														  TInt aCurrentState,
														  TInt aErrorcCode)
	{
	iError = aErrorcCode;
	if (aPreviousState == CMdaAudioClipUtility::ENotReady
		&& aCurrentState == CMdaAudioClipUtility::EOpen)
		{
		iIsOpen = ETrue;	
		}
	CActiveScheduler::Stop();
	}
	

/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
TVerdict CTestStepRecorderCheckPlayImplementationInfo::DoTestStepPreambleL(void)
	{
	// this installs the scheduler
	TVerdict verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();
	if (verdict != EPass)
		{
		return verdict;	
		}

	 if(!GetStringFromConfig(_L("SectionOne"), _L("AudioPlayFName1"), iFileName))
		{	 
		 return EInconclusive;
		}

	// Printing to the console and log file
	INFO_PRINTF1(iTestStepName);
	INFO_PRINTF1(_L("this test is a test of CMMFMdaAudioRecorderUtility::ControllerImplementationInformationL()"));

	return verdict;
	}

/**
 *
 * Test step Postamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepRecorderCheckPlayImplementationInfo::DoTestStepPostambleL(void)
	{
	//[ Destroy the scheduler ]
	return CTestStepUnitMMFAudClient::DoTestStepPostambleL();
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
TVerdict CTestStepRecorderCheckPlayImplementationInfo::DoTestStepL()
	{
	TInt err = KErrNone;
	iTestStepResult = EFail;

	iRecorder = CMMFMdaAudioRecorderUtility::NewL(*this);
	
	TRAP(err, iRecorder->OpenFileL(iFileName));
	
	if (err != KErrNone) 
		{
		return EInconclusive;	
		}

	CActiveScheduler::Start();

	if (iError != KErrNone) 
		{	
		return EInconclusive;	
		}

	const CMMFControllerImplementationInformation* info = NULL;
	
	TRAP(err, info = &iRecorder->AudioPlayControllerImplementationInformationL());
	if (err != KErrNone)
		{
		return EFail;	
		}

	TBuf<40> displayName;
	displayName.Copy(info->DisplayName());
	
	iRecorder->Close();
	
	if(!GetStringFromConfig(_L("SectionOne"), _L("AudioFNameTestController"), iFileName))
		{
		return EInconclusive;
		}

	TUid testController = TUid::Uid(KUidTestController);
	TRAP(err, iRecorder->OpenFileL(iFileName,
									testController,
									testController,
									KNullUid));
	
	if (err != KErrNone)
		{
		return EInconclusive;	
		}
		
	CActiveScheduler::Start();
	
	if (iError != KErrNone)
		{
		return EInconclusive;		
		}

	TRAP(err, info = &iRecorder->AudioPlayControllerImplementationInformationL());
	
	if(info->DisplayName().Compare(displayName) != 0)
		{
		iTestStepResult = EPass;	
		}
	else
		{
		iTestStepResult = EFail;	
		}
	
	delete iRecorder;
	iRecorder = NULL;
			
	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}


/**
 *
 * Static constructor for CTestStepRecorderCheckRecordImplementationInfo.
 *
 *
 * @return	"CTestStepRecorderCheckRecordImplementationInfo*"
 *			The constructed CTestStepRecorderCheckRecordImplementationInfo
 *
 * @xxxx
 * 
 */
CTestStepRecorderCheckRecordImplementationInfo* CTestStepRecorderCheckRecordImplementationInfo::NewL()
	{
	CTestStepRecorderCheckRecordImplementationInfo* self = new(ELeave) CTestStepRecorderCheckRecordImplementationInfo;
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
CTestStepRecorderCheckRecordImplementationInfo::CTestStepRecorderCheckRecordImplementationInfo() 
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-1007-HP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepRecorderCheckRecordImplementationInfo::~CTestStepRecorderCheckRecordImplementationInfo()
	{
	}

void CTestStepRecorderCheckRecordImplementationInfo::MoscoStateChangeEvent(CBase* /*aObject*/,
														  TInt aPreviousState,
														  TInt aCurrentState,
														  TInt aErrorcCode)
	{
	iError = aErrorcCode;
	if (aPreviousState == CMdaAudioClipUtility::ENotReady
		&& aCurrentState == CMdaAudioClipUtility::EOpen)
		{
		iIsOpen = ETrue;
		}		
	CActiveScheduler::Stop();
	}
	

/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
TVerdict CTestStepRecorderCheckRecordImplementationInfo::DoTestStepPreambleL(void)
	{
	// this installs the scheduler
	TVerdict verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();
	if (verdict != EPass)
		{
		return verdict;	
		}

	 if(!GetStringFromConfig(_L("SectionOne"), _L("AudioPlayFName1"), iFileName))
		{
		return EInconclusive;
		}

	// Printing to the console and log file
	INFO_PRINTF1(iTestStepName);
	INFO_PRINTF1(_L("this test is a test of CMMFMdaAudioRecorderUtility::ControllerImplementationInformationL()"));

	return verdict;
	}

/**
 *
 * Test step Postamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepRecorderCheckRecordImplementationInfo::DoTestStepPostambleL(void)
	{
	//[ Destroy the scheduler ]
	return CTestStepUnitMMFAudClient::DoTestStepPostambleL();
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
TVerdict CTestStepRecorderCheckRecordImplementationInfo::DoTestStepL()
	{
	TInt err = KErrNone;
	
	iTestStepResult = EFail;

	iRecorder = CMMFMdaAudioRecorderUtility::NewL(*this);
	
	TRAP(err, iRecorder->OpenFileL(iFileName));
	
	if (err != KErrNone) 
		{
		return EInconclusive;	
		}

	CActiveScheduler::Start();

	if (iError != KErrNone) 
		{
		return EInconclusive;	
		}

	const CMMFControllerImplementationInformation* info = NULL;
	
	TRAP(err, info = &iRecorder->AudioRecorderControllerImplementationInformationL());
	if (err != KErrNone)
		{
		return EFail;	
		}

	TBuf<40> displayName;
	displayName.Copy(info->DisplayName());
	
	iRecorder->Close();
	
	if(!GetStringFromConfig(_L("SectionOne"), _L("AudioFNameTestController"), iFileName))
		{
		return EInconclusive;
		}

	TUid testController = TUid::Uid(KUidTestController);
	TRAP(err, iRecorder->OpenFileL(iFileName,
									testController,
									testController,
									KNullUid));
	if (err != KErrNone)
		{
		return EInconclusive;	
		}
		
	CActiveScheduler::Start();
	
	if (iError != KErrNone)
		{
		return EInconclusive;		
		}

	TRAP(err, info = &iRecorder->AudioRecorderControllerImplementationInformationL());
	
	if(info->DisplayName().Compare(displayName) != 0)
		{
		iTestStepResult = EPass;	
		}
	else
		{
		iTestStepResult = EFail;	
		}
		
	delete iRecorder;
	iRecorder = NULL;

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}


