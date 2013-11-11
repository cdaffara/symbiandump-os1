// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32base.h>
#include <testframework.h>
#include "tsu_mmf_avictrl_testsuite.h"
#include "tsu_mmf_avictrl_teststep.h"
#include "tsu_mmf_avictrlplay.h"
#include "tsu_mmf_avictrlrecord.h"
#include "tsu_mmf_avictrltestids.h"

_LIT(KTextVersion,"0.1");
_LIT(KTestCameraPluginName, "C:\\102070cc.txt");
_LIT(KMMCameraPluginName, "C:\\mmcameraclientplugin.txt");

#ifndef EKA2
/**
Required for all DLLs but not used
*/
GLDEF_C TInt E32Dll(enum TDllReason)
	{
	return 0;
	}					    
#endif

/**
NewTestSuite is exported at ordinal 1this provides the interface to allow schedule test to create instances of this test suite.
@return CTestSuiteAviCtrl*
*/
EXPORT_C CTestSuiteAviCtrl* NewTestSuiteAviCtrlL()
	{ 
	CTestSuiteAviCtrl* self = new (ELeave) CTestSuiteAviCtrl;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
    	}
/**
Destructor
*/
CTestSuiteAviCtrl::~CTestSuiteAviCtrl()
	{
	RemoveDependencyFile();
	}

/**
Make a version string available for test system
@return the version.
*/
TPtrC CTestSuiteAviCtrl::GetVersion()
	{
	return KTextVersion();
	}

/**
Add a test step into the suite
@param aPtrTestStep
*/
void CTestSuiteAviCtrl::AddTestStepL(RTestStepAviCtrlBase* aPtrTestStep)
	{
	 ASSERT(aPtrTestStep);
	aPtrTestStep->SetSuite(this); 
	CTestSuite::AddTestStepL(aPtrTestStep);
	}

/**
Add a RAsyncTestStep based test step into the suite 
@param aPtrTestStep
*/
void CTestSuiteAviCtrl::AddTestStepL(RAsyncTestStepAviCtrlBase* aPtrTestStep)
	{
	 ASSERT(aPtrTestStep);
	aPtrTestStep->SetSuite(this); 
	CTestSuite::AddTestStepL(aPtrTestStep);
	}

/**
 Constructor for test suite this creates all the test steps and  stores them inside CTestSuiteAviCtrl 
 */
void CTestSuiteAviCtrl::InitialiseL( void )
	{
	InitSystemPath();
	// Create the dependancy files.
	CreateDependencyFileL();
	iSuiteName = _L("tsu_mmf_avictrl");
	//Steps to test PlayController
	AddTestStepL( new(ELeave) RTestStepAviPlayCtrlPlay(_L("MM-MMF-AVICTRL-U-0001-HP"), _L("SectionPlayer"), _L("video_only"), KAviPlayControllerUid));
	AddTestStepL( new(ELeave) RTestStepAviPlayCtrlVolume(_L("MM-MMF-AVICTRL-U-0002-HP"), _L("SectionPlayer"), _L("aud_vid"), KAviPlayControllerUid));
	AddTestStepL( new(ELeave) RTestStepAviPlayCtrlPlayBalance(_L("MM-MMF-AVICTRL-U-0003-HP"), _L("SectionPlayer"), _L("aud_vid"), KAviPlayControllerUid));
	AddTestStepL( new(ELeave) RTestStepAviPlayCtrlDuration(_L("MM-MMF-AVICTRL-U-0004-HP"), _L("SectionPlayer"), _L("video_only"), KAviPlayControllerUid));
	AddTestStepL( new(ELeave) RTestStepAviPlayCtrlPause(_L("MM-MMF-AVICTRL-U-0005-HP"), _L("SectionPlayer"), _L("video_only"), KAviPlayControllerUid));
	AddTestStepL( new(ELeave) RTestStepAviPlayCtrlStop(_L("MM-MMF-AVICTRL-U-0006-HP"), _L("SectionPlayer"), _L("video_only"), KAviPlayControllerUid));
	AddTestStepL( new(ELeave) RTestStepAviPlayCtrlSetGetCropRegion(_L("MM-MMF-AVICTRL-U-0007-HP"), _L("SectionPlayer"), _L("video_only"), KAviPlayControllerUid));
	AddTestStepL( new(ELeave) RTestStepAviPlayCtrlSetGetFrameRate(_L("MM-MMF-AVICTRL-U-0008-HP"), _L("SectionPlayer"), _L("video_only"), KAviPlayControllerUid));
	AddTestStepL( new(ELeave) RTestStepAviPlayCtrlGetFrameSize(_L("MM-MMF-AVICTRL-U-0009-HP"), _L("SectionPlayer"), _L("video_only"), KAviPlayControllerUid));
	AddTestStepL( new(ELeave) RTestStepAviPlayCtrlGetAudioDataType(_L("MM-MMF-AVICTRL-U-0010-HP"), _L("SectionPlayer"), _L("aud_vid"), KAviPlayControllerUid));
	AddTestStepL( new(ELeave) RTestStepAviPlayCtrlGetVideoDataType(_L("MM-MMF-AVICTRL-U-0011-HP"), _L("SectionPlayer"), _L("video_only"), KAviPlayControllerUid));
	AddTestStepL( new(ELeave) RTestStepAviPlayCtrlMetaData(_L("MM-MMF-AVICTRL-U-0012-HP"), _L("SectionPlayer"), _L("video_only"), KAviPlayControllerUid));
	AddTestStepL( new(ELeave) RTestStepAviPlayCtrlSetGetRotation(_L("MM-MMF-AVICTRL-U-0013-HP"), _L("SectionPlayer"), _L("video_only"), KAviPlayControllerUid));
	AddTestStepL( new(ELeave) RTestStepAviPlayCtrlSetGetScaling(_L("MM-MMF-AVICTRL-U-0014-HP"), _L("SectionPlayer"), _L("video_only"), KAviPlayControllerUid));
	AddTestStepL( new(ELeave) RTestStepAviPlayCtrlSetPosition(_L("MM-MMF-AVICTRL-U-0015-HP"), _L("SectionPlayer"), _L("video_only"), KAviPlayControllerUid));
	AddTestStepL( new(ELeave) RTestStepAviPlayCtrlGetVideoBitrate(_L("MM-MMF-AVICTRL-U-0016-HP"), _L("SectionPlayer"), _L("video_only"), KAviPlayControllerUid));
	AddTestStepL( new(ELeave) RTestStepAviPlayCtrlGetAudioBitrate(_L("MM-MMF-AVICTRL-U-0017-HP"), _L("SectionPlayer"), _L("video_only"), KAviPlayControllerUid));
	AddTestStepL( new(ELeave) RTestStepAviCtrlPlayInvalid(_L("MM-MMF-AVICTRL-U-0018-HP"), _L("SectionPlayer"), _L("invalid"), KAviPlayControllerUid));
	AddTestStepL( new(ELeave) RTestStepAviCtrlNotAudioEnabled(_L("MM-MMF-AVICTRL-U-0019-HP"), _L("SectionPlayer"), _L("video_only"), KAviPlayControllerUid));
	AddTestStepL( new(ELeave) RTestStepAviCtrlDirectScreenAccess(_L("MM-MMF-AVICTRL-U-0020-HP"), _L("SectionPlayer"), _L("video_only"), KAviPlayControllerUid));
	// TODO: MM-MMF-AVICTRL-U-0021-HP identifier is duplicated, should be fixed
	AddTestStepL( new(ELeave) RTestStepAviCtrlSetPlayWindow(_L("MM-MMF-AVICTRL-U-0021-HP"), _L("SectionPlayer"), _L("video_only"), KAviPlayControllerUid));
	// NOTE: More PlayController test steps follow at the bottom...


	//Steps to test RecordController
	AddTestStepL( new(ELeave) RTestStepAviRecordCtrlRecord(_L("MM-MMF-AVICTRL-U-0021-HP"), _L("SectionRecorder"), _L("record"), KAviRecordControllerUid));
	AddTestStepL( new(ELeave) RTestStepAviRecordCtrlGain(_L("MM-MMF-AVICTRL-U-0022-HP"), _L("SectionRecorder"), _L("record"), KAviRecordControllerUid));
	AddTestStepL( new(ELeave) RTestStepAviRecordCtrlRecordBalance(_L("MM-MMF-AVICTRL-U-0023-HP"), _L("SectionRecorder"), _L("record"), KAviRecordControllerUid));
	AddTestStepL( new(ELeave) RTestStepAviRecordCtrlSetMaxFileSize(_L("MM-MMF-AVICTRL-U-0024-HP"), _L("SectionRecorder"), _L("record"), KAviRecordControllerUid));
	AddTestStepL( new(ELeave) RTestStepAviRecordCtrlRecordTimeAvailable(_L("MM-MMF-AVICTRL-U-0025-HP"), _L("SectionRecorder"), _L("record"), KAviRecordControllerUid));
	AddTestStepL( new(ELeave) RTestStepAviRecordCtrlSetGetFrameRate(_L("MM-MMF-AVICTRL-U-0026-HP"), _L("SectionRecorder"), _L("record"), KAviRecordControllerUid));
	AddTestStepL( new(ELeave) RTestStepAviRecirdCtrlSetGetFrameSize(_L("MM-MMF-AVICTRL-U-0027-HP"), _L("SectionRecorder"), _L("record"), KAviRecordControllerUid));
	AddTestStepL( new(ELeave) RTestStepAviRecordCtrlGetSetAudioDataType(_L("MM-MMF-AVICTRL-U-0028-HP"), _L("SectionRecorder"), _L("record"), KAviRecordControllerUid));
	AddTestStepL( new(ELeave) RTestStepAviRecordCtrlGetSetVideoDataType(_L("MM-MMF-AVICTRL-U-0029-HP"), _L("SectionRecorder"), _L("record"), KAviRecordControllerUid));
	AddTestStepL( new(ELeave) RTestStepAviRecordCtrlGetSetMetaData(_L("MM-MMF-AVICTRL-U-0030-HP"), _L("SectionRecorder"), _L("record"), KAviRecordControllerUid));
	AddTestStepL( new(ELeave) RTestStepAviRecordCtrlDuration(_L("MM-MMF-AVICTRL-U-0031-HP"), _L("SectionRecorder"), _L("record"), KAviRecordControllerUid));
	AddTestStepL( new(ELeave) RTestStepAviRecordNotAudioEnabled(_L("MM-MMF-AVICTRL-U-0032-HP"), _L("SectionRecorder"), _L("record"), KAviRecordControllerUid));
	AddTestStepL( new(ELeave) RTestStepAviRecordCtrlPosition(_L("MM-MMF-AVICTRL-U-0033-HP"), _L("SectionRecorder"), _L("record"), KAviRecordControllerUid));
	AddTestStepL( new(ELeave) RTestStepAviRecordCtrlPause(_L("MM-MMF-AVICTRL-U-0034-HP"), _L("SectionRecorder"), _L("record"), KAviRecordControllerUid));
	AddTestStepL( new(ELeave) RTestStepAviRecordCtrlDurationNewClip(_L("MM-MMF-AVICTRL-U-0035-HP"), _L("SectionRecorder"), _L("record"), KAviRecordControllerUid));
	// CR0958
	AddTestStepL( new(ELeave) RTestStepAviRecordCtrlSetGetPixelAspectRatio(_L("MM-MMF-AVICTRL-U-0045-HP"), _L("SectionRecorder"), _L("record"), KAviRecordControllerUid));
	AddTestStepL( new(ELeave) RTestStepAviRecordCtrlGetSupportedPixelAspectRatios(_L("MM-MMF-AVICTRL-U-0046-HP"), _L("SectionRecorder"), _L("record"), KAviRecordControllerUid));
	AddTestStepL( new(ELeave) RTestStepAviRecordCtrlSetGetAudioSampleRate(_L("MM-MMF-AVICTRL-U-0047-HP"), _L("SectionRecorder"), _L("record"), KAviRecordControllerUid));
	AddTestStepL( new(ELeave) RTestStepAviRecordCtrlGetSupportedAudioSampleRate(_L("MM-MMF-AVICTRL-U-0048-HP"), _L("SectionRecorder"), _L("record"), KAviRecordControllerUid));
	AddTestStepL( new(ELeave) RTestStepAviRecordCtrlSetGetAudioChannels(_L("MM-MMF-AVICTRL-U-0049-HP"), _L("SectionRecorder"), _L("record"), KAviRecordControllerUid));
	AddTestStepL( new(ELeave) RTestStepAviRecordCtrlGetSupportedAudioChannels(_L("MM-MMF-AVICTRL-U-0050-HP"), _L("SectionRecorder"), _L("record"), KAviRecordControllerUid));
	
	// Steps to test RecordController: Negative Testing
	// CR958
	AddTestStepL( new(ELeave) RTestStepAviRecordCtrlSetInvalidPixelAspectRatio(_L("MM-MMF-AVICTRL-U-0051-HP"), _L("SectionRecorder"), _L("record"), KAviRecordControllerUid));
	AddTestStepL( new(ELeave) RTestStepAviRecordCtrlSetInvalidAudioSampleRate(_L("MM-MMF-AVICTRL-U-0052-HP"), _L("SectionRecorder"), _L("record"), KAviRecordControllerUid));
	AddTestStepL( new(ELeave) RTestStepAviRecordCtrlSetInvalidAudioChannels(_L("MM-MMF-AVICTRL-U-0053-HP"), _L("SectionRecorder"), _L("record"), KAviRecordControllerUid));
	AddTestStepL( new(ELeave) RTestStepAviRecordCtrlInvalidStateSetPixelAspectRatio(_L("MM-MMF-AVICTRL-U-0054-HP"), _L("SectionRecorder"), _L("record"), KAviRecordControllerUid));
	AddTestStepL( new(ELeave) RTestStepAviRecordCtrlInvalidStateSetAudioSampleRate(_L("MM-MMF-AVICTRL-U-0055-HP"), _L("SectionRecorder"), _L("record"), KAviRecordControllerUid));
	AddTestStepL( new(ELeave) RTestStepAviRecordCtrlInvalidStateSetAudioChannels(_L("MM-MMF-AVICTRL-U-0056-HP"), _L("SectionRecorder"), _L("record"), KAviRecordControllerUid));	
	
	//Steps to test setting Screen device on AviPlayController - CR0779
	//Play on primary screen
	AddTestStepL( new(ELeave) RTestStepAviPlayCtrlSetScrnPlay(_L("MM-MMF-AVICTRL-U-0041-HP"), _L("SectionPlayer"), _L("video_only"), KAviPlayControllerUid, KPrimaryScreenNo));
	//Play on secondary screen
	AddTestStepL( new(ELeave) RTestStepAviPlayCtrlSetScrnPlay(_L("MM-MMF-AVICTRL-U-0042-HP"), _L("SectionPlayer"), _L("video_only"), KAviPlayControllerUid, KSecondaryScreenNo));
	//try setting an invalid screen id
	AddTestStepL( new(ELeave) RTestStepAviPlayCtrlSetInvalidScrn(_L("MM-MMF-AVICTRL-U-0043-HP"), _L("SectionPlayer"), _L("video_only"), KAviPlayControllerUid, KInvalidScreenNo));
	//Add data sink twice
	AddTestStepL( new(ELeave) RTestStepAviPlayCtrlAddDataSinkTwice(_L("MM-MMF-AVICTRL-U-0044-HP"), _L("SectionPlayer"), _L("video_only"), KAviPlayControllerUid, KInvalidScreenNo));

	// More PlayController test steps here...
	AddTestStepL( new(ELeave) RTestStepAviPlayNotEnoughMemoryForXvidCodecHeap(_L("MM-MMF-AVICTRL-U-0057-HP"), _L("SectionPlayer"), _L("long_video"), KAviPlayControllerUid));
	// INC112128 : To test if AVI Files with Video Property Header are played correctly
	AddTestStepL( new(ELeave) RTestStepAviPlayCtrlPlay(_L("MM-MMF-AVICTRL-U-0058-HP"), _L("SectionPlayer"), _L("videoprophdr"), KAviPlayControllerUid));
	
	AddTestStepL( new(ELeave) RTestStepAviPlayCtrlMemAlloc1(_L("MM-MMF-AVICTRL-U-0070-HP"), _L("SectionPlayer"), _L("aud_vid"), KUidCustomTestAviPlayPlugin));
	AddTestStepL( new(ELeave) RTestStepAviPlayCtrlMemAlloc2(_L("MM-MMF-AVICTRL-U-0071-HP"), _L("SectionPlayer"), _L("aud_vid"), KUidCustomTestAviPlayPlugin));
	AddTestStepL( new(ELeave) RTestStepAviPlayCtrlMemAlloc3(_L("MM-MMF-AVICTRL-U-0072-HP"), _L("SectionPlayer"), _L("aud_vid"), KUidCustomTestAviPlayPlugin));

#ifdef SYMBIAN_BUILD_GCE
	// PlayController test steps for surface support here:
	AddTestStepL(new (ELeave) RTestStepAviPlayCtrlSurfaceSupport(_L("MM-MMF-AVICTRL-U-0059-HP"), _L("SectionPlayer"), _L("video_only"), KAviPlayControllerUid));
	AddTestStepL(new (ELeave) RTestStepAviPlayCtrlGetSurfaceParametersNoUS(_L("MM-MMF-AVICTRL-U-0064-HP"), _L("SectionPlayer"), _L("video_only"), KAviPlayControllerUid));
	AddTestStepL(new (ELeave) RTestStepAviPlayCtrlSurfaceRemovedNoUS(_L("MM-MMF-AVICTRL-U-0073-HP"), _L("SectionPlayer"), _L("video_only"), KAviPlayControllerUid));
	AddTestStepL(new (ELeave) RTestStepAviPlayCtrlSurfaceRemovedNullSurfaceId(_L("MM-MMF-AVICTRL-U-0074-HP"), _L("SectionPlayer"), _L("video_only"), KAviPlayControllerUid));
	AddTestStepL(new (ELeave) RTestStepAviPlayCtrlSurfaceRemovedInvalidSurfaceId(_L("MM-MMF-AVICTRL-U-0075-HP"), _L("SectionPlayer"), _L("video_only"), KAviPlayControllerUid));
#endif // SYMBIAN_BUILD_GCE
	
#ifdef SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT
	// AVI play controller test steps for subtitle support.
	AddTestStepL(new (ELeave) RAsyncTestSubtitlePlay(_L("MM-MMF-AVICTRL-U-0100-HP"), _L("SectionPlayer"), _L("srt_available"), KAviPlayControllerUid));
	AddTestStepL(new (ELeave) RAsyncTestSubtitleUpdateConfig(_L("MM-MMF-AVICTRL-U-0101-HP"), _L("SectionPlayer"), _L("srt_available"), KAviPlayControllerUid));
	AddTestStepL(new (ELeave) RAsyncTestSubtitleUpdateDuring(_L("MM-MMF-AVICTRL-U-0102-HP"), _L("SectionPlayer"), _L("srt_available"), KAviPlayControllerUid));
	AddTestStepL(new (ELeave) RAsyncTestSubtitleRemoveAdd(_L("MM-MMF-AVICTRL-U-0103-HP"), _L("SectionPlayer"), _L("srt_available"), KAviPlayControllerUid));
	AddTestStepL(new (ELeave) RAsyncTestSubtitleNotEnabled(_L("MM-MMF-AVICTRL-U-0200-HP"), _L("SectionPlayer"), _L("srt_available"), KAviPlayControllerUid));
	AddTestStepL(new (ELeave) RAsyncTestSubtitleNotAvailable(_L("MM-MMF-AVICTRL-U-0201-HP"), _L("SectionPlayer"), _L("srt_unavailable"), KAviPlayControllerUid));
	AddTestStepL(new (ELeave) RAsyncTestSubtitleMismatchedId(_L("MM-MMF-AVICTRL-U-0202-HP"), _L("SectionPlayer"), _L("srt_available"), KAviPlayControllerUid));
	AddTestStepL(new (ELeave) RAsyncTestSubtitleDoubleCall(_L("MM-MMF-AVICTRL-U-0203-HP"), _L("SectionPlayer"), _L("srt_available"), KAviPlayControllerUid));
	AddTestStepL(new (ELeave) RAsyncTestSubtitleMultipleConfigs(_L("MM-MMF-AVICTRL-U-0204-HP"), _L("SectionPlayer"), _L("srt_available"), KAviPlayControllerUid));
#endif //SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT
	}
	
/**
Initialises the system path. 
*/
void CTestSuiteAviCtrl::InitSystemPath()
	{
	_LIT(KTestSourcePathOnC,"c:\\mm\\mmf\\testfiles\\avictrl\\");
	RFs fs;
	TUint att;
	TInt ret = fs.Connect();
	__ASSERT_ALWAYS(ret == KErrNone,User::Panic(_L("Sys path not setup"),ret));
	if (fs.Att(KTestSourcePathOnC, att) == KErrNone)
		{
		iDefaultPath = KTestSourcePathOnC;
		}
	else
		{
		User::Panic(_L("Test files not found"),KErrNotFound);	
		}
	fs.Close();
	}

const TDesC& CTestSuiteAviCtrl::DefaultPath() const
	{
	return iDefaultPath;
	}
	

/**
 * Creates the dependency file for test camera
 */
void CTestSuiteAviCtrl::CreateDependencyFileL()
	{
	// enforce dependency
	// create the file the plugin depends on.
	User::LeaveIfError(ifsSession.Connect());

	ifsSession.Delete(KMMCameraPluginName);

	RFile file;
	TInt err = file.Replace(ifsSession, KTestCameraPluginName, EFileShareAny);
  	file.Close();
  	if(err != KErrNone)
  		{
	    	User::LeaveIfError(KErrNotSupported);
  		}
	}
	
/**
 * Removes the dependency file for test camera
 */
void CTestSuiteAviCtrl::RemoveDependencyFile()
	{
	ifsSession.Delete(KTestCameraPluginName);
	ifsSession.Close();
	}
	
	

