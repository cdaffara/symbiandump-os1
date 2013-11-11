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
#include <testframework.h>

// Test system includes
#include <testframework.h>

#include <mmf/common/mmfstandardcustomcommands.h>  
#include <mmf/common/mmfvideo.h>
#include <mmf/server/mmfdes.h>
#include <mmf/server/mmffile.h>

#include <mmf/common/mmfcontroller.h>
#include <videoplayer.h>

#ifdef SYMBIAN_BUILD_GCE
#include <videoplayer2.h>
#endif

#include <videorecorder.h>

// Specific includes for this test suite
#include "TestStepUnitMMFVidClient.h"
#include "TestSuiteUnitMMFVidClient.h"

// Specific includes for these test steps
#include "TestStepVidPlayer.h"
#include "TestStepVidRecorder.h"
#include "TestStepAllocFailVidPlayer.h"
#include "teststepvidplayereventhandler.h"

#ifdef SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT
#include "teststepvidplayersubtitle.h"
#endif //SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT

/** DLL Entry point */
//GLDEF_C TInt E32Dll(TDllReason /*aReason*/)
//	{
//	return(KErrNone);
//	}

/**
 *
 * Create an instance of this test suite.
 *
 * NewTestSuiteUnitMMFVidClientL is exported at ordinal 1.
 * This provides the interface to allow the Test Framework
 * to create instances of this test suite.
 *
 * @return	"CTestSuiteUnitMMFVidClient*"
 *			The newly created test suite
 */
EXPORT_C CTestSuiteUnitMMFVidClient* NewTestSuiteUnitMMFVidClientL() 
    { 
	CTestSuiteUnitMMFVidClient* result = new (ELeave) CTestSuiteUnitMMFVidClient;
	CleanupStack::PushL(result);
	result->ConstructL();
	CleanupStack::Pop(); // result
	return result;
    }

/**
 *
 * CTestSuiteUnitMMFVidClient destructor
 */
CTestSuiteUnitMMFVidClient::~CTestSuiteUnitMMFVidClient()
	{
	}

/**
 *
 * This test suite version string
 */
_LIT(KTxtVersion,"0.1a (build 4-Sept-2002)");

/**
 *
 * Get test suite version.
 *
 * @return	"TPtrC"
 *			The version string.
 */
TPtrC CTestSuiteUnitMMFVidClient::GetVersion() const
	{
	return KTxtVersion();
	}

/**
 *
 * Add a test step into the suite.
 *
 * @param	"CTestStepUnitMMFVidClient* aTestStep"
 *			The test step to be added.
 * @xxxx
 * 
 */
void CTestSuiteUnitMMFVidClient::AddTestStepL( CTestStepUnitMMFVidClient* aTestStep )
	{
	// test steps contain a pointer back to the suite which owns them
	aTestStep->SetUnitMMFVidClientSuite(this); 

	// add the step using the base class method
	CTestSuite::AddTestStepL(aTestStep);
	}

/**
 *
 * Initialiser for test suite.
 * This creates all the test steps and stores them
 * inside CTestSuiteUnitMMFVidClient
 */
void CTestSuiteUnitMMFVidClient::InitialiseL()
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
	iSuiteName = _L("TSU_MMF_VCLNT_01");

	// add test steps
	AddTestStepL(CTestStepVidPlayerNew::NewL());
	AddTestStepL(CTestStepVidPlayerNewPriorPref::NewL());
	AddTestStepL(CTestStepVidPlayerOpenFile::NewL(_L("MM-MMF-VCLNT-U-0003-CP"), CTestStepVidPlayerOpenFile::EFileName));
	AddTestStepL(CTestStepVidPlayerOpenFileCtrlUid::NewL(CTestStepVidPlayerOpenFileCtrlUid::EFileName));
	AddTestStepL(CTestStepVidPlayerOpenDesc::NewL());
	AddTestStepL(CTestStepVidPlayerOpenUrl::NewL());
	AddTestStepL(CTestStepVidPlayerClose::NewL());
	AddTestStepL(CTestStepVidPlayerPlay::NewL());
	AddTestStepL(CTestStepVidPlayerStop::NewL());
	AddTestStepL(CTestStepVidPlayerPause::NewL());
	AddTestStepL(CTestStepVidPlayerPriority::NewL());
	AddTestStepL(CTestStepVidPlayerSetDisplayWindow::NewL(EFalse));
	AddTestStepL(CTestStepVidPlayerRestart::NewL());
	AddTestStepL(CTestStepVidPlayerGetFrame::NewL(EFalse));
	AddTestStepL(CTestStepVidPlayerGetFrame::NewL(ETrue));
	AddTestStepL(CTestStepVidPlayerGetFrameRate::NewL());
	AddTestStepL(CTestStepVidPlayerGetVFrameSize::NewL());
	AddTestStepL(CTestStepVidPlayerGetVBitRate::NewL());
	AddTestStepL(CTestStepVidPlayerGetABitRate::NewL());
	AddTestStepL(CTestStepVidPlayerAudEnabled::NewL());
	AddTestStepL(CTestStepVidPlayerClipPosition::NewL());
	AddTestStepL(CTestStepVidPlayerDuration::NewL());
	AddTestStepL(CTestStepVidPlayerVideoType::NewL());
	AddTestStepL(CTestStepVidPlayerAudioType::NewL());
	AddTestStepL(CTestStepVidPlayerVol::NewL());
	AddTestStepL(CTestStepVidPlayerBalance::NewL());
	AddTestStepL(CTestStepVidPlayerMeta::NewL());
	// New tests for CR
	AddTestStepL(CTestStepVidPlayerPlayWindow::NewL());
	AddTestStepL(CTestStepVidPlayerGetImplementationInfo::NewL());
	AddTestStepL(CTestStepVidPlayerSendCustomCommandSync::NewL());
	AddTestStepL(CTestStepVidPlayerSendCustomCommandAsync::NewL());
	//DEF083334 SWSV_604_R4B_Video: Real media icon still visible when changing to a non realVideo file
	AddTestStepL(CTestStepVidPlayerCheckImplementationInfo::NewL());
	AddTestStepL(CTestStepVidPlayerGeneralEvent::NewL());
	AddTestStepL(CTestStepVidPlayerReloadingTest::NewL());
	AddTestStepL(CTestStepVidPlayerRefreshFrame::NewL());
	AddTestStepL(CTestStepVidPlayerRotation::NewL());
	AddTestStepL(CTestStepVidPlayerScale::NewL());
	AddTestStepL(CTestStepVidPlayerCropRegion::NewL());

	AddTestStepL(CTestStepOOMVidPlayerNew::NewL());
	AddTestStepL(CTestStepOOMVidPlayerOpenFile::NewL(EFalse));
	AddTestStepL(CTestStepOOMVidPlayerOpenDesc::NewL());
	AddTestStepL(CTestStepOOMVidPlayerOpenUrl::NewL());

	AddTestStepL(CTestStepVidRecorderNew::NewL());
	AddTestStepL(CTestStepVidRecorderOpenFile::NewL(EFalse));
	AddTestStepL(CTestStepVidRecorderOpenFileCtrlUid::NewL(EFalse));
	AddTestStepL(CTestStepVidRecorderOpenDesc::NewL());
	AddTestStepL(CTestStepVidRecorderOpenUrl::NewL());
	AddTestStepL(CTestStepVidRecorderPrepare::NewL());
	AddTestStepL(CTestStepVidRecorderClose::NewL());
	AddTestStepL(CTestStepVidRecorderRecord::NewL());
	AddTestStepL(CTestStepVidRecorderStop::NewL());
	AddTestStepL(CTestStepVidRecorderPause::NewL());
	AddTestStepL(CTestStepVidRecorderGain::NewL());
	AddTestStepL(CTestStepVidRecorderAudioType::NewL());
	AddTestStepL(CTestStepVidRecorderVideoType::NewL());
	AddTestStepL(CTestStepVidRecorderMaxClipSize::NewL());
	AddTestStepL(CTestStepVidRecorderRecordTime::NewL());
	AddTestStepL(CTestStepVidRecorderDuration::NewL());
	AddTestStepL(CTestStepVidRecorderAudEnabled::NewL());
	AddTestStepL(CTestStepVidRecorderABitRate::NewL());
	AddTestStepL(CTestStepVidRecorderVBitRate::NewL());
	AddTestStepL(CTestStepVidRecorderVFrameSize::NewL());
	AddTestStepL(CTestStepVidRecorderVFrameRate::NewL());
	AddTestStepL(CTestStepVidRecorderMeta::NewL());
	AddTestStepL(CTestStepVidRecorderHandleEvent::NewL());

	// New tests for CR
	AddTestStepL(CTestStepVidRecorderGetImplementationInfo::NewL());
	AddTestStepL(CTestStepVidRecorderSendCustomCommandSync::NewL());
	AddTestStepL(CTestStepVidRecorderSendCustomCommandAsync::NewL());
	AddTestStepL(CTestStepVidRecorderGeneralEvent::NewL());
	//DEF083334 SWSV_604_R4B_Video: Real media icon still visible when changing to a non realVideo file
	AddTestStepL(CTestStepVidRecorderCheckImplementationInfo::NewL());
	


	AddTestStepL(CTestStepAllocFailVidRecorderNew::NewL());
	AddTestStepL(CTestStepAllocFailVidRecorderOpenFile::NewL(EFalse));
	AddTestStepL(CTestStepAllocFailVidRecorderOpenFileCtrlUid::NewL(EFalse));
	AddTestStepL(CTestStepAllocFailVidRecorderOpenDesc::NewL());
	AddTestStepL(CTestStepAllocFailVidRecorderOpenUrl::NewL());

	// RFile Handle Tests
	AddTestStepL(CTestStepVidPlayerOpenFile::NewL(_L("MM-MMF-VCLNT-U-0011-CP"), CTestStepVidPlayerOpenFile::EFileHandle));
	AddTestStepL(CTestStepVidPlayerOpenFileCtrlUid::NewL(CTestStepVidPlayerOpenFileCtrlUid::EFileHandle));
	AddTestStepL(CTestStepOOMVidPlayerOpenFile::NewL(ETrue));
	AddTestStepL(CTestStepVidRecorderOpenFile::NewL(ETrue));
	AddTestStepL(CTestStepVidRecorderOpenFileCtrlUid::NewL(ETrue));
	AddTestStepL(CTestStepAllocFailVidRecorderOpenFile::NewL(ETrue));
	AddTestStepL(CTestStepAllocFailVidRecorderOpenFileCtrlUid::NewL(ETrue));

	// Use TMMSource to access unprotected content.
	AddTestStepL(CTestStepVidPlayerOpenFile::NewL(_L("MM-MMF-VCLNT-U-0008-CP"), CTestStepVidPlayerOpenFile::EFileSource));
	AddTestStepL(CTestStepVidPlayerOpenFile::NewL(_L("MM-MMF-VCLNT-U-0013-CP"), CTestStepVidPlayerOpenFile::EFileHandleSource));
	
	// DRM tests - postive
	AddTestStepL(CTestStepVidPlayerDrm::NewL(_L("MM-MMF-VCLNT-U-0501-HP"), CTestStepVidPlayerDrm::EFileSource, CTestStepVidPlayerDrm::EProtectedPlayIntent));
	AddTestStepL(CTestStepVidPlayerDrm::NewL(_L("MM-MMF-VCLNT-U-0502-HP"), CTestStepVidPlayerDrm::EFileHandleSource, CTestStepVidPlayerDrm::EProtectedPlayIntent));

	AddTestStepL(CTestStepVidPlayerDrm::NewAllocTestL(_L("MM-MMF-VCLNT-U-0505-HP"), CTestStepVidPlayerDrm::EFileSource, CTestStepVidPlayerDrm::EProtectedPlayIntent));
	AddTestStepL(CTestStepVidPlayerDrm::NewAllocTestL(_L("MM-MMF-VCLNT-U-0506-HP"), CTestStepVidPlayerDrm::EFileHandleSource, CTestStepVidPlayerDrm::EProtectedPlayIntent));


	// DRM tests - negative (with file name)
	AddTestStepL(CTestStepVidPlayerDrm::NewL(_L("MM-MMF-VCLNT-U-0601-HP"), CTestStepVidPlayerDrm::EFileSource, CTestStepVidPlayerDrm::EProtectedUnknownIntent));
	AddTestStepL(CTestStepVidPlayerDrm::NewL(_L("MM-MMF-VCLNT-U-0602-HP"), CTestStepVidPlayerDrm::EFileSource, CTestStepVidPlayerDrm::EProtectedPlayNoRights));
	
	// DRM tests - negative (with file handle)
	AddTestStepL(CTestStepVidPlayerDrm::NewL(_L("MM-MMF-VCLNT-U-0611-HP"), CTestStepVidPlayerDrm::EFileHandleSource, CTestStepVidPlayerDrm::EProtectedUnknownIntent));
	AddTestStepL(CTestStepVidPlayerDrm::NewL(_L("MM-MMF-VCLNT-U-0612-HP"), CTestStepVidPlayerDrm::EFileHandleSource, CTestStepVidPlayerDrm::EProtectedPlayNoRights));

	// INC041303  - PSS - PlaybackComplete problem in MMF Framework 
	AddTestStepL(CTestStepVidPlayerPlayPause::NewL());
	
	AddTestStepL(CTestStepVidPlayerStopStartDSA::NewL());
	
	AddTestStepL(CTestStepVidPlayerOpenviaUriTag::NewL());
	AddTestStepL(CTestStepVidPlayerOpenTryNextCtrl::NewL());
	// DEF081389,DEF082497 Duration Info custom command has User::Leave called in non-leaving function
	AddTestStepL(CTestStepVidPlayerOpenDurationCustomCommand::NewL());
	//DEF092672
	AddTestStepL(CTestStepVideoInterruptionByAudio::NewL());
	//CR0805
	CTestSuite::AddTestStepL(CTestStepVidPlayerARN0700::NewL());
	CTestSuite::AddTestStepL(CTestStepVidPlayerARN0701::NewL());
	CTestSuite::AddTestStepL(CTestStepVidPlayerARN0702::NewL());
	CTestSuite::AddTestStepL(CTestStepVidPlayerARN0703::NewL());
	//DEF100150
	AddTestStepL(CTestStepVidOnlyPlayerNoAudioType::NewL());

	// DEF097361
	AddTestStepL(CTestStepVidRecordError::NewL());

	// INC122181
	AddTestStepL(CTestStepINC122181::NewL());

#ifdef SYMBIAN_BUILD_GCE	
	// Positive tests with CVideoPlayerUtility2
	AddTestStepL(CTestStepVidPlayerNew::NewL(ETrue));
	AddTestStepL(CTestStepVidPlayerNewPriorPref::NewL(ETrue));
	
	// Positive tests with CVideoPlayerUtility2 when the controller supports graphics surface
	AddTestStepL(CTestStepVidPlayerAddDisplayWindow::NewL(EFalse, KErrNone)); // not simplified method 
	AddTestStepL(CTestStepVidPlayerAddDisplayWindow::NewL(ETrue, KErrNone)); // simplified method
	AddTestStepL(CTestStepVidPlayerRemoveDisplayWindow::NewL(ETrue));
	AddTestStepL(CTestStepVidPlayerRemoveDisplayWindow::NewL(EFalse));
	AddTestStepL(CTestStepVidPlayerSetVideoExtent::NewL(KErrNone));
	AddTestStepL(CTestStepVidPlayerSetWindowClipRect::NewL(KErrNone));
	AddTestStepL(CTestStepVidPlayerOpenFile::NewL(_L("MM-MMF-VCLNT-U-0803-CP"), CTestStepVidPlayerOpenFile::EFileName));
	AddTestStepL(CTestStepVidPlayerOpenDesc::NewL(CTestStepUnitMMFVidClient::ESurfaceMode));
	AddTestStepL(CTestStepVidPlayerOpenUrl::NewL(CTestStepUnitMMFVidClient::ESurfaceMode));
	AddTestStepL(CTestStepVidPlayerOpenFile::NewL(_L("MM-MMF-VCLNT-U-0809-CP"), CTestStepVidPlayerOpenFile::EFileHandle));
	AddTestStepL(CTestStepVidPlayerOpenFile::NewL(_L("MM-MMF-VCLNT-U-0811-CP"), CTestStepVidPlayerOpenFile::EFileSource));
	AddTestStepL(CTestStepVidPlayerOpenFileCtrlUid::NewL(CTestStepVidPlayerOpenFileCtrlUid::EFileName, CTestStepUnitMMFVidClient::ESurfaceMode));
	AddTestStepL(CTestStepVidPlayerOpenFileCtrlUid::NewL(CTestStepVidPlayerOpenFileCtrlUid::EFileHandle, CTestStepUnitMMFVidClient::ESurfaceMode));
	AddTestStepL(CTestStepVidPlayerOpenFileCtrlUid::NewL(CTestStepVidPlayerOpenFileCtrlUid::EDescriptor, CTestStepUnitMMFVidClient::ESurfaceMode));
	AddTestStepL(CTestStepVidPlayerOpenFileCtrlUid::NewL(CTestStepVidPlayerOpenFileCtrlUid::EUrl, CTestStepUnitMMFVidClient::ESurfaceMode));
	AddTestStepL(CTestStepVidPlayerReplaceSurfaceSequence1::NewL());
	AddTestStepL(CTestStepVidPlayerReplaceSurfaceSequence2::NewL());
	AddTestStepL(CTestStepVidPlayerAutoScale::NewL(_L("MM-MMF-VCLNT-U-1000-CP")));
	AddTestStepL(CTestStepVidPlayerAutoScale::NewL(_L("MM-MMF-VCLNT-U-1001-CP")));
	AddTestStepL(CTestStepVidPlayerAutoScale::NewL(_L("MM-MMF-VCLNT-U-1002-CP")));
	AddTestStepL(CTestStepVidPlayerAutoScale::NewL(_L("MM-MMF-VCLNT-U-1003-CP")));
	AddTestStepL(CTestStepVidPlayerAutoScale::NewL(_L("MM-MMF-VCLNT-U-1004-CP")));
	AddTestStepL(CTestStepVidPlayerAutoScale::NewL(_L("MM-MMF-VCLNT-U-1005-CP")));
	AddTestStepL(CTestStepVidPlayerAutoScale::NewL(_L("MM-MMF-VCLNT-U-1006-CP")));
	AddTestStepL(CTestStepVidPlayerAutoScale::NewL(_L("MM-MMF-VCLNT-U-1007-CP")));
	AddTestStepL(CTestStepVidPlayerAddDisplay::NewL(_L("MM-MMF-VCLNT-U-1100-CP")));
	AddTestStepL(CTestStepVidPlayerAddDisplay::NewL(_L("MM-MMF-VCLNT-U-1101-CP")));
	AddTestStepL(CTestStepVidPlayerEvent::NewL(_L("MM-MMF-VCLNT-U-1102-CP")));
	AddTestStepL(CTestStepVidPlayerEvent::NewL(_L("MM-MMF-VCLNT-U-1104-CP")));
	AddTestStepL(CTestStepVidPlayer1106::NewL());
	AddTestStepL(CTestStepVidPlayer1107::NewL());
	AddTestStepL(CTestStepVidPlayer1108::NewL());
	AddTestStepL(CTestStepVidPlayerEvent::NewL(_L("MM-MMF-VCLNT-U-1109-CP")));
	AddTestStepL(CTestStepVidPlayer1110::NewL());
	AddTestStepL(CTestStepVidPlayer1111::NewL());
	AddTestStepL(CTestStepVidPlayerEvent::NewL(_L("MM-MMF-VCLNT-U-1112-CP")));	
	AddTestStepL(CTestStepVidPlayerEvent::NewL(_L("MM-MMF-VCLNT-U-1113-CP")));
	AddTestStepL(CTestStepVidPlayerEvent::NewL(_L("MM-MMF-VCLNT-U-1114-CP")));
	AddTestStepL(CTestStepVidPlayerEvent::NewL(_L("MM-MMF-VCLNT-U-1115-CP")));
	AddTestStepL(CTestStepVidPlayer1200::NewL());
	AddTestStepL(CTestStepVidPlayer1201::NewL());
	AddTestStepL(CTestStepVidPlayer1202::NewL());
	
	// graphics surface  support,OOM tests 
	AddTestStepL(CTestStepOOMVidPlayerNew::NewL(ETrue));
	AddTestStepL(CTestStepOOMVidPlayerOpenFile::NewL(EFalse,CTestStepUnitMMFVidClient::ESurfaceMode));
	AddTestStepL(CTestStepOOMVidPlayerOpenFile::NewL(ETrue,CTestStepUnitMMFVidClient::ESurfaceMode));
	AddTestStepL(CTestStepOOMVidPlayerOpenDesc::NewL(CTestStepUnitMMFVidClient::ESurfaceMode));
	AddTestStepL(CTestStepOOMVidPlayerOpenUrl::NewL(CTestStepUnitMMFVidClient::ESurfaceMode));

	// negative tests with CVideoPlayerUtility2
	AddTestStepL(CTestStepVidPlayerSetDisplayWindow::NewL(ETrue, CTestStepUnitMMFVidClient::ESurfaceMode));
	AddTestStepL(CTestStepVidPlayerSetInitScreenNumber::NewL(ETrue));
	AddTestStepL(CTestStepVidPlayerStopStartDSA::NewL(CTestStepUnitMMFVidClient::ESurfaceMode));
	
	// negative tests with CVideoPlayerUtility2 when the controller doesn't support graphics surface
	AddTestStepL(CTestStepVidPlayerOpenFile::NewL(_L("MM-MMF-VCLNT-U-0850-CP"), CTestStepVidPlayerOpenFile::EFileName));
	AddTestStepL(CTestStepVidPlayerOpenDesc::NewL(CTestStepUnitMMFVidClient::ESurfaceModeNotSupported));
	AddTestStepL(CTestStepVidPlayerOpenUrl::NewL(CTestStepUnitMMFVidClient::ESurfaceModeNotSupported));
	AddTestStepL(CTestStepVidPlayerOpenFile::NewL(_L("MM-MMF-VCLNT-U-0853-CP"), CTestStepVidPlayerOpenFile::EFileHandle));

	// negative tests with CVideoPlayerUtility2 when the controller supports graphics surface
	AddTestStepL(CTestStepVidPlayerAddDisplayWindow::NewL(EFalse, KErrInUse, ETrue));	// not simplified method, add a same window
	AddTestStepL(CTestStepVidPlayerAddDisplayWindow::NewL(EFalse, KErrInUse, EFalse));	// not simplified method, add a different window
	AddTestStepL(CTestStepVidPlayerAddDisplayWindow::NewL(EFalse, KErrNotReady));
	AddTestStepL(CTestStepVidPlayerAddDisplayWindow::NewL(EFalse, KErrArgument));
	AddTestStepL(CTestStepVidPlayerAddDisplayWindow::NewL(ETrue, KErrInUse)); // simplified method
	AddTestStepL(CTestStepVidPlayerAddDisplayWindow::NewL(ETrue, KErrNotReady));
	AddTestStepL(CTestStepVidPlayerSetVideoExtent::NewL(KErrNotReady));
	AddTestStepL(CTestStepVidPlayerSetWindowClipRect::NewL(KErrNotReady));
	AddTestStepL(CTestStepVidPlayerSetWindowClipRect::NewL(KErrArgument));
	
#endif
	
#ifdef SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT
	AddTestStepL(CTestStepVidPlayerSubtitleAvailable::NewL(_L("MM-MMF-VCLNT-U-1300-CP")));
	AddTestStepL(CTestStepVidPlayerSubtitleEnableDisable::NewL(_L("MM-MMF-VCLNT-U-1301-CP")));
	AddTestStepL(CTestStepVidPlayerSubtitleRedraw::NewL(_L("MM-MMF-VCLNT-U-1302-HP")));
	AddTestStepL(CTestStepVidPlayerSubtitleRedrawNotReady::NewL(_L("MM-MMF-VCLNT-U-1303-HP")));
	AddTestStepL(CTestStepVidPlayerSubtitleMultiWindow::NewL(_L("MM-MMF-VCLNT-U-1304-HP")));
	AddTestStepL(CTestStepVidPlayerSubtitleAddRemoveMultiWindow::NewL(_L("MM-MMF-VCLNT-U-1305-HP")));
	AddTestStepL(CTestStepVidPlayerSubtitleMultiWindowSettingChange::NewL(_L("MM-MMF-VCLNT-U-1306-MP"), ETrue));
	AddTestStepL(CTestStepVidPlayerSubtitleMultiWindowSettingChange::NewL(_L("MM-MMF-VCLNT-U-1307-MP"), EFalse));
	AddTestStepL(CTestStepVidPlayerSubtitleLanguage::NewL(_L("MM-MMF-VCLNT-U-1308-HP")));
	AddTestStepL(CTestStepVidPlayerSubtitleWindowSettingChange::NewL(_L("MM-MMF-VCLNT-U-1309-HP"), ETrue));
	AddTestStepL(CTestStepVidPlayerSubtitleWindowSettingChange::NewL(_L("MM-MMF-VCLNT-U-1310-HP"), EFalse));
	AddTestStepL(CTestStepVidPlayerSubtitleEnableNeg::NewL(_L("MM-MMF-VCLNT-U-1350-CP"), ETrue, EFalse, EFalse, EFalse, KErrNotFound));
	AddTestStepL(CTestStepVidPlayerSubtitleEnableNeg::NewL(_L("MM-MMF-VCLNT-U-1351-CP"), EFalse, ETrue, EFalse, EFalse, KErrNotReady));
	AddTestStepL(CTestStepVidPlayerSubtitleEnableNeg::NewL(_L("MM-MMF-VCLNT-U-1352-MP"), EFalse, EFalse, ETrue, EFalse, KErrInUse));
	AddTestStepL(CTestStepVidPlayerSubtitleEnableNeg::NewL(_L("MM-MMF-VCLNT-U-1353-CP"), EFalse, EFalse, EFalse, ETrue, KErrNotReady));
	AddTestStepL(CTestStepVidPlayerSubtitleLangNotEnable::NewL(_L("MM-MMF-VCLNT-U-1354-HP"), ETrue, EFalse, EFalse));
	AddTestStepL(CTestStepVidPlayerSubtitleLangNotEnable::NewL(_L("MM-MMF-VCLNT-U-1355-HP"), EFalse, ETrue, EFalse));
	AddTestStepL(CTestStepVidPlayerSubtitleLangNotEnable::NewL(_L("MM-MMF-VCLNT-U-1356-HP"), EFalse, EFalse, ETrue));
	AddTestStepL(CTestStepVidPlayerSubtitleLangNotAvail::NewL(_L("MM-MMF-VCLNT-U-1357-HP")));
	AddTestStepL(CTestStepVidPlayerSubtitleNotEnabledRedraw::NewL(_L("MM-MMF-VCLNT-U-1358-LP")));
#endif //SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT
	}

#ifdef __WINS__
//
// Wait synchronously for the first WS redraw event...
//
TBool CTestSuiteUnitMMFVidClient::WaitUntilWindowServerReadyL()
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

void CTestSuiteUnitMMFVidClient::InitWservL()
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

void CTestSuiteUnitMMFVidClient::DeInitWserv()
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

// -------------------------

