
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
// This main DLL entry point for the TS_WP5.dll
// 
//


// EPOC includes
#include <e32base.h>

// Test system includes
#include <testframework.h>

#include "TSI_MMFVCLNT.h"
#include "TS_MMFVCLNTSuite.h"
#include "TestPlayer.h"
#include "TestRecord.h"
#include "DRMTestVidPlayer.h"
#include "OpenFileByHandle0034.h"
#include "OpenFileByHandle0036.h"
#include "OpenFileByHandle0522.h"
#include "OpenFileByHandle0523.h"
#include "OpenFileByHandle0524.h"
#include "OpenFileByHandle1011.h"
#include "OpenFileByHandle_PlayText.h"
#include "OpenFileByHandle1511.h"
#include "OpenFileByHandle1512.h"
#include "OpenFileByHandle1513.h"



/**
 * NewTestSuite is exported at ordinal 1
 * this provides the interface to allow schedule test
 * to create instances of this test suite
 */
EXPORT_C CTestSuite* NewTestSuiteL()
    {
	CTestMmfVclntSuite* result = new (ELeave) CTestMmfVclntSuite;
	CleanupStack::PushL(result);
	result->ConstructL();
	CleanupStack::Pop(); // result
	return result;
    }

/**
 * destructor
 */
CTestMmfVclntSuite::~CTestMmfVclntSuite()
	{
	}

/**
 * make a version string available for test system
 * @return version string
 */
TPtrC CTestMmfVclntSuite::GetVersion()
	{
	_LIT(KTxtVersion,"1.0");
	return KTxtVersion();
	}

/**
 * Add a test step into the suite
 *
 * @param test step pointer to add
 */
void CTestMmfVclntSuite::AddTestStepL( CTestMmfVclntStep* aPtrTestStep )
	{
	// test steps contain a pointer back to the suite which owns them
	aPtrTestStep->SetTestSuite(this);
	// add the step using tyhe base class method
	CTestSuite::AddTestStepL(aPtrTestStep);
	}

/**
 * Constructor for test suite
 * this creates all the test steps and 
 * stores them inside CTestMmfVclntSuite 
 */
void CTestMmfVclntSuite::InitialiseL()
	{
#ifdef __WINS__
	// Check that the window server is ready for us
	// This is so that the call to CDirectScreenAccess::StartL does not leave with KErrNotReady
	if (!WaitUntilWindowServerReadyL())
		{
		INFO_PRINTF1(_L("TestSuite error: error receiving window server redraw event"));
		return;
		}
#endif // __WINS__

	// store the name of this test suite 
	iSuiteName = _L("MMF_VCLNT");

	TTimeIntervalMicroSeconds position = TInt64(0);

	//Play video file
	AddTestStepL( CTestVclntVideoFile::NewL( _L("MM-MMF-VCLNT-I-0001-CP"), _L("SectionOne"), _L("filename"), ETrue) );
	
	// Play video descriptor
	AddTestStepL( CTestVclntVideoDes::NewL( _L("MM-MMF-VCLNT-I-0002-CP"), _L("SectionOne"), _L("filename"), ETrue) );
	
	AddTestStepL( CTestVclntEnqFrameRate::NewL( _L("MM-MMF-VCLNT-I-0010-CP"), _L("SectionOne"), _L("filename"), 10) );
	AddTestStepL( CTestVclntPosition::NewL( _L("MM-MMF-VCLNT-I-0011-CP"), _L("SectionOne"), _L("filename"), position)  ); 
	AddTestStepL( CTestVclntPriority::NewL( _L("MM-MMF-VCLNT-I-0012-CP"), _L("SectionOne"), _L("filename"), -1/*EPriorityMin*/) );
	AddTestStepL( CTestVclntDuration::NewL( _L("MM-MMF-VCLNT-I-0015-HP"), _L("SectionOne"), _L("filename"), position) );
	AddTestStepL( CTestVclntVolume::NewL( _L("MM-MMF-VCLNT-I-0016-HP"), _L("SectionOne"), _L("filename"), -1) );
	AddTestStepL( CTestVclntCloseOpen::NewL( _L("MM-MMF-VCLNT-I-0017-HP"), _L("SectionOne"), _L("filename")) );
	AddTestStepL( CTestVclntPause::NewL( _L("MM-MMF-VCLNT-I-0018-HP"), _L("SectionOne"), _L("filename")) );
	AddTestStepL( CTestVclntBalance::NewL( _L("MM-MMF-VCLNT-I-0019-HP"), _L("SectionOne"), _L("filename"),-10) );
	AddTestStepL( CTestVclntPlayWindow::NewL( _L("MM-MMF-VCLNT-I-0020-HP"), _L("SectionOne"), _L("filename"), position , position) );
	AddTestStepL( CTestVclntMeta::NewL( _L("MM-MMF-VCLNT-I-0021-HP"), _L("SectionOne"), _L("filename")) );
	AddTestStepL( CTestVclntFrameSize::NewL( _L("MM-MMF-VCLNT-I-0022-HP"), _L("SectionOne"), _L("filename"),12) );
	AddTestStepL( CTestVclntRepeat::NewL( _L("MM-MMF-VCLNT-I-0023-HP"), _L("SectionOne"), _L("filename"),3) );
	AddTestStepL( CTestVclntMimeType::NewL( _L("MM-MMF-VCLNT-I-0031-LP"), _L("SectionOne"), _L("filename")) );
	AddTestStepL( CTestVclntRebuffering::NewL( _L("MM-MMF-VCLNT-I-0032-LP"), _L("SectionOne"), _L("filename")) );

	// RecorderUtils

	AddTestStepL( CTestVclntRecordVideoFile::NewL( _L("MM-MMF-VCLNT-I-0502-CP"), _L("SectionTwo"), _L("filename"), ETrue) );
	AddTestStepL( CTestVclntRecorderVideoDes::NewL( _L("MM-MMF-VCLNT-I-0503-CP"), _L("SectionTwo"), _L("filename"), ETrue) );
	AddTestStepL( CTestVclntRecorderVideoUrl::NewL( _L("MM-MMF-VCLNT-I-0504-LP"), _L("SectionTwo"), _L("filename"), ETrue) );
	AddTestStepL( CTestVclntRecordCloseOpen::NewL( _L("MM-MMF-VCLNT-I-0505-HP"), _L("SectionTwo"), _L("filename")) );
	AddTestStepL( CTestVclntRecordConfig::NewL( _L("MM-MMF-VCLNT-I-0506-CP")) );
	AddTestStepL( CTestVclntRecordPause::NewL( _L("MM-MMF-VCLNT-I-0507-HP"), _L("SectionTwo"), _L("filename")) );

	AddTestStepL( CTestVclntRecordDuration::NewL( _L("MM-MMF-VCLNT-I-0509-HP"), _L("SectionTwo"), _L("filename"), position) );
	AddTestStepL( CTestVclntRecordTime::NewL( _L("MM-MMF-VCLNT-I-0510-HP"), _L("SectionTwo"), _L("filename"), 10) ); 
	AddTestStepL( CTestVclntRecordCrop::NewL( _L("MM-MMF-VCLNT-I-0511-HP"), _L("SectionTwo"), _L("filename"), ETrue) );
	AddTestStepL( CTestVclntRecordGain::NewL( _L("MM-MMF-VCLNT-I-0512-HP"), _L("SectionTwo"), _L("filename"), 10) );
	AddTestStepL( CTestVclntRecordMeta::NewL( _L("MM-MMF-VCLNT-I-0513-LP"), _L("SectionTwo"), _L("filename")) );
	AddTestStepL( CTestVclntRecordPriority::NewL( _L("MM-MMF-VCLNT-I-0514-LP"), _L("SectionOne"), _L("filename"), -1/*EPriorityMin*/) );
	AddTestStepL( CTestVclntRecordBalance::NewL( _L("MM-MMF-VCLNT-I-0515-HP"), _L("SectionTwo"), _L("filename"), -10) );
	AddTestStepL( CTestVclntRecordSize::NewL( _L("MM-MMF-VCLNT-I-0516-LP"), _L("SectionTwo"), _L("filename")) );
	AddTestStepL( CTestVclntRecordTypes::NewL( _L("MM-MMF-VCLNT-I-0521-LP"), _L("SectionTwo"), _L("filename")) );

//
// NEGATIVE TEST 
//

	AddTestStepL( CTestVclntDelete::NewL( _L("MM-MMF-VCLNT-I-1001-CP"), _L("SectionOne"), _L("filename")) );
	AddTestStepL( CTestVclntFrameSize::NewL( _L("MM-MMF-VCLNT-I-1002-CP"), _L("SectionOne"), _L("filename"),-12) );
	position = 103423423;
	AddTestStepL( CTestVclntPosition::NewL( _L("MM-MMF-VCLNT-I-1003-CP"), _L("SectionOne"), _L("filename"), position) ); 
	position = -12;
	AddTestStepL( CTestVclntPosition::NewL( _L("MM-MMF-VCLNT-I-1004-CP"), _L("SectionOne"), _L("filename"), position) ); 
	AddTestStepL( CTestVclntVolume::NewL( _L("MM-MMF-VCLNT-I-1005-HP"), _L("SectionOne"), _L("filename"), 1000400) );
	AddTestStepL( CTestVclntVolume::NewL( _L("MM-MMF-VCLNT-I-1010-HP"), _L("SectionOne"), _L("filename"), -100) );
	AddTestStepL( CTestVclntBalance::NewL( _L("MM-MMF-VCLNT-I-1006-HP"), _L("SectionOne"), _L("filename"),200) );
	AddTestStepL( CTestVclntBalance::NewL( _L("MM-MMF-VCLNT-I-1011-HP"), _L("SectionOne"), _L("filename"),-200) );
	position = 2000000;
	AddTestStepL( CTestVclntPlayWindow::NewL( _L("MM-MMF-VCLNT-I-1007-HP"), _L("SectionOne"), _L("filename"), position , position) );
	AddTestStepL( CTestVclntVideoFile::NewL( _L("MM-MMF-VCLNT-I-1008-HP"), _L("SectionOne"), _L("corruptFile"), ETrue) );
	AddTestStepL( CTestVclntVideoFile::NewL( _L("MM-MMF-VCLNT-I-1009-HP"), _L("SectionOne"), _L("NoCodecFile"), ETrue) );

	// RecorderUtils

	AddTestStepL( CTestVclntRecordVideoFile::NewL( _L("MM-MMF-VCLNT-I-1501-HP"), _L("SectionTwo"), _L("unsupported"), ETrue) );
	AddTestStepL( CTestVclntRecordDelete::NewL( _L("MM-MMF-VCLNT-I-1502-LP"), _L("SectionTwo"), _L("unsupported")) );
	AddTestStepL( CTestVclntRecordGain::NewL( _L("MM-MMF-VCLNT-I-1503-HP"), _L("SectionTwo"), _L("filename"), 1005670) );
	AddTestStepL( CTestVclntRecordGain::NewL( _L("MM-MMF-VCLNT-I-1504-HP"), _L("SectionTwo"), _L("filename"), -10) );
	
	AddTestStepL( CTestVclntRecordBalance::NewL( _L("MM-MMF-VCLNT-I-1507-HP"), _L("SectionTwo"), _L("filename"), -200) );
	AddTestStepL( CTestVclntRecordBalance::NewL( _L("MM-MMF-VCLNT-I-1508-HP"), _L("SectionTwo"), _L("filename"), 200) );
	AddTestStepL( CTestVclntRecordSize::NewL( _L("MM-MMF-VCLNT-I-1510-LP"), _L("SectionTwo"), _L("filename")) );

	//DRM
	//GetFrame unprotected file 
	AddTestStepL( CTestStepVidPlayerDRMGetFrame::NewL( _L("MM-MMF-VCLNT-I-1600-LP"), _L("SectionDRM"), _L("filename")) );
	
	//Getframe from DRM protect file, with rights play count set to zero
	AddTestStepL( CTestStepVidPlayerDRMGetFrameProtected::NewL( _L("MM-MMF-VCLNT-I-1601-LP"), _L("SectionDRM"), _L("DRMFile")) );
	
	// Open unprotected file and play it no rights
	AddTestStepL( CTestStepVidPlayerDRMPlayNoRights::NewL( _L("MM-MMF-VCLNT-I-1602-LP"), _L("SectionDRM"), _L("DRMFile")) );
	
	//Open DRM protected video and play it with rights set and Agent providing MIME type 
	AddTestStepL( CTestStepVidPlayerDRMPlayRights::NewL( _L("MM-MMF-VCLNT-I-1603-LP"), _L("SectionDRM"), _L("DRMFile")) );

	//Media Access by Handle
	AddTestStepL( CTestMmfVclntOpenFile0034::NewL( _L("MM-MMF-VCLNT-I-0034-CP"), _L("SectionOne"), _L("filename"), ETrue ) );
	AddTestStepL( CTestMmfVclntOpenFile0036::NewL( _L("MM-MMF-VCLNT-I-0036-CP"), _L("SectionOne"), _L("filename"), ETrue ) );
	AddTestStepL( CTestMmfVclntOpenFile0522::NewL( _L("MM-MMF-VCLNT-I-0522-CP"), _L("SectionTwo"), _L("filename"), ETrue ) );
	AddTestStepL( CTestMmfVclntOpenFile0523::NewL( _L("MM-MMF-VCLNT-I-0523-CP"), _L("SectionTwo"), _L("filename"), ETrue ) );
	AddTestStepL( CTestMmfVclntOpenFile0524::NewL( _L("MM-MMF-VCLNT-I-0524-CP"), _L("SectionTwo"), _L("filename"), ETrue ) );
	AddTestStepL( CTestMmfVclntOpenFile1011::NewL( _L("MM-MMF-VCLNT-I-1011-CP"), _L("SectionTwo"), _L("filename"), ETrue ) );
	AddTestStepL( CTestMmfVclntOpenFilePlayTextFile::NewL( _L("MM-MMF-VCLNT-I-1010-CP"), _L("SectionOne"), _L("textFile"), ETrue ) );

	AddTestStepL( CTestMmfVclntOpenFile1511::NewL( _L("MM-MMF-VCLNT-I-1511-CP"), _L("SectionTwo"), _L("filename"), ETrue ) );
	AddTestStepL( CTestMmfVclntOpenFile1512::NewL( _L("MM-MMF-VCLNT-I-1512-CP"), _L("SectionTwo"), _L("filename"), ETrue ) );
	AddTestStepL( CTestMmfVclntOpenFile1513::NewL( _L("MM-MMF-VCLNT-I-1513-CP"), _L("SectionTwo"), _L("textFile"), ETrue ) );
	}

#ifdef __WINS__
//
// Wait synchronously for the first WS redraw event...
//
TBool CTestMmfVclntSuite::WaitUntilWindowServerReadyL()
	{
	InitWservL();

	TWsRedrawEvent event;
	TRequestStatus status;
	iWs.RedrawReady(&status);
	User::WaitForRequest(status);
	iWs.GetRedraw(event);

	DeInitWserv();

	return (status.Int() == KErrNone);
	}

void CTestMmfVclntSuite::InitWservL()
	{
	TInt err = iWs.Connect();
	if (err != KErrNone)
		{
		// Access violation if ws is null
		INFO_PRINTF1(_L("TestSuite error: Cannot test, no window server available"));
		User::Leave(err);
		}

	iScreen = new (ELeave) CWsScreenDevice(iWs); // make device for this session
	User::LeaveIfError(iScreen->Construct()); // and complete its construction

	iRootWindow = RWindowGroup(iWs);
	User::LeaveIfError(iRootWindow.Construct((TUint32)this, ETrue));

	iWindow = new(ELeave) RWindow(iWs);
	User::LeaveIfError(((RWindow*)iWindow)->Construct(iRootWindow,((TUint32)(this)) + 1));
	iWindow->SetExtent(TPoint(0,0), TSize(400,200));
	iWindow->SetVisible(ETrue);
	iWindow->Activate();
	iWs.Flush();
	}

void CTestMmfVclntSuite::DeInitWserv()
	{
	if (iWindow)
		{
		iWindow->Close();
		delete iWindow;
		iWindow = NULL;
		}
	iRootWindow.Close();
	delete iScreen;
	iScreen = NULL;
	iWs.Flush();
	iWs.Close();
	}
#endif // __WINS__

