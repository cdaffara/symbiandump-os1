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

#include "CmmfVideoTestControllerUIDs.hrh"

#include <mmf/common/mmfstandardcustomcommands.h>  
#include <mmf/common/mmfvideo.h>
#include <mmf/server/mmfdes.h>
#include <mmf/server/mmffile.h>

#include <mda/common/video.h>
#include <mmf/common/mmfcontroller.h>
#include <videoplayer.h>
#include <videorecorder.h>

// Specific includes for this test suite
#include "TestStepUnitMMFVidClient.h"
#include "TestSuiteUnitMMFVidClient.h"
#include "Ts_cmmfvideotestcontroller.h"
#include "TS_Codes.h"

// Specific includes for these test steps
#include "TestStepVidRecorder.h"

_LIT(KClockFileName,"C:\\mm\\mmf\\testfiles\\vclnt\\clock.dummy");

//
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepVidRecorderOpenFile.
 *
 *
 * @return	"CTestStepVidRecorderOpenFile*"
 *			The constructed CTestStepVidRecorderOpenFile
 *
 * @xxxx
 * 
 */
CTestStepVidRecorderOpenFile* CTestStepVidRecorderOpenFile::NewL(TBool aUseFileHandle)
	{
	CTestStepVidRecorderOpenFile* self = new(ELeave) CTestStepVidRecorderOpenFile(aUseFileHandle);
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
CTestStepVidRecorderOpenFile::CTestStepVidRecorderOpenFile(TBool aUseFileHandle) 
	{
	iTestStepResult = EPass;
	iUseFileHandle = aUseFileHandle;
	if (iUseFileHandle)
		{
		iTestStepName = _L("MM-MMF-VCLNT-U-0211-CP");
		}
	else
		{
		iTestStepName = _L("MM-MMF-VCLNT-U-0202-CP");
		}
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepVidRecorderOpenFile::~CTestStepVidRecorderOpenFile()
	{
	}

/**
 * Implementation of the MVideoRecorderUtilityObserver interface functions
 **/
void CTestStepVidRecorderOpenFile::MvruoOpenComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

void CTestStepVidRecorderOpenFile::MvruoPrepareComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

void CTestStepVidRecorderOpenFile::MvruoRecordComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}
	
void CTestStepVidRecorderOpenFile::MvruoEvent(const TMMFEvent& /*aEvent*/)
	{
	}


/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepVidRecorderOpenFile::DoTestStepPreambleL(void)
	{
	_LIT(KFileName, "c:\\mm\\small2.dummy");

	iFileName.Set(KFileName);
	enum TVerdict verdict;
	// this installs the scheduler
	verdict = CTestStepUnitMMFVidClient::DoTestStepPreambleL();

	return verdict;

	}

/**
 *
 * Test step Postamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepVidRecorderOpenFile::DoTestStepPostambleL(void)
	{
	delete iRecorder;
	iRecorder = NULL;
	//[ Destroy the scheduler ]
	return CTestStepUnitMMFVidClient::DoTestStepPostambleL();
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
TVerdict CTestStepVidRecorderOpenFile::DoTestStepL()
	{
	TInt err;
	err = KErrNone;
		
	__MM_HEAP_MARK;
	// Printing to the console and log file
	INFO_PRINTF1(_L("this test is openning a file with a CVideoRecorderUtility::OpenFileL()"));

	iError = KErrNone;

	iRecorder = CVideoRecorderUtility::NewL(*this);

	if (iRecorder == NULL ||
				!RecorderStateStopped() )
				return EFail;

	TRAP(err, RecOpenAndStartSchedulerL(iUseFileHandle) );
	if (err == KErrNotFound)
		iTestStepResult = EInconclusive;
	else if ( (err == KErrNotSupported || err == KErrNone) &&
			  (iError == KErrNotSupported || iError == KErrNone)
			)
		iTestStepResult = EPass;
	else 
		iTestStepResult = EFail;

	delete iRecorder;
	iRecorder = NULL;

	__MM_HEAP_MARKEND;

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

//
//------------------------------------------
//
/**
 *
 * Static constructor for CTestStepVidRecorderOpenFileCtrlUid.
 *
 *
 * @return	"CTestStepVidRecorderOpenFileCtrlUid*"
 *			The constructed CTestStepVidRecorderOpenFileCtrlUid
 *
 * @xxxx
 * 
 */
CTestStepVidRecorderOpenFileCtrlUid* CTestStepVidRecorderOpenFileCtrlUid::NewL(TBool aUseFileHandle)
	{
	CTestStepVidRecorderOpenFileCtrlUid* self = new(ELeave) CTestStepVidRecorderOpenFileCtrlUid(aUseFileHandle);
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
CTestStepVidRecorderOpenFileCtrlUid::CTestStepVidRecorderOpenFileCtrlUid(TBool aUseFileHandle) 
	{
	iTestStepResult = EPass;
	iUseFileHandle = aUseFileHandle;
	if (iUseFileHandle)
		{
		iTestStepName = _L("MM-MMF-VCLNT-U-0212-CP");
		}
	else
		{
		iTestStepName = _L("MM-MMF-VCLNT-U-0203-CP");
		}
	
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepVidRecorderOpenFileCtrlUid::~CTestStepVidRecorderOpenFileCtrlUid()
	{
	}

/**
 * Implementation of the MVideoRecorderUtilityObserver interface functions
 **/
void CTestStepVidRecorderOpenFileCtrlUid::MvruoOpenComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

void CTestStepVidRecorderOpenFileCtrlUid::MvruoPrepareComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

void CTestStepVidRecorderOpenFileCtrlUid::MvruoRecordComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}
void CTestStepVidRecorderOpenFileCtrlUid::MvruoEvent(const TMMFEvent& /*aEvent*/)
	{
	}


/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepVidRecorderOpenFileCtrlUid::DoTestStepPreambleL(void)
	{
	iFileName.Set(KClockFileName);
	enum TVerdict verdict;
	// this installs the scheduler
	verdict = CTestStepUnitMMFVidClient::DoTestStepPreambleL();

	return verdict;

	}

/**
 *
 * Test step Postamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepVidRecorderOpenFileCtrlUid::DoTestStepPostambleL(void)
	{
	//[ Destroy the scheduler ]
	return CTestStepUnitMMFVidClient::DoTestStepPostambleL();
	}

void CTestStepVidRecorderOpenFileCtrlUid::OpenFileAndStartSchedulerL()
	{
	const TUid KVidTstControllerUid = {KMmfVideoTestControllerUid}; //(0x101F88D9) //{KMmfTestControllerUid}; //(0x101F88D8)
	if (iUseFileHandle)
		{
		User::LeaveIfError(iFs.Connect());
		User::LeaveIfError(iFs.ShareProtected());
		iFile.Open(iFs, iFileName,EFileWrite);
		iRecorder->OpenFileL(iFileName, NULL, KVidTstControllerUid, KUidMdaBmpClipFormat ); //KTstControllerUid );
		}
	else
		{
		iRecorder->OpenFileL(iFileName, NULL, KVidTstControllerUid, KUidMdaBmpClipFormat ); //KTstControllerUid );
		}
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
TVerdict CTestStepVidRecorderOpenFileCtrlUid::DoTestStepL()
	{
	TInt err;
	err = KErrNone;

	__MM_HEAP_MARK;
	// Printing to the console and log file
	INFO_PRINTF1(_L("this test is openning a file with a specific Controller Uid: CVideoRecorderUtility::OpenFileL()"));

	iError = KErrNone; //KErrTimedOut;
	iTestStepResult = EPass;

	iRecorder = CVideoRecorderUtility::NewL(*this);

	if (iRecorder == NULL ||
				!RecorderStateStopped() )
				return EFail;

	TRAP(err, OpenFileAndStartSchedulerL() );
	if (err == KErrNotFound)
		return EInconclusive;
	else if (err != KErrNone) 
		return EFail;

	delete iRecorder;
	iRecorder = NULL;

	__MM_HEAP_MARKEND;

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

//
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepVidRecorderOpenDesc.
 *
 *
 * @return	"CTestStepVidRecorderOpenDesc*"
 *			The constructed CTestStepVidRecorderOpenDesc
 *
 * @xxxx
 * 
 */
CTestStepVidRecorderOpenDesc* CTestStepVidRecorderOpenDesc::NewL()
	{
	CTestStepVidRecorderOpenDesc* self = new(ELeave) CTestStepVidRecorderOpenDesc;
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
CTestStepVidRecorderOpenDesc::CTestStepVidRecorderOpenDesc() 
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-VCLNT-U-0204-CP");
	iHeapSize = 128000;
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepVidRecorderOpenDesc::~CTestStepVidRecorderOpenDesc()
	{
	}

/**
 * Implementation of the MVideoRecorderUtilityObserver interface functions
 **/
void CTestStepVidRecorderOpenDesc::MvruoOpenComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

void CTestStepVidRecorderOpenDesc::MvruoPrepareComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

void CTestStepVidRecorderOpenDesc::MvruoRecordComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}
	
void CTestStepVidRecorderOpenDesc::MvruoEvent(const TMMFEvent& /*aEvent*/)
	{
	}


/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepVidRecorderOpenDesc::DoTestStepPreambleL(void)
	{
	iFileName.Set(KClockFileName);

	TInt fSize;
	iFs.Connect();
	if ( iFile.Open(iFs, iFileName, EFileRead) != KErrNone )
		return EInconclusive;
	iFile.Size(fSize);
	// load the sound from the file to a Descriptor
	iBuf = HBufC8::NewL( fSize );
	TPtr8 des = iBuf->Des();
	iFile.Read(des);

	iFile.Close();
	iFs.Close();

	enum TVerdict verdict;
	// this installs the scheduler
	verdict = CTestStepUnitMMFVidClient::DoTestStepPreambleL();

	return verdict;

	}

/**
 *
 * Test step Postamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepVidRecorderOpenDesc::DoTestStepPostambleL(void)
	{
	delete iRecorder;
	iRecorder = NULL;
	//[ Destroy the scheduler ]
	return CTestStepUnitMMFVidClient::DoTestStepPostambleL();
	}

void CTestStepVidRecorderOpenDesc::OpenDescAndStartSchedulerL()
	{
	const TUid KVidTstControllerUid = {KMmfVideoTestControllerUid}; //(0x101F88D9) //{KMmfTestControllerUid}; //(0x101F88D8)
	TPtr8 des = iBuf->Des();
	iRecorder->OpenDesL(des, NULL, KVidTstControllerUid, KUidMdaBmpClipFormat);
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
TVerdict CTestStepVidRecorderOpenDesc::DoTestStepL()
	{
	TInt err;
	err = KErrNone;

	__MM_HEAP_MARK;
	// Printing to the console and log file
	INFO_PRINTF1(_L("MM-MMF-VCLNT-U-0204-CP"));
	INFO_PRINTF1(_L("this test is openning a descriptor with a CVideoRecorderUtility::OpenDesL()"));

	iError = KErrNone; //KErrTimedOut;

	iRecorder = CVideoRecorderUtility::NewL(*this);

	if (iRecorder == NULL ||
				!RecorderStateStopped() )
				return EFail;

	TRAP(err, OpenDescAndStartSchedulerL() );
	if (err == KErrNotFound)
		iTestStepResult = EInconclusive;
	else if ( (err == KErrNotSupported || err == KErrNone) &&
			  (iError == KErrNotSupported || iError == KErrNone)
			)
		iTestStepResult = EPass;
	else 
		iTestStepResult = EFail;

	delete iRecorder;
	iRecorder = NULL;

	__MM_HEAP_MARKEND;

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

//
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepVidRecorderOpenUrl.
 *
 *
 * @return	"CTestStepVidRecorderOpenUrl*"
 *			The constructed CTestStepVidRecorderOpenUrl
 *
 * @xxxx
 * 
 */
CTestStepVidRecorderOpenUrl* CTestStepVidRecorderOpenUrl::NewL()
	{
	CTestStepVidRecorderOpenUrl* self = new(ELeave) CTestStepVidRecorderOpenUrl;
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
CTestStepVidRecorderOpenUrl::CTestStepVidRecorderOpenUrl() 
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-VCLNT-U-0205-CP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepVidRecorderOpenUrl::~CTestStepVidRecorderOpenUrl()
	{
	}

/**
 * Implementation of the MVideoRecorderUtilityObserver interface functions
 **/
void CTestStepVidRecorderOpenUrl::MvruoOpenComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

void CTestStepVidRecorderOpenUrl::MvruoPrepareComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

void CTestStepVidRecorderOpenUrl::MvruoRecordComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}
	
void CTestStepVidRecorderOpenUrl::MvruoEvent(const TMMFEvent& /*aEvent*/)
	{
	}


/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepVidRecorderOpenUrl::DoTestStepPreambleL(void)
	{
	iFileName.Set(KTestURL);
	enum TVerdict verdict;
	// this installs the scheduler
	verdict = CTestStepUnitMMFVidClient::DoTestStepPreambleL();

	return verdict;

	}

/**
 *
 * Test step Postamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepVidRecorderOpenUrl::DoTestStepPostambleL(void)
	{
	delete iRecorder;
	iRecorder = NULL;
	//[ Destroy the scheduler ]
	return CTestStepUnitMMFVidClient::DoTestStepPostambleL();
	}

void CTestStepVidRecorderOpenUrl::OpenUrlAndStartSchedulerL()
	{
	const TUid KVidTstControllerUid = {KMmfVideoTestControllerUid}; //(0x101F88D9) //{KMmfTestControllerUid}; //(0x101F88D8)
	iRecorder->OpenUrlL(iFileName,KTestIap, NULL, KVidTstControllerUid, KUidMdaBmpClipFormat);
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
TVerdict CTestStepVidRecorderOpenUrl::DoTestStepL()
	{
	TInt err;
	err = KErrNone;

	__MM_HEAP_MARK;
	// Printing to the console and log file
	INFO_PRINTF1(_L("MM-MMF-VCLNT-U-0205-CP"));
	INFO_PRINTF1(_L("this test is openning a file with a CVideoRecorderUtility::OpenUrlL()"));

	iError = KErrNone; //KErrTimedOut;

	iRecorder = CVideoRecorderUtility::NewL(*this);

	if ( !iRecorder || !RecorderStateStopped() )
		 return EFail;

	TRAP(err, OpenUrlAndStartSchedulerL() );
	if (err == KErrNotFound)
		iTestStepResult = EInconclusive;
	else if ( (err == KErrNone) &&
			  (iError == KErrNone)
			)
		{
		
		// now use a special custom command to check the sending of these parameters
		TUid uid = TUid::Uid(0x101f72B4);
		TMMFMessageDestinationPckg destPckg(uid);

		TInt err = iRecorder->CustomCommandSync(destPckg,
						 					EMMFTestCustomCommandCheckUrl,
											KNullDesC8,
											KNullDesC8);
		if (err==KErrNone)
			iTestStepResult = EPass;
		else
			iTestStepResult = EFail;
		}
	else 
		iTestStepResult = EFail;

	delete iRecorder;
	iRecorder = NULL;

	__MM_HEAP_MARKEND;

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

//
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepVidRecorderPrepare.
 *
 *
 * @return	"CTestStepVidRecorderPrepare*"
 *			The constructed CTestStepVidRecorderPrepare
 *
 * @xxxx
 * 
 */
CTestStepVidRecorderPrepare* CTestStepVidRecorderPrepare::NewL()
	{
	CTestStepVidRecorderPrepare* self = new(ELeave) CTestStepVidRecorderPrepare;
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
CTestStepVidRecorderPrepare::CTestStepVidRecorderPrepare()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-VCLNT-U-0207-CP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepVidRecorderPrepare::~CTestStepVidRecorderPrepare()
	{
	}

/**
 * Implementation of the MVideoRecorderUtilityObserver interface functions
 **/
void CTestStepVidRecorderPrepare::MvruoOpenComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

void CTestStepVidRecorderPrepare::MvruoPrepareComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

void CTestStepVidRecorderPrepare::MvruoRecordComplete(TInt /*aError*/)
	{
	}
	
void CTestStepVidRecorderPrepare::MvruoEvent(const TMMFEvent& /*aEvent*/)
	{
	}


/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepVidRecorderPrepare::DoTestStepPreambleL(void)
	{
	iFileName.Set(KClockFileName);
	enum TVerdict verdict;
	// this installs the scheduler
	verdict = CTestStepUnitMMFVidClient::DoTestStepPreambleL();

	// Printing to the console and log file
	INFO_PRINTF1(_L("MM-MMF-VCLNT-U-0207-CP"));
	INFO_PRINTF1(_L("testing CVideoRecorderUtility::Prepare()"));
	INFO_PRINTF1(_L(" INC023486 Video Recording Client API does not handle server side leaves in prepare."));

	return verdict;

	}

/**
 *
 * Test step Postamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepVidRecorderPrepare::DoTestStepPostambleL(void)
	{
	delete iRecorder;iRecorder = NULL;
	//[ Destroy the scheduler ]
	return CTestStepUnitMMFVidClient::DoTestStepPostambleL();
	}

/**
 *
 * Do the test step.
 * Call Prepare() normally. 
 * Call Prepare() with server side error return.
 * Call Prepare() with server side leave.
 * Verify the error codes are returned correctly.
 *
 * @return	"TVerdict"
 *			The result of the test step
 *
 * @xxxx
 * 
 */
TVerdict CTestStepVidRecorderPrepare::DoTestStepL()
	{
	TUid testControllerUid = { KMmfVideoTestControllerUid };
	const TInt KExpectedErrorVal = KErrCompletion;
	TMMFMessageDestination handleInfo(testControllerUid);
	TMMFMessageDestinationPckg message(handleInfo);
	TBuf8<64> returnBuf;
	TPckgBuf<TInt> errorValPckg = KExpectedErrorVal;	// error value we want
	TPckgBuf<TInt> errorTypePckg = EMmfTC_PrepareError;	// type of error we want
	RMMFController* controller = NULL;

	//for now the test is inconclusive on error
	iTestStepResult = EInconclusive;

	iRecorder = CVideoRecorderUtility::NewL(*this);
	if (!iRecorder || !RecorderStateStopped())
		{
		INFO_PRINTF3(_L("Error CVideoRecorderUtility::NewL() iRecoder=%X RecoderStateStopped=%d"),iRecorder,RecorderStateStopped());
		goto TestFinish;
		}

	TRAPD(err, RecOpenAndStartSchedulerL() );
	if (err != KErrNone || iError != KErrNone)
		{
		INFO_PRINTF3(_L("Error RecOpenAndStartSchedulerL() err=%d iError=%d"),err,iError);
		goto TestFinish;
		}

	//from here the test fail on error
	iTestStepResult = EFail;

	// First test: normal call - expect no error
	RecorderPrepareAndStartScheduler();
	if (iError != KErrNone)
		{
		INFO_PRINTF2(_L("Error RecorderPrepareAndStartScheduler() iError=%d"),iError);
		goto TestFinish;
		}

	// Get the controller
	err = GetRecorderController(controller);
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("Error GetRecorderController() %d"), err);
		goto TestFinish;
		}

	//Second test: server side error return
	errorTypePckg = EMmfTC_PrepareError;	// type of error we want
	err = controller->CustomCommandSync(message, KTestSetError, errorValPckg, errorTypePckg, returnBuf);
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("Error CustomCommandSync(EMmfTC_PrepareError) %d"), err);
		goto TestFinish;
		}

	//server side error return expect KExpectedErrorVal
	RecorderPrepareAndStartScheduler();
	if (iError != KExpectedErrorVal)
		{
		INFO_PRINTF3(_L("RecorderPrepareAndStartScheduler() Expected %d got %d"),KExpectedErrorVal,iError);
		goto TestFinish;
		}

	//Third test: server side leave
	errorTypePckg = EMmfTC_PrepareLeave;	// type of error we want
	err = controller->CustomCommandSync(message, KTestSetError, errorValPckg, errorTypePckg, returnBuf);
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("Error CustomCommandSync(EMmfTC_PrepareLeave) %d"), err);
		goto TestFinish;
		}

	//server side leave - expect KExpectedErrorVal
	RecorderPrepareAndStartScheduler();
	if (iError != KExpectedErrorVal)
		{
		INFO_PRINTF3(_L("RecorderPrepareAndStartScheduler() Expected %d got %d"),KExpectedErrorVal,iError);
		goto TestFinish;
		}
	
	//test passed
	iTestStepResult = EPass;

TestFinish:
	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

//
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepVidRecorderClose.
 *
 *
 * @return	"CTestStepVidRecorderClose*"
 *			The constructed CTestStepVidRecorderClose
 *
 * @xxxx
 * 
 */
CTestStepVidRecorderClose* CTestStepVidRecorderClose::NewL()
	{
	CTestStepVidRecorderClose* self = new(ELeave) CTestStepVidRecorderClose;
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
CTestStepVidRecorderClose::CTestStepVidRecorderClose() 
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-VCLNT-U-0210-CP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepVidRecorderClose::~CTestStepVidRecorderClose()
	{
	}

/**
 * Implementation of the MVideoRecorderUtilityObserver interface functions
 **/
void CTestStepVidRecorderClose::MvruoOpenComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

void CTestStepVidRecorderClose::MvruoPrepareComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

void CTestStepVidRecorderClose::MvruoRecordComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}
	
void CTestStepVidRecorderClose::MvruoEvent(const TMMFEvent& /*aEvent*/)
	{
	}


/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepVidRecorderClose::DoTestStepPreambleL(void)
	{
	enum TVerdict verdict;
	// this installs the scheduler
	verdict = CTestStepUnitMMFVidClient::DoTestStepPreambleL();
	if (verdict != KErrNone)
		return EInconclusive;

	iFileName.Set(KClockFileName);

	// Printing to the console and log file
	INFO_PRINTF1(_L("MM-MMF-VCLNT-U-0210-CP"));
	INFO_PRINTF1(_L("this test is closing a source with CVideoRecorderUtility::Close()"));

	iError = KErrNone; //KErrTimedOut;

	return verdict;

	}

/**
 *
 * Test step Postamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepVidRecorderClose::DoTestStepPostambleL(void)
	{
	delete iRecorder;
	iRecorder = NULL;
	//[ Destroy the scheduler ]
	return CTestStepUnitMMFVidClient::DoTestStepPostambleL();
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
TVerdict CTestStepVidRecorderClose::DoTestStepL()
	{
	TInt err = KErrNone;

	iRecorder = CVideoRecorderUtility::NewL(*this);

	if ( !iRecorder || !RecorderStateStopped())
		 return EInconclusive;

	TRAP(err, RecOpenAndStartSchedulerL() );
	if (err != KErrNone) 
		return EInconclusive;

	TRAP(err, iRecorder->Close() );
	if (err != KErrNone) // we can check the duration if it's 0, or the file handle if we could access it....
		return EFail;

	delete iRecorder;
	iRecorder = NULL;


	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

