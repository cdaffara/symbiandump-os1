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

// Test system includes
#include <testframework.h>

#include <mmf/common/mmfstandardcustomcommands.h>  
#include <mmf/common/mmfvideo.h>
#include <mmf/server/mmfdes.h>
#include <mmf/server/mmffile.h>
#include <mmf/common/mmfdurationinfocustomcommands.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <mmf/common/mmfdurationinfocustomcommandsimpl.h>
#include <mmf/common/mmfdurationinfocustomcommandsenums.h>
#endif

#include <mmf/common/mmfcontroller.h>
#include <videoplayer.h>
#include <videorecorder.h>

#ifdef SYMBIAN_BUILD_GCE
#include <videoplayer2.h>
#endif

// Specific includes for this test suite
#include "TestStepUnitMMFVidClient.h"
#include "TestSuiteUnitMMFVidClient.h"
#include "TestStepVidPlayer.h"
#include "TS_Codes.h"



_LIT(KClockFileName,"C:\\mm\\mmf\\testfiles\\vclnt\\clock.dummy");

//
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepVidPlayerOpenFile.
 *
 *
 * @return	"CTestStepVidPlayerOpenFile*"
 *			The constructed CTestStepVidPlayerOpenFile
 */
CTestStepVidPlayerOpenFile* CTestStepVidPlayerOpenFile::NewL(const TDesC& aTestName, const TTest aTest)
	{
	CTestStepVidPlayerOpenFile* self = new(ELeave) CTestStepVidPlayerOpenFile(aTestName, aTest);
	return self;
	}

/**
 *
 * Test step constructor.
 * Each test step initialises its own name.
 */
CTestStepVidPlayerOpenFile::CTestStepVidPlayerOpenFile(const TDesC& aTestName, const TTest aTest) 
	{
	// This is the test case name that is used by the script file.
	iTestStepName = aTestName;
	iTest = aTest;
	iTestStepResult = EPass;
	if (iTestStepName == _L("MM-MMF-VCLNT-U-0803-CP") ||
		iTestStepName == _L("MM-MMF-VCLNT-U-0809-CP") ||
		iTestStepName == _L("MM-MMF-VCLNT-U-0811-CP") )
		{
		iVpu2 = ETrue;
		iSurfaceMode = ETrue;
		}
	if (iTestStepName == _L("MM-MMF-VCLNT-U-0850-CP") || iTestStepName == _L("MM-MMF-VCLNT-U-0853-CP"))
		{
		iVpu2 = ETrue;
		iSurfaceMode = EFalse;
		}	
	}

/**
 *
 * Test step destructor.
 */
CTestStepVidPlayerOpenFile::~CTestStepVidPlayerOpenFile()
	{
	}

/**
 * Implementation of the MVideoRecorderUtilityObserver interface functions
 **/
void CTestStepVidPlayerOpenFile::MvpuoOpenComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

void CTestStepVidPlayerOpenFile::MvpuoFrameReady(CFbsBitmap& /*aFrame*/, TInt /*aError*/)
	{
	}

void CTestStepVidPlayerOpenFile::MvpuoPlayComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}
	
void CTestStepVidPlayerOpenFile::MvpuoPrepareComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

	
void CTestStepVidPlayerOpenFile::MvpuoEvent(const TMMFEvent& /*aEvent*/)
	{
	}

/**
 *
 * Test step Preamble.
 */
enum TVerdict CTestStepVidPlayerOpenFile::DoTestStepPreambleL(void)
	{
	enum TVerdict verdict;
	// this installs the scheduler
	verdict = CTestStepUnitMMFVidClient::DoTestStepPreambleL();
	if (verdict != EPass)
		{
		return verdict;
		}

	iFileName.Set(KClockFileName);

	// Printing to the console and log file
	INFO_PRINTF2(_L("%S"), &iTestStepName);
	if (iVpu2 && iSurfaceMode)
		{
		if (iTest == EFileName)
			{
			INFO_PRINTF1(_L("this test is opening a file name with a CVideoPlayerUtility2::OpenFileL()"));
			}
		if (iTest == EFileHandle)
			{	
			INFO_PRINTF1(_L("this test is opening a file handle with a CVideoPlayerUtility2::OpenFileL()"));
			}
		if (iTest == EFileSource)
			{
			INFO_PRINTF1(_L("this test is opening a TMMFileSource with a CVideoPlayerUtility2::OpenFileL()"));
			}
		}
	else if (iVpu2 && !iSurfaceMode)
		{
		if (iTest == EFileName)
			{
			INFO_PRINTF1(_L("this is a negative test for opening a file name with a CVideoPlayerUtility2::OpenFileL()"));
			}
		if (iTest == EFileHandle)
			{
			INFO_PRINTF1(_L("this is a negative test for opening a file handle with a CVideoPlayerUtility2::OpenFileL()"));
			}
		}
	else
		{
		if (iTest == EFileName)
			{
			INFO_PRINTF1(_L("this test is opening a filename with a CVideoPlayerUtility::OpenFileL()"));
			}
		if (iTest == EFileHandle)
			{
			INFO_PRINTF1(_L("this test is opening a file handle with a CVideoPlayerUtility::OpenFileL()"));
			}
		if (iTest == EFileSource)
			{
			INFO_PRINTF1(_L("this test is opening a TMMFileSource with a CVideoPlayerUtility::OpenFileL()"));
			}
		if (iTest == EFileHandleSource)
			{
			INFO_PRINTF1(_L("this test is opening a TMMFileHandleSource with a CVideoPlayerUtility::OpenFileL()"));
			}
		}
	
	iError = KErrNone;

	return verdict;
	}

/**
 *
 * Test step Postamble.
 */
enum TVerdict CTestStepVidPlayerOpenFile::DoTestStepPostambleL(void)
	{
	if (!iVpu2)
		{
		delete iPlayer;
		iPlayer = NULL;
		}
#ifdef SYMBIAN_BUILD_GCE
	else
		{
		delete iPlayer2;
		iPlayer2 = NULL;
		}
#endif
	return CTestStepUnitMMFVidClient::DoTestStepPostambleL();
	}

/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 * @return	"TVerdict"
 *			The result of the test step
 */
TVerdict CTestStepVidPlayerOpenFile::DoTestStepL()
	{
	TInt err = KErrNone;
	iError = KErrNone;

	InitWservL();

	if (!iVpu2)
		{
		TRect rect, clipRect;
		iPlayer = CVideoPlayerUtility::NewL(*this, EMdaPriorityNormal, EMdaPriorityPreferenceTimeAndQuality,
										iWs, *iScreen, *iWindow, rect, clipRect);
		}
#ifdef SYMBIAN_BUILD_GCE
	else
		{
		iPlayer2 = CVideoPlayerUtility2::NewL(*this, EMdaPriorityNormal, EMdaPriorityPreferenceTimeAndQuality);
		}
#endif

	if ( !PlayerStateStopped() )
		{
		return EInconclusive;
		}

	switch (iTest)
		{
		case EFileName:
			TRAP(err, OpenAndStartSchedulerL());
			break;
		
		case EFileHandle:
			TRAP(err, OpenAndStartSchedulerL(ETrue));
			break;
		
		case EFileSource:
			{
			TMMFileSource fileSource(iFileName);
			TRAP(err, OpenAndStartSchedulerL(fileSource));
			}
			break;
		
		case EFileHandleSource:
			{
			User::LeaveIfError(iFs.Connect());
			User::LeaveIfError(iFs.ShareProtected());
			User::LeaveIfError(iFile.Open(iFs, iFileName, EFileRead));
			TMMFileHandleSource fileSource(iFile);
			TRAP(err, OpenAndStartSchedulerL(fileSource));
			}
			break;
		
		default:
			ASSERT(0);
			break;
		}

	if (!iSurfaceMode && iVpu2 && err == KErrNone && iError == KErrNotSupported)
		{
		INFO_PRINTF2(_L("This test is when the controller doesn't support graphics surfaces. Expected: %d"), err);
		return EPass;
		}

	if (err == KErrNotFound)
		{
		iTestStepResult = EInconclusive;
		}	
	else if (  err == KErrNone && iError == KErrNone )
		{
#ifdef SYMBIAN_BUILD_GCE
		SurfaceSupported();
#endif
		if (iSurfaceSupported)
			{
			if (iVpu2)
				{
				iTestStepResult = ControllerCalled(_L8("MvpssUseSurfacesL Called"));
				}
			}
		else 
			{
			iTestStepResult = ControllerCalled(_L8("SetDisplayWindowL called"));
			}
		}
	else 
		{
		iTestStepResult = EFail;
		}

	if (!iVpu2)
		{
		delete iPlayer;
		iPlayer = NULL;
		}
#ifdef SYMBIAN_BUILD_GCE
	else
		{
		delete iPlayer2;
		iPlayer2 = NULL;
		}
#endif
	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

//
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepVidPlayerOpenDesc.
 *
 *
 * @return	"CTestStepVidPlayerOpenDesc*"
 *			The constructed CTestStepVidPlayerOpenDesc
 */
CTestStepVidPlayerOpenDesc* CTestStepVidPlayerOpenDesc::NewL(TInt aUseSurface)
	{
	CTestStepVidPlayerOpenDesc* self = new(ELeave) CTestStepVidPlayerOpenDesc(aUseSurface);
	return self;
	}

/**
 *
 * Test step constructor.
 * Each test step initialises its own name.
 */
CTestStepVidPlayerOpenDesc::CTestStepVidPlayerOpenDesc(TInt aUseSurface) 
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iVpu2 = EFalse;
	if (aUseSurface == CTestStepUnitMMFVidClient::ENotSurfaceMode)
		{
		iTestStepName = _L("MM-MMF-VCLNT-U-0005-CP");
		}
	if (aUseSurface == CTestStepUnitMMFVidClient::ESurfaceMode)
		{
		iVpu2 = ETrue;
		iSurfaceMode = ETrue;
		iTestStepName = _L("MM-MMF-VCLNT-U-0805-CP");
		}
	if (aUseSurface == CTestStepUnitMMFVidClient::ESurfaceModeNotSupported)
		{
		iVpu2 = ETrue;
		iSurfaceMode = EFalse;
		iTestStepName = _L("MM-MMF-VCLNT-U-0851-CP");
		}

	iHeapSize = 150000;
	}

/**
 *
 * Test step destructor.
 */
CTestStepVidPlayerOpenDesc::~CTestStepVidPlayerOpenDesc()
	{
	}

/**
 * Implementation of the MVideoRecorderUtilityObserver interface functions
 **/
void CTestStepVidPlayerOpenDesc::MvpuoOpenComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

void CTestStepVidPlayerOpenDesc::MvpuoFrameReady(CFbsBitmap& /*aFrame*/, TInt /*aError*/)
	{
	}

void CTestStepVidPlayerOpenDesc::MvpuoPlayComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}
	
void CTestStepVidPlayerOpenDesc::MvpuoPrepareComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

	
void CTestStepVidPlayerOpenDesc::MvpuoEvent(const TMMFEvent& /*aEvent*/)
	{
	}


/**
 *
 * Test step Preamble.
 */
enum TVerdict CTestStepVidPlayerOpenDesc::DoTestStepPreambleL(void)
	{
	enum TVerdict verdict;
	// this installs the scheduler
	verdict = CTestStepUnitMMFVidClient::DoTestStepPreambleL();
	if (verdict != EPass)
		{
		return verdict;
		}

	iFileName.Set(KClockFileName);

	TInt fSize;
	iFs.Connect();
	if ( iFile.Open(iFs, iFileName, EFileRead) != KErrNone )
		{
		return EInconclusive;
		}
		
	iFile.Size(fSize);
	// load the sound from the file to a Descriptor
	iBuf = HBufC8::NewL( fSize );
	TPtr8 des = iBuf->Des();
	iFile.Read(des);

	iFile.Close();
	iFs.Close();

	// Printing to the console and log file
	INFO_PRINTF2(_L("%S"), &iTestStepName);
	if (!iVpu2)
		{
		INFO_PRINTF1(_L("this test is opening a descriptor with a CVideoPlayerUtility::OpenDesL()"));
		}
	else
		{
		INFO_PRINTF1(_L("this test is opening a descriptor with a CVideoPlayerUtility2::OpenDesL()"));
		}
		
	return verdict;
	}

/**
 *
 * Test step Postamble.
 */
enum TVerdict CTestStepVidPlayerOpenDesc::DoTestStepPostambleL(void)
	{
	if (!iVpu2)
		{
		delete iPlayer;
		iPlayer = NULL;
		}
#ifdef SYMBIAN_BUILD_GCE
	else
		{
		delete iPlayer2;
		iPlayer2 = NULL;
		}
#endif
	return CTestStepUnitMMFVidClient::DoTestStepPostambleL();
	}

void CTestStepVidPlayerOpenDesc::OpenDescAndStartSchedulerL()
	{
	if (!iVpu2)
		{
		iPlayer->OpenDesL(iBuf->Des());
		}
#ifdef SYMBIAN_BUILD_GCE
	else
		{
		iPlayer2->OpenDesL(iBuf->Des());
		}
#endif
	CActiveScheduler::Start();
}
/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 * @return	"TVerdict"
 *			The result of the test step
 */
TVerdict CTestStepVidPlayerOpenDesc::DoTestStepL()
	{
	TInt err = KErrNone;
	iError = KErrNone;

	InitWservL();

	if (!iVpu2)
		{
		TRect rect, clipRect;
		iPlayer = CVideoPlayerUtility::NewL(*this, EMdaPriorityNormal, EMdaPriorityPreferenceTimeAndQuality,
										iWs, *iScreen, *iWindow, rect, clipRect);
		}
#ifdef SYMBIAN_BUILD_GCE
	else
		{
		iPlayer2 = CVideoPlayerUtility2::NewL(*this, EMdaPriorityNormal, EMdaPriorityPreferenceTimeAndQuality);
		}
#endif

	if (!PlayerStateStopped() )
		{
		return EInconclusive;
		}
	
	TRAP(err, OpenDescAndStartSchedulerL() );

	
#ifdef _TYPHOON_TEST_
	//Typhoon fix, video is not supported
	if (err == KErrNotSupported)
		{
		return EPass;
		}
#endif

	if (!iSurfaceMode && iVpu2 && err == KErrNone && iError == KErrNotSupported)
		{
		INFO_PRINTF2(_L("This test is when the controller doesn't support graphics surfaces. Expected: %d"), err);
		return EPass;
		}
		
	if (err == KErrNotFound)
		{
		iTestStepResult = EInconclusive;
		}
	else if (  err == KErrNone && iError == KErrNone )
		{
#ifdef SYMBIAN_BUILD_GCE
		SurfaceSupported();
#endif
		if (iSurfaceSupported)
			{
			iTestStepResult = ControllerCalled(_L8("SetDisplayWindowL called"), EFalse);
			}
		else 
			{
			iTestStepResult = ControllerCalled(_L8("SetDisplayWindowL called"));
			}
		}
	else 
		{
		INFO_PRINTF3(_L("OpenDescAndStartSchedulerL() left with %d.  Error code iError is %d"), err, iError);
		iTestStepResult = EFail;
		}

	if (!iVpu2)
		{
		delete iPlayer;
		iPlayer = NULL;
		}
#ifdef SYMBIAN_BUILD_GCE
	else
		{
		delete iPlayer2;
		iPlayer2 = NULL;
		}
#endif

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

//
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepVidPlayerOpenUrl.
 *
 *
 * @return	"CTestStepVidPlayerOpenUrl*"
 *			The constructed CTestStepVidPlayerOpenUrl
 */
CTestStepVidPlayerOpenUrl* CTestStepVidPlayerOpenUrl::NewL(TInt aUseSurface)
	{
	CTestStepVidPlayerOpenUrl* self = new(ELeave) CTestStepVidPlayerOpenUrl(aUseSurface);
	return self;
	}

/**
 *
 * Test step constructor.
 * Each test step initialises its own name.
 */
CTestStepVidPlayerOpenUrl::CTestStepVidPlayerOpenUrl(TInt aUseSurface) 
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iVpu2 = EFalse;
	if (aUseSurface == CTestStepUnitMMFVidClient::ENotSurfaceMode)
		{
		iTestStepName = _L("MM-MMF-VCLNT-U-0006-CP");
		}
	if (aUseSurface == CTestStepUnitMMFVidClient::ESurfaceMode)
		{
		iVpu2 = ETrue;
		iSurfaceMode = ETrue;
		iTestStepName = _L("MM-MMF-VCLNT-U-0807-CP");
		}
	if (aUseSurface == CTestStepUnitMMFVidClient::ESurfaceModeNotSupported)
		{
		iVpu2 = ETrue;
		iSurfaceMode = EFalse;
		iTestStepName = _L("MM-MMF-VCLNT-U-0852-CP");
		}
	}

/**
 *
 * Test step destructor.
 */
CTestStepVidPlayerOpenUrl::~CTestStepVidPlayerOpenUrl()
	{
	}

/**
 * Implementation of the MVideoRecorderUtilityObserver interface functions
 **/
void CTestStepVidPlayerOpenUrl::MvpuoOpenComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

void CTestStepVidPlayerOpenUrl::MvpuoFrameReady(CFbsBitmap& /*aFrame*/, TInt /*aError*/)
	{
	}

void CTestStepVidPlayerOpenUrl::MvpuoPlayComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}
	
void CTestStepVidPlayerOpenUrl::MvpuoPrepareComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

	
void CTestStepVidPlayerOpenUrl::MvpuoEvent(const TMMFEvent& /*aEvent*/)
	{
	}


/**
 *
 * Test step Preamble.
 */
enum TVerdict CTestStepVidPlayerOpenUrl::DoTestStepPreambleL(void)
	{
	enum TVerdict verdict;
	// this installs the scheduler
	verdict = CTestStepUnitMMFVidClient::DoTestStepPreambleL();
	if (verdict != EPass)
		{
		return verdict;
		}

	_LIT8(KMimeType, "video/msvideo");

	iFileName.Set(KTestURL);
	iMimeType.Set(KMimeType);

	// Printing to the console and log file
	INFO_PRINTF2(_L("%S"), &iTestStepName);
	if (!iVpu2)
		{
		INFO_PRINTF1(_L("this test is opening a file with a CVideoPlayerUtility::OpenUrlL()"));
		}
	else
		{
		INFO_PRINTF1(_L("this test is opening a file with a CVideoPlayerUtility2::OpenUrlL()"));
		}
	
	InitWservL();
	
		if (!iVpu2)
		{
		TRect rect, clipRect;
		iPlayer = CVideoPlayerUtility::NewL(*this, EMdaPriorityNormal, EMdaPriorityPreferenceTimeAndQuality,
										iWs, *iScreen, *iWindow, rect, clipRect);
		}
#ifdef SYMBIAN_BUILD_GCE
	else
		{
		iPlayer2 = CVideoPlayerUtility2::NewL(*this, EMdaPriorityNormal, EMdaPriorityPreferenceTimeAndQuality);
		}
#endif

	if (!PlayerStateStopped() )
		{
		return EInconclusive;
		}

	return verdict;
	}

/**
 *
 * Test step Postamble.
 */
enum TVerdict CTestStepVidPlayerOpenUrl::DoTestStepPostambleL(void)
	{
	if (!iVpu2)
		{
		delete iPlayer;
		iPlayer = NULL;
		}
#ifdef SYMBIAN_BUILD_GCE
	else
		{
		delete iPlayer2;
		iPlayer2 = NULL;
		}
#endif
	return CTestStepUnitMMFVidClient::DoTestStepPostambleL();
	}

void CTestStepVidPlayerOpenUrl::OpenUrlAndStartSchedulerL()
	{
	if (!iVpu2)
		{
		iPlayer->OpenUrlL(iFileName,KTestIap,iMimeType);
		}
#ifdef SYMBIAN_BUILD_GCE
	else
		{
		iPlayer2->OpenUrlL(iFileName,KTestIap,iMimeType);
		}
#endif	
	CActiveScheduler::Start();
}
/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 * @return	"TVerdict"
 *			The result of the test step
 */
TVerdict CTestStepVidPlayerOpenUrl::DoTestStepL()
	{
	TInt err = KErrNone;
	iError = KErrNone;

	TRAP(err, OpenUrlAndStartSchedulerL() );
	
	if (!iSurfaceMode && iVpu2 && iError == KErrNotSupported && err == KErrNone)
		{
		INFO_PRINTF2(_L("This test is when the controller doesn't support graphics surfaces. Expected: %d"), err);
		return EPass;
		}
		
 	if (err == KErrNotFound)
 		{
		iTestStepResult = EInconclusive;
 		}
	else if ( (err == KErrNone) && (iError == KErrNone))
		{
		
		// now use a special custom command to check the sending of these parameters
		TUid uid = TUid::Uid(0x101f72B4);
		TMMFMessageDestinationPckg destPckg(uid);
		
		if (!iVpu2)
			{
			err = iPlayer->CustomCommandSync(destPckg,
						 					EMMFTestCustomCommandCheckUrl,
											KNullDesC8,
											KNullDesC8);
			}
#ifdef SYMBIAN_BUILD_GCE
		else
			{
			err = iPlayer2->CustomCommandSync(destPckg,
						 					EMMFTestCustomCommandCheckUrl,
											KNullDesC8,
											KNullDesC8);
			}
#endif
		if (err==KErrNone)
			{
#ifdef SYMBIAN_BUILD_GCE
			SurfaceSupported();
#endif
			if (iSurfaceSupported)
				{
				iTestStepResult = ControllerCalled(_L8("SetDisplayWindowL called"), EFalse);
				}
			else 
				{
				iTestStepResult = ControllerCalled(_L8("SetDisplayWindowL called"));
				}
			}
		else
			{
			iTestStepResult = EFail;
			}
		}
	else 
		{
		iTestStepResult = EFail;
		}

	if (!iVpu2)
		{
		delete iPlayer;
		iPlayer = NULL;
		}
#ifdef SYMBIAN_BUILD_GCE
	else
		{
		delete iPlayer2;
		iPlayer2 = NULL;
		}
#endif
	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}
	
//
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepVidPlayerOpenviaUriTag.
 *
 *
 * @return	"CTestStepVidPlayerOpenviaUriTag*"
 *			The constructed CTestStepVidPlayerOpenviaUriTag
 */
CTestStepVidPlayerOpenviaUriTag* CTestStepVidPlayerOpenviaUriTag::NewL()
	{
	CTestStepVidPlayerOpenviaUriTag* self = new(ELeave) CTestStepVidPlayerOpenviaUriTag;
	return self;
	}

/**
 *
 * Test step constructor.
 * Each test step initialises its own name.
 */
CTestStepVidPlayerOpenviaUriTag::CTestStepVidPlayerOpenviaUriTag() 
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-VCLNT-U-0313-CP");
	}

/**
 *
 * Test step destructor.
 */
CTestStepVidPlayerOpenviaUriTag::~CTestStepVidPlayerOpenviaUriTag()
	{
	}

/**
 * Implementation of the MVideoRecorderUtilityObserver interface functions
 **/
void CTestStepVidPlayerOpenviaUriTag::MvpuoOpenComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

void CTestStepVidPlayerOpenviaUriTag::MvpuoFrameReady(CFbsBitmap& /*aFrame*/, TInt /*aError*/)
	{
	}

void CTestStepVidPlayerOpenviaUriTag::MvpuoPlayComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}
	
void CTestStepVidPlayerOpenviaUriTag::MvpuoPrepareComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

void CTestStepVidPlayerOpenviaUriTag::MvpuoEvent(const TMMFEvent& /*aEvent*/)
	{
	}


/**
 *
 * Test step Preamble.
 */
enum TVerdict CTestStepVidPlayerOpenviaUriTag::DoTestStepPreambleL(void)
	{
	enum TVerdict verdict;
	// this installs the scheduler
	verdict = CTestStepUnitMMFVidClient::DoTestStepPreambleL();
	if (verdict != EPass)
		{
		return verdict;
		}

	_LIT8(KMimeType, "");
	_LIT(KTestURiTag, "rttp://www.symbian.com/test");

	iFileName.Set(KTestURiTag);
	iMimeType.Set(KMimeType);

	// Printing to the console and log file
	INFO_PRINTF1(_L("MM-MMF-VCLNT-U-0313-CP"));
	INFO_PRINTF1(_L("this test is opening a file with a CVideoPlayerUtility::OpenUrlL()"));

	InitWservL();

	TRect rect, clipRect;
	iPlayer = CVideoPlayerUtility::NewL(*this, EMdaPriorityNormal, EMdaPriorityPreferenceTimeAndQuality,
										iWs, *iScreen, *iWindow, rect, clipRect);

	if (iPlayer == NULL || !PlayerStateStopped() )
		{
		return EInconclusive;
		}
	
	return verdict;
	}

/**
 *
 * Test step Postamble.
 */
enum TVerdict CTestStepVidPlayerOpenviaUriTag::DoTestStepPostambleL(void)
	{
	delete iPlayer;
	iPlayer = NULL;
	return CTestStepUnitMMFVidClient::DoTestStepPostambleL();
	}

void CTestStepVidPlayerOpenviaUriTag::OpenUrlAndStartSchedulerL()
	{
	iPlayer->OpenUrlL(iFileName,KTestIap,iMimeType);
	CActiveScheduler::Start();
	}
/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 * @return	"TVerdict"
 *			The result of the test step
 */
TVerdict CTestStepVidPlayerOpenviaUriTag::DoTestStepL()
	{
	TInt err = KErrNone;
	iError = KErrNone;

	TRAP(err, OpenUrlAndStartSchedulerL() );
	
	if (err == KErrNotFound)
		{
		return EInconclusive;
		}
			
	if(iError != KErrExtensionNotSupported)
		{
		iTestStepResult = EFail;
		}
	else
		{
		iTestStepResult = EPass;
		}

	delete iPlayer;
	iPlayer = NULL;

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}


//
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepVidPlayerOpenTryNextCtrl.
 *
 *
 * @return	"CTestStepVidPlayerOpenTryNextCtrl*"
 *			The constructed CTestStepVidPlayerOpenTryNextCtrl
 */
CTestStepVidPlayerOpenTryNextCtrl* CTestStepVidPlayerOpenTryNextCtrl::NewL()
	{
	CTestStepVidPlayerOpenTryNextCtrl* self = new(ELeave) CTestStepVidPlayerOpenTryNextCtrl;
	return self;
	}

/**
 *
 * Test step constructor.
 * Each test step initialises its own name.
 */
CTestStepVidPlayerOpenTryNextCtrl::CTestStepVidPlayerOpenTryNextCtrl() 
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-VCLNT-U-0314-CP");
	}

/**
 *
 * Test step destructor.
 */
CTestStepVidPlayerOpenTryNextCtrl::~CTestStepVidPlayerOpenTryNextCtrl()
	{
	}

/**
 * Implementation of the MVideoRecorderUtilityObserver interface functions
 **/
void CTestStepVidPlayerOpenTryNextCtrl::MvpuoOpenComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

void CTestStepVidPlayerOpenTryNextCtrl::MvpuoFrameReady(CFbsBitmap& /*aFrame*/, TInt /*aError*/)
	{
	}

void CTestStepVidPlayerOpenTryNextCtrl::MvpuoPlayComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}
	
void CTestStepVidPlayerOpenTryNextCtrl::MvpuoPrepareComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}
	
void CTestStepVidPlayerOpenTryNextCtrl::MvpuoEvent(const TMMFEvent& /*aEvent*/)
	{
	}


/**
 *
 * Test step Preamble.
 */
enum TVerdict CTestStepVidPlayerOpenTryNextCtrl::DoTestStepPreambleL(void)
	{
	enum TVerdict verdict;
	// this installs the scheduler
	verdict = CTestStepUnitMMFVidClient::DoTestStepPreambleL();
	if (verdict != EPass)
		{
		return verdict;
		}

	_LIT8(KMimeType, "");
	_LIT(KTestURL1, "rttp://www.symbian.com/clock.dummy");
	iFileName.Set(KTestURL1);
	iMimeType.Set(KMimeType);

	// Printing to the console and log file
	INFO_PRINTF1(_L("MM-MMF-VCLNT-U-0314-CP"));
	INFO_PRINTF1(_L("this test is opening a file with a CVideoPlayerUtility::OpenUrlL()"));

	InitWservL();

	TRect rect, clipRect;
	iPlayer = CVideoPlayerUtility::NewL(*this, EMdaPriorityNormal, EMdaPriorityPreferenceTimeAndQuality,
										iWs, *iScreen, *iWindow, rect, clipRect);

	if (iPlayer == NULL || !PlayerStateStopped() )
		{
		return EInconclusive;
		}
	
	return verdict;
	}

/**
 *
 * Test step Postamble.
 */
enum TVerdict CTestStepVidPlayerOpenTryNextCtrl::DoTestStepPostambleL(void)
	{
	delete iPlayer;
	iPlayer = NULL;
	return CTestStepUnitMMFVidClient::DoTestStepPostambleL();
	}

void CTestStepVidPlayerOpenTryNextCtrl::OpenUrlAndStartSchedulerL()
	{
	iPlayer->OpenUrlL(iFileName,KTestIap,iMimeType);
	CActiveScheduler::Start();
	}
/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 * @return	"TVerdict"
 *			The result of the test step
 */
TVerdict CTestStepVidPlayerOpenTryNextCtrl::DoTestStepL()
	{
	TInt err = KErrNone;
	iError = KErrNone;

	TRAP(err, OpenUrlAndStartSchedulerL() );
	
	if (err == KErrNotFound)
		{
		return EInconclusive;
		}
		
	//need to remove <n>yes tag from 101f7D2D.RSS to really check for Trying Next Ctrl.
	//in that case, KErrNone returned
	if(iError != KErrExtensionNotSupported)
		{
		iTestStepResult = EFail;
		}
	else
		{
		iTestStepResult = EPass;
		}

	delete iPlayer;
	iPlayer = NULL;

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}
	
//
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepVidPlayerClose.
 *
 *
 * @return	"CTestStepVidPlayerClose*"
 *			The constructed CTestStepVidPlayerClose
 */
CTestStepVidPlayerClose* CTestStepVidPlayerClose::NewL()
	{
	CTestStepVidPlayerClose* self = new(ELeave) CTestStepVidPlayerClose;
	return self;
	}

/**
 *
 * Test step constructor.
 * Each test step initialises its own name.
 */
CTestStepVidPlayerClose::CTestStepVidPlayerClose() 
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-VCLNT-U-0010-CP");
	}

/**
 *
 * Test step destructor.
 */
CTestStepVidPlayerClose::~CTestStepVidPlayerClose()
	{
	}

/**
 * Implementation of the MVideoRecorderUtilityObserver interface functions
 **/
void CTestStepVidPlayerClose::MvpuoOpenComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

void CTestStepVidPlayerClose::MvpuoFrameReady(CFbsBitmap& /*aFrame*/, TInt /*aError*/)
	{
	}

void CTestStepVidPlayerClose::MvpuoPlayComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}
	
void CTestStepVidPlayerClose::MvpuoPrepareComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

	
void CTestStepVidPlayerClose::MvpuoEvent(const TMMFEvent& /*aEvent*/)
	{
	}


/**
 *
 * Test step Preamble.
 */
enum TVerdict CTestStepVidPlayerClose::DoTestStepPreambleL(void)
	{
	enum TVerdict verdict;
	// this installs the scheduler
	verdict = CTestStepUnitMMFVidClient::DoTestStepPreambleL();
	if (verdict != KErrNone)
		{
		return EInconclusive;
		}

	iFileName.Set(KClockFileName);

	// Printing to the console and log file
	INFO_PRINTF1(_L("MM-MMF-VCLNT-U-0010-CP"));
	INFO_PRINTF1(_L("this test is closing a source with CVideoPlayerUtility::Close()"));

	iError = KErrNone; //KErrTimedOut;


	return verdict;

	}

/**
 *
 * Test step Postamble.
 */
enum TVerdict CTestStepVidPlayerClose::DoTestStepPostambleL(void)
	{
	delete iPlayer;
	iPlayer = NULL;
	return CTestStepUnitMMFVidClient::DoTestStepPostambleL();
	}

/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 * @return	"TVerdict"
 *			The result of the test step
 */
TVerdict CTestStepVidPlayerClose::DoTestStepL()
	{
	InitWservL();

	TRect rect, clipRect;
	iPlayer = CVideoPlayerUtility::NewL(*this, EMdaPriorityNormal, EMdaPriorityPreferenceTimeAndQuality,
										iWs, *iScreen, *iWindow, rect, clipRect);

	if (!iPlayer || !PlayerStateStopped())
		{
		return EInconclusive;
		}

	TInt err = KErrNone;
	TRAP(err, OpenAndStartSchedulerL() );

#ifdef _TYPHOON_TEST_
	//Typhoon fix, video is not supported
	if (err == KErrNotSupported)
		{
		return EPass;
		}
#endif

	if (err != KErrNone) 
		{
		return EInconclusive;
		}


	iPlayer->Close();

#ifdef _TYPHOON_TEST_
	//Typhoon fix, video is not supported
	if (err == KErrNotSupported)
		{
		return EPass;
		}
#endif

	if (err != KErrNone) // we can check the duration if it's 0, or the file handle if we could access it....
		{	
		return EFail;
		}

	delete iPlayer;
	iPlayer = NULL;


	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

/**
 *
 * Static constructor for CTestStepVidPlayerOpenDurationCustomCommand.
 *
 *
 * @return	"CTestStepVidPlayerOpenDurationCustomCommand*"
 *			The constructed CTestStepVidPlayerOpenDurationCustomCommand
 */
CTestStepVidPlayerOpenDurationCustomCommand* CTestStepVidPlayerOpenDurationCustomCommand::NewL()
	{
	CTestStepVidPlayerOpenDurationCustomCommand* self = new(ELeave) CTestStepVidPlayerOpenDurationCustomCommand;
	return self;
	}

/**
 *
 * Test step constructor.
 * Each test step initialises its own name.
 */
CTestStepVidPlayerOpenDurationCustomCommand::CTestStepVidPlayerOpenDurationCustomCommand() 
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-VCLNT-U-0256-HP");
	}

/**
 *
 * Test step destructor.
 */
CTestStepVidPlayerOpenDurationCustomCommand::~CTestStepVidPlayerOpenDurationCustomCommand()
	{
	}


/**
 *
 * Test step Preamble.
 */
TVerdict CTestStepVidPlayerOpenDurationCustomCommand::DoTestStepPreambleL(void)
	{
	enum TVerdict verdict;
	// this installs the scheduler
	verdict = CTestStepUnitMMFVidClient::DoTestStepPreambleL();
	if (verdict != EPass)
		{
		return verdict;
		}
		
	_LIT8(KMimeType, "video/msvideo");
	iFileName.Set(KTestURL);
	iMimeType.Set(KMimeType);

	// Printing to the console and log file
	INFO_PRINTF1(_L("MM-MMF-VCLNT-U-0256-HP")); 
	INFO_PRINTF1(_L("This test opens a file with a CVideoPlayerUtility::OpenUrlL() and then tests"));
	INFO_PRINTF1(_L("the custom command: EMMFGetDurationInfo.  The custom command parser for this test case"));
	INFO_PRINTF1(_L("is implemented in the video test controller: TInt CMMFTestController::GetDurationInfo(TMMFDurationInfo& aDurationInfo)"));
	
	InitWservL(); 
	TRect rect, clipRect; 
	iPlayer = CVideoPlayerUtility::NewL(*this, EMdaPriorityNormal, EMdaPriorityPreferenceTimeAndQuality,
										iWs, *iScreen, *iWindow, rect, clipRect); 
	if (iPlayer == NULL ||!PlayerStateStopped())
		{
		return EInconclusive;
		}
	return verdict;
	}
	
void CTestStepVidPlayerOpenDurationCustomCommand::OpenUrlAndStartSchedulerL()
	{
	iPlayer->OpenUrlL(iFileName,KTestIap,iMimeType);
	CActiveScheduler::Start();
    }

/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 * @return	"TVerdict"
 *			The result of the test step
 */
TVerdict CTestStepVidPlayerOpenDurationCustomCommand::DoTestStepL()
	{
	TInt err = KErrNone;
	
	TRAP(err,OpenUrlAndStartSchedulerL());
	if(err == KErrNotFound)
		{
		return EInconclusive;
		}
		
	// Now the player is open we can check that the duration custom command parser is working correctly
	// The test controller is hard coded to always return TMMFDurationInfo::EMMFDurationInfoInfinite
	TUid uid = TUid::Uid(0x10273812);
	TMMFMessageDestinationPckg destPckg(uid);
	TPckgBuf<TMMFDurationInfo> enumPckg;
	iPlayer->CustomCommandSync(destPckg, EMMFGetDurationInfo, KNullDesC8, KNullDesC8, enumPckg);
	
	if(enumPckg()!=EMMFDurationInfoInfinite)
		{
		iTestStepResult=EFail;
		}
		
    INFO_PRINTF1(_L("Finished with this test step"));
    return iTestStepResult;
	}

/**
 *
 * Test step Postamble.
 */
TVerdict CTestStepVidPlayerOpenDurationCustomCommand::DoTestStepPostambleL(void)
	{
	delete iPlayer;
	iPlayer = NULL;
	return CTestStepUnitMMFVidClient::DoTestStepPostambleL();
	}

// from MVideoPlayerUtilityObserver
void CTestStepVidPlayerOpenDurationCustomCommand::MvpuoOpenComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop();
	}

void CTestStepVidPlayerOpenDurationCustomCommand::MvpuoFrameReady(CFbsBitmap& /*aFrame*/, TInt /*aError*/)
	{
	}

void CTestStepVidPlayerOpenDurationCustomCommand::MvpuoPlayComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop();
	}
	
void CTestStepVidPlayerOpenDurationCustomCommand::MvpuoPrepareComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop();
	}
	
void CTestStepVidPlayerOpenDurationCustomCommand::MvpuoEvent(const TMMFEvent& /*aEvent*/)
	{
	}

