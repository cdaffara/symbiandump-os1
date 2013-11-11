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

// Test system includes
#include <testframework.h>

#include <mmf/common/mmfstandardcustomcommands.h>  
#include <mmf/common/mmfvideo.h>
#include <mmf/server/mmfdes.h>
#include <mmf/server/mmffile.h>
#include <mmf/common/mmfcontroller.h>

#include "CmmfVideoTestControllerUIDs.hrh"
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

// --------------------------------------------

/**
 *
 * Static constructor for CTestStepVidPlayerOpenFileCtrlUid.
 *
 *
 * @return	"CTestStepVidPlayerOpenFileCtrlUid*"
 *			The constructed CTestStepVidPlayerOpenFileCtrlUid
 * 
 */
CTestStepVidPlayerOpenFileCtrlUid* CTestStepVidPlayerOpenFileCtrlUid::NewL(TInt aUseFileHandle, TInt aSurfaceMode)
	{
	CTestStepVidPlayerOpenFileCtrlUid* self = new(ELeave) CTestStepVidPlayerOpenFileCtrlUid(aUseFileHandle, aSurfaceMode);
	return self;
	}

/**
 *
 * Test step constructor.
 * Each test step initialises its own name.
 *
 */
CTestStepVidPlayerOpenFileCtrlUid::CTestStepVidPlayerOpenFileCtrlUid(TInt aUseFileHandle, TInt aSurfaceMode) 
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iUseFileHandle = aUseFileHandle;
	iSurfaceMode = aSurfaceMode;
	if (iSurfaceMode == CTestStepUnitMMFVidClient::ENotSurfaceMode )
		{
		iVpu2 = EFalse;
		if (iUseFileHandle == CTestStepVidPlayerOpenFileCtrlUid::EFileHandle)
			{
			iTestStepName = _L("MM-MMF-VCLNT-U-0012-CP");
			}
		else
			{
			iTestStepName = _L("MM-MMF-VCLNT-U-0004-CP");
			}
		}
	else
		{
		iVpu2 = ETrue;
		if (iUseFileHandle == CTestStepVidPlayerOpenFileCtrlUid::EFileName)
			{
			iTestStepName = _L("MM-MMF-VCLNT-U-0804-CP");
			}
		if (iUseFileHandle == CTestStepVidPlayerOpenFileCtrlUid::EFileHandle)
			{
			iTestStepName = _L("MM-MMF-VCLNT-U-0810-CP");
			}
		if (iUseFileHandle == CTestStepVidPlayerOpenFileCtrlUid::EDescriptor)
			{
			iTestStepName = _L("MM-MMF-VCLNT-U-0806-CP");
			iHeapSize = 150000;
			}
		if (iUseFileHandle == CTestStepVidPlayerOpenFileCtrlUid::EUrl)
			{
			iTestStepName = _L("MM-MMF-VCLNT-U-0808-CP");
			}
		}	
	}

/**
 *
 * Test step destructor.
 *
 */
CTestStepVidPlayerOpenFileCtrlUid::~CTestStepVidPlayerOpenFileCtrlUid()
	{
	}

/**
 * Implementation of the MVideoRecorderUtilityObserver interface functions
 **/
void CTestStepVidPlayerOpenFileCtrlUid::MvpuoOpenComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

void CTestStepVidPlayerOpenFileCtrlUid::MvpuoFrameReady(CFbsBitmap& /*aFrame*/, TInt /*aError*/)
	{
	}

void CTestStepVidPlayerOpenFileCtrlUid::MvpuoPlayComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}
	
void CTestStepVidPlayerOpenFileCtrlUid::MvpuoPrepareComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

	
void CTestStepVidPlayerOpenFileCtrlUid::MvpuoEvent(const TMMFEvent& /*aEvent*/)
	{
	}


/**
 *
 * Test step Preamble.
 */
enum TVerdict CTestStepVidPlayerOpenFileCtrlUid::DoTestStepPreambleL(void)
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
 */
enum TVerdict CTestStepVidPlayerOpenFileCtrlUid::DoTestStepPostambleL(void)
	{
	return CTestStepUnitMMFVidClient::DoTestStepPostambleL();
	}

void CTestStepVidPlayerOpenFileCtrlUid::OpenFileAndStartSchedulerL()
	{
	const TUid KVidTstControllerUid = {KMmfVideoTestControllerUid}; //(0x101F88D9) //{KMmfTestControllerUid}; //(0x101F88D8)
	switch (iUseFileHandle)
		{
	case CTestStepVidPlayerOpenFileCtrlUid::EFileHandle:
		{
		User::LeaveIfError(iFs.Connect());
		User::LeaveIfError(iFs.ShareProtected());
		User::LeaveIfError(iFile.Open(iFs, iFileName, EFileRead));
		if (!iVpu2)
			{			
			iPlayer->OpenFileL(iFile, KVidTstControllerUid ); 
			}
#ifdef SYMBIAN_BUILD_GCE
		else
			{
			iPlayer2->OpenFileL(iFile, KVidTstControllerUid );
			}
#endif
		break;
		}
	case CTestStepVidPlayerOpenFileCtrlUid::EFileName:
		{
		if (!iVpu2)
			{			
			iPlayer->OpenFileL(iFileName, KVidTstControllerUid );
			}
#ifdef SYMBIAN_BUILD_GCE
		else
			{
			iPlayer2->OpenFileL(iFileName, KVidTstControllerUid );
			}
#endif
		break;
		}
	case CTestStepVidPlayerOpenFileCtrlUid::EDescriptor:
		{
		iFileName.Set(KClockFileName);

		TInt fSize;
		iFs.Connect();
		iFile.Open(iFs, iFileName, EFileRead);
		iFile.Size(fSize);
		// load the sound from the file to a Descriptor
		iBuf.CreateMaxL( fSize );
		
		iFile.Read(iBuf);

		iFile.Close();
		iFs.Close();

		if (!iVpu2)
			{			
			iPlayer->OpenDesL(iBuf, KVidTstControllerUid );
			}
#ifdef SYMBIAN_BUILD_GCE
		else
			{
			iPlayer2->OpenDesL(iBuf, KVidTstControllerUid );
			}
#endif
		iBuf.Close();
		break;
		}
	case CTestStepVidPlayerOpenFileCtrlUid::EUrl:
		{
		_LIT8(KMimeType, "video/msvideo");

		iFileName.Set(KTestURL);
		iMimeType.Set(KMimeType);
		if (!iVpu2)
			{			
			iPlayer->OpenUrlL(iFileName,KTestIap,iMimeType, KVidTstControllerUid );
			}
#ifdef SYMBIAN_BUILD_GCE
		else
			{
			iPlayer2->OpenUrlL(iFileName,KTestIap,iMimeType, KVidTstControllerUid );
			}
#endif
		break;
		}
	default:
		ERR_PRINTF1(_L("Unknown case"));
		break;
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
 */
TVerdict CTestStepVidPlayerOpenFileCtrlUid::DoTestStepL()
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

	// Open file using specific controller UID
	TRAP(err, OpenFileAndStartSchedulerL());

	if (err == KErrNotSupported)
		{
		return EPass;
		}
		
	if (err == KErrNotFound)
		{
		iTestStepResult = EInconclusive;
		}
	else if ((err == KErrNotSupported || err == KErrNone) &&
			 (iError == KErrNotSupported || iError == KErrNone))
		{
		// Get the controller
		RMMFController* controller = NULL;
		err = GetPlayerController(controller);
		if (err != KErrNone)
			{
			INFO_PRINTF2(_L("Error getting controller %d"), err);
			return EInconclusive;
			}

		// Check that the correct functions were called in DoOpenController
		TBuf8<64> functionText;
		TUid uid = { KMmfVideoTestControllerUid };
		TMMFMessageDestination handleInfo(uid);
		TMMFMessageDestinationPckg message(handleInfo);

		err = controller->CustomCommandSync(message, KLogFunction, KNullDesC8, KNullDesC8, functionText);
		
#ifdef SYMBIAN_BUILD_GCE
		SurfaceSupported();
#endif
		if (err == KErrNone && !iSurfaceSupported) 
			{
			// If the controller doesn't support graphics surfaces, 
			// now that the opening of the controller is asynchronous,
			// we might get a MDirectScreenAccess::AbortNow() or 
			// MDirectScreenAccess::Restart callbback 
			// So 
			if ((functionText.Compare(_L8("SetDisplayWindowL called")) != 0) &&
		 	   (functionText.Compare(_L8("DirectScreenAccessEventL EAbortDSA")) != 0) &&
		 	   (functionText.Compare(_L8("DirectScreenAccessEventL EResumeDSA")) != 0))
				{
				ERR_PRINTF1(_L("SetDisplayWindowL not called"));
				iTestStepResult = EFail;
				}
			else
				{
				iTestStepResult = EPass;
				}
			}

#ifdef SYMBIAN_BUILD_GCE
		SurfaceSupported();
#endif	
		if (err == KErrNone && iSurfaceSupported) 
			{
			// If the controller supports graphics surfaces, 
			// we should not get a MDirectScreenAccess::AbortNow() or 
			// MDirectScreenAccess::Restart callbback or SetDisplayWindowL
			// So 
			if ((functionText.Compare(_L8("SetDisplayWindowL called")) == 0) ||
		 	   (functionText.Compare(_L8("DirectScreenAccessEventL EAbortDSA")) == 0) ||
		 	   (functionText.Compare(_L8("DirectScreenAccessEventL EResumeDSA")) == 0))
				{
				ERR_PRINTF1(_L("SetDisplayWindowL should not be called"));
				iTestStepResult = EFail;
				}
			else
				{
				INFO_PRINTF1(_L("Pass. SetDisplayWindowL is not called"));
				iTestStepResult = EPass;
				}
			}
		
#ifdef SYMBIAN_BUILD_GCE
		SurfaceSupported();
#endif	
		if (iTestStepResult == EPass && !iSurfaceSupported)
			{
			err = controller->CustomCommandSync(message, KLogFunction2, KNullDesC8, KNullDesC8, functionText);
			if (err == KErrNone) 
				{
				if (functionText.Compare(_L8("UpdateDisplayRegionL called")) != 0)
					{
					INFO_PRINTF1(_L("UpdateDisplayRegionL not called"));
					iTestStepResult = EFail;
					}
				else
					{
					iTestStepResult = EPass;
					}
				}
			}	
		}
	else
		{
		INFO_PRINTF3(_L("Unexpected error (err = %d, iError = %d)"), err, iError);
		iTestStepResult = EFail;
		}

	delete iPlayer;
	iPlayer = NULL;

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

