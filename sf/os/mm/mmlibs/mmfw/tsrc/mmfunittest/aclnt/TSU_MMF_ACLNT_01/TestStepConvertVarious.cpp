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

// Test system includes
#include <testframework.h>

// Specific includes for this test suite
#include "TestStepUnitMMFAudClient.h"
#include "TestSuiteUnitMMFAudClient.h"

// Specific includes for these test steps
#include "TestStepConvert.h"

#include "mmfclientaudioplayer.h"
#include "../../ctlfrm/TS_Codes.h"

_LIT(KAudioControllerName,"Symbian Audio Wav Test controller");



//
//
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepConvertGetImplementationInfo.
 *
 *
 * @return	"CTestStepConvertGetImplementationInfo*"
 *			The constructed CTestStepConvertGetImplementationInfo
 *
 * @xxxx
 * 
 */
CTestStepConvertGetImplementationInfo* CTestStepConvertGetImplementationInfo::NewL()
	{
	CTestStepConvertGetImplementationInfo* self = new(ELeave) CTestStepConvertGetImplementationInfo;
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
CTestStepConvertGetImplementationInfo::CTestStepConvertGetImplementationInfo() 
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0571-HP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepConvertGetImplementationInfo::~CTestStepConvertGetImplementationInfo()
	{
	}


void CTestStepConvertGetImplementationInfo::MoscoStateChangeEvent(CBase* /*aObject*/,
														  TInt aPreviousState,
														  TInt aCurrentState,
														  TInt aErrorcCode)
	{
	iError = aErrorcCode;
	iHasStartedConverting = EFalse;
	iHasFinishedConverting = EFalse;
	if (aPreviousState == CMdaAudioClipUtility::EOpen
		&& aCurrentState == CMdaAudioClipUtility::EPlaying)
		iHasStartedConverting = ETrue;
	if (aPreviousState == CMdaAudioClipUtility::EPlaying
		&& aCurrentState == CMdaAudioClipUtility::EOpen)
		iHasFinishedConverting = ETrue;

	CActiveScheduler::Stop();
	}
/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepConvertGetImplementationInfo::DoTestStepPreambleL(void)
	{
	enum TVerdict verdict;
	// this installs the scheduler
	verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();
	if (verdict != EPass)
		return verdict;

	// Printing to the console and log file
	INFO_PRINTF1(iTestStepName);
	INFO_PRINTF1(_L("this is a test of CMMFMdaAudioConvertUtility::ControllerImplementationInformationL()"));

	if(!GetStringFromConfig(_L("SectionOne"), _L("AudioPlayFName7"), iFileName) ||
	   !GetStringFromConfig(_L("SectionOne"), _L("AudioFNameToConvert"), iFileName2) )
		{
		//INFO_PRINTF2(_L("file name %s not found..."), fileptr);
		return EInconclusive;
		}

	// create the Convert utility
	if ( (iConvert = CMdaAudioConvertUtility::NewL(*this)) == NULL )
		verdict = EInconclusive;
	// create an empty target file
	iFs.Connect();
	if ( iFile.Replace(iFs, iFileName2, EFileWrite) )
		return EInconclusive;
	if ( iFile.SetSize(0) )
		return EInconclusive;
	iFile.Close();
	iFs.Close();

	// Printing to the console and log file
	INFO_PRINTF1(iTestStepName);
	INFO_PRINTF1(_L("CMMFMdaAudioConvertUtility::ControllerImplementationInformationL()"));

	return verdict;
	}

/**
 *
 * Test step Postamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepConvertGetImplementationInfo::DoTestStepPostambleL(void)
	{
	//delete the output file 
	CFileMan* fileMan;
	iFs.Connect();
	fileMan = CFileMan::NewL(iFs);
	fileMan->Delete(iFileName2);
	iFs.Close();
	delete fileMan;
	fileMan = NULL;

	delete iConvert;
	iConvert = NULL;
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
TVerdict CTestStepConvertGetImplementationInfo::DoTestStepL()
	{
	TInt err;
	err = KErrNone;
	iTestStepResult = EFail;

	iConvert->OpenL(iFileName, iFileName2 );
	CActiveScheduler::Start();

	if (iError != KErrNone) 
		return EInconclusive;

	const CMMFControllerImplementationInformation* info = NULL;
	
	TRAP(err, info = &iConvert->ControllerImplementationInformationL());
	if (err != KErrNone)
		return EFail;

	if (info->DisplayName().Compare(KAudioControllerName)!=0)
		iTestStepResult = EFail;
	else
		iTestStepResult = EPass;
	
	delete iConvert;
	iConvert = NULL;

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
 * Static constructor for CTestStepConvertSendCustomCommandSync.
 *
 *
 * @return	"CTestStepConvertSendCustomCommandSync*"
 *			The constructed CTestStepConvertSendCustomCommandSync
 *
 * @xxxx
 * 
 */
CTestStepConvertSendCustomCommandSync* CTestStepConvertSendCustomCommandSync::NewL()
	{
	CTestStepConvertSendCustomCommandSync* self = new(ELeave) CTestStepConvertSendCustomCommandSync;
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
CTestStepConvertSendCustomCommandSync::CTestStepConvertSendCustomCommandSync()
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0572-HP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepConvertSendCustomCommandSync::~CTestStepConvertSendCustomCommandSync()
	{
	}

void CTestStepConvertSendCustomCommandSync::MoscoStateChangeEvent(CBase* /*aObject*/,
														  TInt aPreviousState,
														  TInt aCurrentState,
														  TInt aErrorcCode)
	{
	iError = aErrorcCode;
	iHasStartedConverting = EFalse;
	iHasFinishedConverting = EFalse;
	if (aPreviousState == CMdaAudioClipUtility::EOpen
		&& aCurrentState == CMdaAudioClipUtility::EPlaying)
		iHasStartedConverting = ETrue;
	if (aPreviousState == CMdaAudioClipUtility::EPlaying
		&& aCurrentState == CMdaAudioClipUtility::EOpen)
		iHasFinishedConverting = ETrue;

	CActiveScheduler::Stop();
	}
	
/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepConvertSendCustomCommandSync::DoTestStepPreambleL(void)
	{
	enum TVerdict verdict;
	// this installs the scheduler
	verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();
	if (verdict != EPass)
		return verdict;

	if(!GetStringFromConfig(_L("SectionOne"), _L("AudioPlayFName7"), iFileName) ||
	   !GetStringFromConfig(_L("SectionOne"), _L("AudioFNameToConvert"), iFileName2) )
		{
		//INFO_PRINTF2(_L("file name %s not found..."), fileptr);
		return EInconclusive;
		}
		
	iClipLocationSrc = new TMdaFileClipLocation(iFileName);
	iClipLocationTrgt = new TMdaFileClipLocation(iFileName2);

	// Printing to the console and log file
	INFO_PRINTF1(iTestStepName);
	INFO_PRINTF1(_L("this test is a test of CMMFMdaAudioConvertUtility::CustomCommandSync()"));

	return verdict;
	}

/**
 *
 * Test step Postamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepConvertSendCustomCommandSync::DoTestStepPostambleL(void)
	{
	//delete the output file 
	CFileMan* fileMan;
	iFs.Connect();
	fileMan = CFileMan::NewL(iFs);
	fileMan->Delete(iFileName2);
	iFs.Close();
	delete fileMan;
	fileMan = NULL;

	delete iConvert;
	iConvert = NULL;
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
TVerdict CTestStepConvertSendCustomCommandSync::DoTestStepL()
	{
	TInt err;
	err = KErrNone;
	iTestStepResult = EFail;

	iConvert = CMdaAudioConvertUtility::NewL(*this);
	
	TRAP( err, iConvert->OpenL(iClipLocationSrc,iClipLocationTrgt,TUid::Uid(KUidTestController),KNullUid,KFourCCNULL));
	if (err!=KErrNone)
		return EInconclusive;	

	CActiveScheduler::Start();
	if (iError!=KErrNone)
		return EInconclusive;

	TUid uid;
	uid = TUid::Uid(0x101f72B4);
	TMMFMessageDestinationPckg destPckg(uid);
	// test without a return value
	err = iConvert->CustomCommandSync(destPckg,
								EMMFTestCustomCommandGeneralSynchronous,
								KFirstParameter,
								KSecondParameter);
								
	if (err!=KTestCustomCommandSuccess)
		return EFail;
		
	TBuf8<256> buf;
		
	err = iConvert->CustomCommandSync(destPckg,
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

	delete iConvert;
	iConvert = NULL;

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
 * Static constructor for CTestStepConvertSendCustomCommandAsync.
 *
 *
 * @return	"CTestStepConvertSendCustomCommandAsync*"
 *			The constructed CTestStepConvertSendCustomCommandAsync
 *
 * @xxxx
 * 
 */
CTestStepConvertSendCustomCommandAsync* CTestStepConvertSendCustomCommandAsync::NewL()
	{
	CTestStepConvertSendCustomCommandAsync* self = new(ELeave) CTestStepConvertSendCustomCommandAsync;
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
CTestStepConvertSendCustomCommandAsync::CTestStepConvertSendCustomCommandAsync()
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0573-HP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepConvertSendCustomCommandAsync::~CTestStepConvertSendCustomCommandAsync()
	{
	}

/**
 *
 * Test step Close
 * handles all deletion
 *
 * @xxxx
 * 
 */
void CTestStepConvertSendCustomCommandAsync::Close()
	{
	delete iAsyncObserver;
	}


void CTestStepConvertSendCustomCommandAsync::MoscoStateChangeEvent(CBase* /*aObject*/,
														  TInt aPreviousState,
														  TInt aCurrentState,
														  TInt aErrorcCode)
	{
	iError = aErrorcCode;
	iHasStartedConverting = EFalse;
	iHasFinishedConverting = EFalse;
	if (aPreviousState == CMdaAudioClipUtility::EOpen
		&& aCurrentState == CMdaAudioClipUtility::EPlaying)
		iHasStartedConverting = ETrue;
	if (aPreviousState == CMdaAudioClipUtility::EPlaying
		&& aCurrentState == CMdaAudioClipUtility::EOpen)
		iHasFinishedConverting = ETrue;

	CActiveScheduler::Stop();
	}
	
/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepConvertSendCustomCommandAsync::DoTestStepPreambleL(void)
	{
	enum TVerdict verdict;
	// this installs the scheduler
	verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();
	if (verdict != EPass)
		return verdict;

	if(!GetStringFromConfig(_L("SectionOne"), _L("AudioPlayFName7"), iFileName) ||
	   !GetStringFromConfig(_L("SectionOne"), _L("AudioFNameToConvert"), iFileName2) )
		{
		//INFO_PRINTF2(_L("file name %s not found..."), fileptr);
		return EInconclusive;
		}
		
	iClipLocationSrc = new TMdaFileClipLocation(iFileName);
	iClipLocationTrgt = new TMdaFileClipLocation(iFileName2);
		
	iAsyncObserver = new (ELeave) CAsyncObserver;

	// Printing to the console and log file
	INFO_PRINTF1(iTestStepName);
	INFO_PRINTF1(_L("this test is a test of CMMFMdaAudioConvertUtility::CustomCommandAsync()"));

	return verdict;
	}

/**
 *
 * Test step Postamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepConvertSendCustomCommandAsync::DoTestStepPostambleL(void)
	{
	delete iConvert;
	iConvert = NULL;
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
TVerdict CTestStepConvertSendCustomCommandAsync::DoTestStepL()
	{
	TInt err;
	err = KErrNone;
	iTestStepResult = EFail;

	iConvert = CMdaAudioConvertUtility::NewL(*this);
	
	TRAP( err, iConvert->OpenL(iClipLocationSrc,iClipLocationTrgt,TUid::Uid(KUidTestController),KNullUid,KFourCCNULL));
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
	iConvert->CustomCommandAsync(destinationPckg,
								EMMFTestCustomCommandGeneralAsynchronous,
								KFirstParameter,
								KSecondParameter,
								iAsyncObserver->ActiveStatus());

	CActiveScheduler::Start();	
	if (iAsyncObserver->iStatus.Int() !=KTestCustomCommandSuccess)
		return EFail;
								
	TBuf8<256> buf;
		
	iConvert->CustomCommandAsync(destinationPckg,
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

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

/**
 *
 * Static constructor for CTestStepConvertCheckImplementationInfo.
 *
 *
 * @return	"CTestStepConvertCheckImplementationInfo*"
 *			The constructed CTestStepConvertCheckImplementationInfo
 *
 * @xxxx
 * 
 */
CTestStepConvertCheckImplementationInfo* CTestStepConvertCheckImplementationInfo::NewL()
	{
	CTestStepConvertCheckImplementationInfo* self = new(ELeave) CTestStepConvertCheckImplementationInfo;
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
CTestStepConvertCheckImplementationInfo::CTestStepConvertCheckImplementationInfo() 
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-1005-HP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepConvertCheckImplementationInfo::~CTestStepConvertCheckImplementationInfo()
	{
	}


void CTestStepConvertCheckImplementationInfo::MoscoStateChangeEvent(CBase* /*aObject*/,
														  TInt aPreviousState,
														  TInt aCurrentState,
														  TInt aErrorcCode)
	{
	iError = aErrorcCode;
	iHasStartedConverting = EFalse;
	iHasFinishedConverting = EFalse;
	if (aPreviousState == CMdaAudioClipUtility::EOpen
		&& aCurrentState == CMdaAudioClipUtility::EPlaying)
		{
		iHasStartedConverting = ETrue;	
		}
	if (aPreviousState == CMdaAudioClipUtility::EPlaying
		&& aCurrentState == CMdaAudioClipUtility::EOpen)
		{
		iHasFinishedConverting = ETrue;	
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
TVerdict CTestStepConvertCheckImplementationInfo::DoTestStepPreambleL(void)
	{
	TVerdict verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();
	if (verdict != EPass)	
		{
		return verdict;	
		}
	TInt err = KErrNone;
	// Printing to the console and log file
	INFO_PRINTF1(iTestStepName);
	INFO_PRINTF1(_L("this is a test of CMMFMdaAudioConvertUtility::ControllerImplementationInformationL()"));

	if(!GetStringFromConfig(_L("SectionOne"), _L("AudioPlayFName7"), iFileName) ||
	   !GetStringFromConfig(_L("SectionOne"), _L("AudioFNameToConvert"), iFileName2) )
		{
		return EInconclusive;
		}
		
	// create the Convert utility
	if ( (iConvert = CMdaAudioConvertUtility::NewL(*this)) == NULL )
		{
		verdict = EInconclusive;
		return verdict;	
		}
	// create an empty target file
	User::LeaveIfError(iFs.Connect());
	err = iFile.Replace(iFs, iFileName2, EFileWrite);
	if (err != KErrNone)
		{
		return EInconclusive;	
		}
	err = iFile.SetSize(0);
	if (err != KErrNone)
		{
		return EInconclusive;	
		}
	iFile.Close();
	iFs.Close();

	// Printing to the console and log file
	INFO_PRINTF1(iTestStepName);
	INFO_PRINTF1(_L("CMMFMdaAudioConvertUtility::ControllerImplementationInformationL()"));

	return verdict;
	}

/**
 *
 * Test step Postamble.
 *
 * @xxxx
 * 
 */
TVerdict CTestStepConvertCheckImplementationInfo::DoTestStepPostambleL(void)
	{
	//delete the output file 
	CFileMan* fileMan;
	iFs.Connect();
	fileMan = CFileMan::NewL(iFs);
	fileMan->Delete(iFileName2);
	iFs.Close();
	delete fileMan;
	fileMan = NULL;
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
TVerdict CTestStepConvertCheckImplementationInfo::DoTestStepL()
	{
	TInt err;
	err = KErrNone;
	iTestStepResult = EFail;
	
	iConvert->OpenL(iFileName, iFileName2 );
	CActiveScheduler::Start();

	if (iError != KErrNone) 
		{
		return EInconclusive;	
		}

	const CMMFControllerImplementationInformation* info = NULL;
	
	TRAP(err, info = &iConvert->ControllerImplementationInformationL());
	TBuf<40> displayName;
	displayName.Copy(info->DisplayName());

	if (err != KErrNone)
		{
		return EFail;	
		}
	
	iConvert->Close();

	if(!GetStringFromConfig(_L("SectionOne"), _L("AudioPlayFName7"), iFileName) ||
	   !GetStringFromConfig(_L("SectionOne"), _L("AudioFNameToConvert"), iFileName2) )
		{
		return EInconclusive;
		}
		
	iClipLocationSrc = new(ELeave) TMdaFileClipLocation(iFileName);
	iClipLocationDest = new(ELeave) TMdaFileClipLocation(iFileName2);
		
	TRAP( err, iConvert->OpenL(iClipLocationSrc,iClipLocationDest,TUid::Uid(KUidTestController),KNullUid,KFourCCNULL));
	if (err != KErrNone)	
		{
		return EInconclusive;		
		}

	CActiveScheduler::Start();
	if (iError != KErrNone)
		{
		return EInconclusive;	
		}

	TRAP(err, info = &iConvert->ControllerImplementationInformationL());
	if (err != KErrNone)
		{
		return EFail;	
		}
			
	if(info->DisplayName().Compare(displayName) != 0)
		{
		iTestStepResult = EPass;	
		}
	else
		{
		iTestStepResult = EFail;	
		}
	
	delete iClipLocationSrc;
	iClipLocationSrc = NULL;
	delete iClipLocationDest;
	iClipLocationDest = NULL;
	delete iConvert;
	iConvert = NULL;
	
	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}


