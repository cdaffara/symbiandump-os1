// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This main DLL entry point for the tsi_mmf_vclntavi.dll
// Source file: Test Suite for Video Client tests
// 
//


// EPOC includes
#include <e32base.h>

// Test system includes
#include <testframework.h>

#include "tsi_mmf_vclntavi_stepbase.h"
#include "tsi_mmf_vclntavi_suite.h"
#include "testvideoplayer.h"
#ifdef SYMBIAN_BUILD_GCE
#include "testvideoplayer2.h"
#include "teststepvclnt2crp.h"
#endif // SYMBIAN_BUILD_GCE
#include "testvideorecorder.h"
#include "testvideoplayer_apr.h"
#include "testvideoplayer_secdisp.h"
#include "testvideorecorder_par_audio.h"

#ifdef SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT
#include "testvideoplayer_subtitles.h"
#endif //SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT

#ifndef EKA2
/**
 * Required for all DLLs but not used
 */
GLDEF_C TInt E32Dll(enum TDllReason)
    {
    return 0;
    }                        
#endif


/**
 * NewTestSuite is exported at ordinal 1
 * this provides the interface to allow schedule test
 * to create instances of this test suite
 */
EXPORT_C CTestSuite* NewTestSuiteL()
    {
    CTestMmfVclntAviSuite* result = new (ELeave) CTestMmfVclntAviSuite;
    CleanupStack::PushL(result);
    result->ConstructL();
    CleanupStack::Pop(); // result
    return result;
    }

/**
 * destructor
 */
CTestMmfVclntAviSuite::~CTestMmfVclntAviSuite()
    {
    RemoveDependencyFile();
    }

/**
 * CTestMmfVclntAviSuite::GetVersion()
 * make a version string available for test system
 */
TPtrC CTestMmfVclntAviSuite::GetVersion()
    {
    _LIT(KTxtVersion,"1.0");
    return KTxtVersion();
    }

/**
 * CTestMmfVclntAviSuite::AddTestStepL()
 * Add a test step into the suite
 */
void CTestMmfVclntAviSuite::AddTestStepL( RTestMmfVclntAviStep* aPtrTestStep )
    {
    // test steps contain a pointer back to the suite which owns them
    aPtrTestStep->SetTestSuite(this);
    // add the step using tyhe base class method
    CTestSuite::AddTestStepL(aPtrTestStep);
    }

/**
 * Constructor for test suite
 * this creates all the test steps and 
 * stores them inside CTestMmfVclntAviSuite 
 */
void CTestMmfVclntAviSuite::InitialiseL()
    {
#ifdef __WINS__
    // Check that the window server is ready for us
    // This is so that the call to CDirectScreenAccess::StartL does not leave with KErrNotReady
    if (!WaitUntilWindowServerReadyL())
        {
        INFO_PRINTF1(_L("TestSuite Error : ERROR Receiving Window Server Redraw event"));
        return;
        }
#endif // __WINS__
    
    // Create the dependancy files.
    CreateDependencyFileL();
    
    // store the name of this test suite 
    iSuiteName = _L("TSI_MMF_VCLNTAVI");

    TTimeIntervalMicroSeconds position = TInt64(0);
    TFourCC KInvalidAudioType(0xAA, 0xAA, 0xAA, 0xAA);

    //
    // POSITIVE TEST //
    //

    //************************************* Video Player Tests **********************************
    AddTestStepL( RTestVclntPlayAviFile::NewL( _L("MM-MMF-VCLNTAVI-I-0001-CP"), _L("SectionOne"), _L("filename"), KErrNone, ETrue) );
    AddTestStepL( RTestVclntPlayAviDes::NewL( _L("MM-MMF-VCLNTAVI-I-0002-CP"), _L("SectionThree"), _L("filename"), KErrNotSupported, ETrue) );
    AddTestStepL( RTestVclntPlayAviUrl::NewL( _L("MM-MMF-VCLNTAVI-I-0003-CP"), _L("SectionOne"), _L("filename"), KErrNotSupported, ETrue) );
    AddTestStepL( RTestVclntEnqFrameRate::NewL( _L("MM-MMF-VCLNTAVI-I-0004-HP"), _L("SectionThree"), _L("filename"), KErrNone, 10) );
    AddTestStepL( RTestVclntPosition::NewL( _L("MM-MMF-VCLNTAVI-I-0005-HP"), _L("SectionThree"), _L("filename"), KErrNone, position)  ); 
    AddTestStepL( RTestVclntPriority::NewL( _L("MM-MMF-VCLNTAVI-I-0006-HP"), _L("SectionOne"), _L("filename"), KErrNone, -1/*EPriorityMin*/) );
    AddTestStepL( RTestVclntDuration::NewL( _L("MM-MMF-VCLNTAVI-I-0007-HP"), _L("SectionThree"), _L("filename"), KErrNone, position) );
    AddTestStepL( RTestVclntVolume::NewL( _L("MM-MMF-VCLNTAVI-I-0008-HP"), _L("SectionOne"), _L("filename"), KErrNone, -1) );
    AddTestStepL( RTestVclntCloseOpen::NewL( _L("MM-MMF-VCLNTAVI-I-0009-HP"), _L("SectionOne"), _L("filename"), KErrNone) );
    AddTestStepL( RTestVclntPause::NewL( _L("MM-MMF-VCLNTAVI-I-0010-HP"), _L("SectionThree"), _L("filename"), KErrNotSupported) );
    AddTestStepL( RTestVclntBalance::NewL( _L("MM-MMF-VCLNTAVI-I-0011-HP"), _L("SectionOne"), _L("filename"), KErrNone,-10) );
    AddTestStepL( RTestVclntPlayWindow::NewL( _L("MM-MMF-VCLNTAVI-I-0012-HP"), _L("SectionOne"), _L("filename"), KErrNone, position , position) );
    AddTestStepL( RTestVclntMeta::NewL( _L("MM-MMF-VCLNTAVI-I-0013-HP"), _L("SectionOne"), _L("filename"), KErrNotSupported) );
    AddTestStepL( RTestVclntFrameSize::NewL( _L("MM-MMF-VCLNTAVI-I-0014-HP"), _L("SectionOne"), _L("filename"), KErrNone) );
    AddTestStepL( RTestVclntRepeat::NewL( _L("MM-MMF-VCLNTAVI-I-0015-HP"), _L("SectionThree"), _L("filename"), KErrNotSupported, 1) );
    AddTestStepL( RTestVclntMimeType::NewL( _L("MM-MMF-VCLNTAVI-I-0016-HP"), _L("SectionOne"), _L("filename"), KErrNone) );
    AddTestStepL( RTestVclntScale::NewL( _L("MM-MMF-VCLNTAVI-I-0017-HP"), _L("SectionOne"), _L("filename"), KErrNone) );
    AddTestStepL( RTestVclntCrop::NewL( _L("MM-MMF-VCLNTAVI-I-0018-HP"), _L("SectionOne"), _L("filename"), KErrNone) );
    AddTestStepL( RTestVclntGetFrame::NewL( _L("MM-MMF-VCLNTAVI-I-0019-HP"), _L("SectionOne"), _L("filename"), KErrNone, EFalse ) );
    AddTestStepL( RTestVclntGetFrame::NewL( _L("MM-MMF-VCLNTAVI-I-0020-HP"), _L("SectionOne"), _L("filename"), KErrNone, ETrue ) );
    AddTestStepL( RTestVclntRebuffering::NewL( _L("MM-MMF-VCLNTAVI-I-0021-HP"), _L("SectionOne"), _L("filename"), KErrNone) );
    //******************************** Video Player RFile Tests ********************************

    AddTestStepL( RTestVclntPlayAviFileHandle::NewL( _L("MM-MMF-VCLNTAVI-I-0022-HP"), _L("SectionOne"), _L("filename"), KErrNone, EFalse ) );
    AddTestStepL( RTestVclntPlayAviFileHandle::NewL( _L("MM-MMF-VCLNTAVI-I-0023-HP"), _L("SectionOne"), _L("filename"), KErrNone, ETrue ) );
    //********************************Test added as part of CR0912******************************
    AddTestStepL( RTestVideoPlayCapabilityVelocity::NewL( _L("MM-MMF-VCLNTAVI-I-0030-HP"), _L("SectionOne"), _L("filename"), KErrNotReady) );
    AddTestStepL( RTestVideoPlayCapabilityVelocity::NewL( _L("MM-MMF-VCLNTAVI-I-0031-HP"), _L("SectionOne"), _L("dummyFile"), KErrNotReady) );    
    AddTestStepL( RTestVideoPlayStepFrame::NewL( _L("MM-MMF-VCLNTAVI-I-0032-HP"), _L("SectionOne"), _L("filename"), KErrNotReady) );
    AddTestStepL( RTestVideoPlayStepFrame::NewL( _L("MM-MMF-VCLNTAVI-I-0033-HP"), _L("SectionOne"), _L("dummyFile"), KErrNotReady) );    
    AddTestStepL( RTestVideoPlayAudVidEnable::NewL( _L("MM-MMF-VCLNTAVI-I-0034-HP"), _L("SectionOne"), _L("filename"), KErrNotReady) );
    AddTestStepL( RTestVideoPlayAudVidEnable::NewL( _L("MM-MMF-VCLNTAVI-I-0035-HP"), _L("SectionOne"), _L("dummyFile"), KErrNotReady) );    
    AddTestStepL( RTestVideoPlayAutoScale::NewL( _L("MM-MMF-VCLNTAVI-I-0036-HP"), _L("SectionOne"), _L("filename"), KErrNotReady) );
    AddTestStepL( RTestVideoPlayAutoScale::NewL( _L("MM-MMF-VCLNTAVI-I-0037-HP"), _L("SectionOne"), _L("dummyFile"), KErrNotReady) );

    //************************************ Video Recorder Tests *********************************
    AddTestStepL( RTestVclntRecordAviFile::NewL( _L("MM-MMF-VCLNTAVI-I-0101-CP"), _L("SectionTwo"), _L("filename"), KErrNone, ETrue) );
    AddTestStepL( RTestVclntRecordAviDes::NewL( _L("MM-MMF-VCLNTAVI-I-0102-CP"), _L("SectionTwo"), _L("filename"), KErrNotSupported, ETrue) );
    AddTestStepL( RTestVclntRecordAviUrl::NewL( _L("MM-MMF-VCLNTAVI-I-0103-CP"), _L("SectionTwo"), _L("filename"), KErrNotSupported, ETrue) );
    AddTestStepL( RTestVclntRecordCloseOpen::NewL( _L("MM-MMF-VCLNTAVI-I-0104-HP"), _L("SectionTwo"), _L("filename"), KErrNone) );
    AddTestStepL( RTestVclntRecordConfig::NewL( _L("MM-MMF-VCLNTAVI-I-0105-HP"), _L("SectionTwo"), _L("filename"), KErrNone) );

    AddTestStepL( RTestVclntRecordPause::NewL( _L("MM-MMF-VCLNTAVI-I-0106-HP"), _L("SectionTwo"), _L("filename"), KErrNone) );
    AddTestStepL( RTestVclntRecordDuration::NewL( _L("MM-MMF-VCLNTAVI-I-0107-HP"), _L("SectionTwo"), _L("filename"), KErrNone, position) );
    AddTestStepL( RTestVclntRecordTime::NewL( _L("MM-MMF-VCLNTAVI-I-0108-HP"), _L("SectionTwo"), _L("filename"), KErrNone, 10) ); 
    AddTestStepL( RTestVclntRecordGain::NewL( _L("MM-MMF-VCLNTAVI-I-0109-HP"), _L("SectionTwo"), _L("filename"), KErrNone, 10) );
    AddTestStepL( RTestVclntRecordMeta::NewL( _L("MM-MMF-VCLNTAVI-I-0110-HP"), _L("SectionTwo"), _L("filename"), KErrNotSupported) );
    AddTestStepL( RTestVclntRecordPriority::NewL( _L("MM-MMF-VCLNTAVI-I-0111-HP"), _L("SectionTwo"), _L("filename"), KErrNone, -1/*EPriorityMin*/) );
    AddTestStepL( RTestVclntRecordSize::NewL( _L("MM-MMF-VCLNTAVI-I-0112-HP"), _L("SectionTwo"), _L("filename"), KErrNone) );
    AddTestStepL( RTestVclntRecordTypes::NewL( _L("MM-MMF-VCLNTAVI-I-0113-HP"), _L("SectionTwo"), _L("filename"), KErrNone) );

    //********************************** Video Recorder RFile Tests *******************************

    AddTestStepL( RTestVclntRecordAviRFile::NewL( _L("MM-MMF-VCLNTAVI-I-0114-HP"), _L("SectionTwo"), _L("filename"), KErrNone) );
    AddTestStepL( RTestVclntRecordAviRFileAllParams::NewL( _L("MM-MMF-VCLNTAVI-I-0115-HP"), _L("SectionTwo"), _L("filename"), KErrNone, EFalse, KMmfTestAviRecordControllerUid, KUidMdaBmpClipFormat, KMMFFourCCCodeNULL) );
    AddTestStepL( RTestVclntRecordAviRFileAllParams::NewL( _L("MM-MMF-VCLNTAVI-I-0116-HP"), _L("SectionTwo"), _L("filename"), KErrNone, ETrue, KMmfTestAviRecordControllerUid, KUidMdaBmpClipFormat, KMMFFourCCCodeNULL) );

    //********************************** Added as part of CR0912***********************************
    AddTestStepL( CTestVclntAviRecordSetVideoEnabled::NewL( _L("MM-MMF-VCLNTAVI-I-0125-HP"), _L("SectionTwo"), _L("filename"), KErrNone) );
    AddTestStepL( CTestVclntAviRecordSetVideoEnabled::NewL( _L("MM-MMF-VCLNTAVI-I-0126-HP"), _L("SectionTwo"), _L("dummyFile"), KErrNone) );    
    AddTestStepL( CTestVclntAviRecordSetVideoQuality::NewL( _L("MM-MMF-VCLNTAVI-I-0127-HP"), _L("SectionTwo"), _L("filename"), KErrNone) );
    AddTestStepL( CTestVclntAviRecordSetVideoQuality::NewL( _L("MM-MMF-VCLNTAVI-I-0128-HP"), _L("SectionTwo"), _L("dummyFile"), KErrNone) );    
    AddTestStepL( CTestVclntAviRecordSetVideoFrameRateFixed::NewL( _L("MM-MMF-VCLNTAVI-I-0129-HP"), _L("SectionTwo"), _L("filename"), KErrNone) );
    AddTestStepL( CTestVclntAviRecordSetVideoFrameRateFixed::NewL( _L("MM-MMF-VCLNTAVI-I-0130-HP"), _L("SectionTwo"), _L("dummyFile"), KErrNone) );    
    
    //
    // NEGATIVE TEST //
    //
    
    //************************************* Video Player Tests **********************************
    AddTestStepL( RTestVclntDelete::NewL( _L("MM-MMF-VCLNTAVI-I-1001-HP"), _L("SectionOne"), _L("filename"), KErrNone) );
    AddTestStepL( RTestVclntFrameSize::NewL( _L("MM-MMF-VCLNTAVI-I-1002-HP"), _L("SectionThree"), _L("filename"), KErrNone) );
    position = 103423423;
    AddTestStepL( RTestVclntPosition::NewL( _L("MM-MMF-VCLNTAVI-I-1003-HP"), _L("SectionThree"), _L("filename"), KErrNone, position) ); 
    position = -12;
    AddTestStepL( RTestVclntPosition::NewL( _L("MM-MMF-VCLNTAVI-I-1004-HP"), _L("SectionThree"), _L("filename"), KErrNone, position) ); 
    AddTestStepL( RTestVclntVolume::NewL( _L("MM-MMF-VCLNTAVI-I-1005-HP"), _L("SectionOne"), _L("filename"), KErrNone, 1000400) );
    AddTestStepL( RTestVclntVolume::NewL( _L("MM-MMF-VCLNTAVI-I-1006-HP"), _L("SectionOne"), _L("filename"), KErrNone, -100) );
    AddTestStepL( RTestVclntBalance::NewL( _L("MM-MMF-VCLNTAVI-I-1007-HP"), _L("SectionOne"), _L("filename"), KErrNone,200) );
    AddTestStepL( RTestVclntBalance::NewL( _L("MM-MMF-VCLNTAVI-I-1008-HP"), _L("SectionOne"), _L("filename"), KErrNone,-200) );
    position = 2000000;
    AddTestStepL( RTestVclntPlayWindow::NewL( _L("MM-MMF-VCLNTAVI-I-1009-HP"), _L("SectionOne"), _L("filename"), KErrNone, position , position) );
    AddTestStepL( RTestVclntPlayAviFile::NewL( _L("MM-MMF-VCLNTAVI-I-1010-HP"), _L("SectionOne"), _L("corruptFile"), KErrCorrupt, ETrue) );
    AddTestStepL( RTestVclntPlayAviFile::NewL( _L("MM-MMF-VCLNTAVI-I-1011-HP"), _L("SectionOne"), _L("NoCodecFile"), KErrNotSupported, ETrue) );
    //******************************* Video Player RFile Tests *********************************
#ifdef SYMBIAN_SECURITY_CAF_RFILE_HANDLE
    AddTestStepL( RTestVclntPlayAviFileHandle::NewL( _L("MM-MMF-VCLNTAVI-I-1012-HP"), _L("SectionOne"), _L("textFile"), KErrNotSupported, EFalse) );
    AddTestStepL( RTestVclntPlayAviFileHandle::NewL( _L("MM-MMF-VCLNTAVI-I-1013-HP"), _L("SectionOne"), _L("filename"), KErrNotFound, EFalse ) );
#endif
    
    //************************************ Video Recorder Tests **********************************
    AddTestStepL( RTestVclntRecordDelete::NewL( _L("MM-MMF-VCLNTAVI-I-1101-HP"), _L("SectionTwo"), _L("unsupported"), KErrNone) );
    AddTestStepL( RTestVclntRecordGain::NewL( _L("MM-MMF-VCLNTAVI-I-1102-HP"), _L("SectionTwo"), _L("filename"), KErrArgument, 1005670) );
    AddTestStepL( RTestVclntRecordGain::NewL( _L("MM-MMF-VCLNTAVI-I-1103-HP"), _L("SectionTwo"), _L("filename"), KErrArgument, -10) );
    AddTestStepL( RTestVclntRecordSize::NewL( _L("MM-MMF-VCLNTAVI-I-1104-HP"), _L("SectionTwo"), _L("filename"), KErrArgument) );
    //********************************** Video Recorder RFile Tests *******************************
#ifdef SYMBIAN_SECURITY_CAF_RFILE_HANDLE
    AddTestStepL( RTestVclntRecordAviRFileAllParams::NewL( _L("MM-MMF-VCLNTAVI-I-1105-HP"), _L("SectionTwo"), _L("filename"), KErrNone, EFalse, KVideoInvalidControllerUid, KUidMdaBmpClipFormat, KMMFFourCCCodeNULL ) );
    AddTestStepL( RTestVclntRecordAviRFileAllParams::NewL( _L("MM-MMF-VCLNTAVI-I-1106-HP"), _L("SectionTwo"), _L("filename"), KErrNone, EFalse, KMmfTestAviRecordControllerUid, KUidMdaBmpClipFormat, KInvalidAudioType ) );
    AddTestStepL( RTestVclntRecordAviRFileAllParams::NewL( _L("MM-MMF-VCLNTAVI-I-1107-HP"), _L("SectionTwo"), _L("textFile"), KErrNone, EFalse, KMmfTestAviRecordControllerUid, KUidMdaBmpClipFormat, KMMFFourCCCodeNULL ) );
#endif
	
	//
    // APR - POSITIVE TEST //
    //	

	AddTestStepL( RTestVclntAprRegOnce4SameEvent::NewL(_L("MM-MMF-VCLNTAVIAPR-I-0001-HP"),_L("SectionAPR") ) );
	AddTestStepL( RTestVclntAprRegB4Open::NewL(_L("MM-MMF-VCLNTAVIAPR-I-0002-HP"),_L("SectionAPR") ) );
	AddTestStepL( RTestVclntAprRegAftrOpenAndB4Play::NewL(_L("MM-MMF-VCLNTAVIAPR-I-0003-HP"),_L("SectionAPR") ) );
	AddTestStepL( RTestVclntAprRegAftrHpClntCompltPlay::NewL(_L("MM-MMF-VCLNTAVIAPR-I-0004-HP"),_L("SectionAPR") ) ); 
	AddTestStepL( RTestVclntAprRegSamePriorityPlayers::NewL(_L("MM-MMF-VCLNTAVIAPR-I-0005-HP"),_L("SectionAPR") ) );
	AddTestStepL( RTestVclntAprRegAtPlayAndCancelNfn::NewL(_L("MM-MMF-VCLNTAVIAPR-I-0006-HP"),_L("SectionAPR") ) );
	AddTestStepL( RTestVclntAprTestWillResumePlay::NewL(_L("MM-MMF-VCLNTAVIAPR-I-0007-HP"),_L("SectionAPR") ) );
	
	//
    // APR - ALLOC TEST //
    //	
	AddTestStepL( RTestVclntAprAllocUsingSharedHeap::NewL(_L("MM-MMF-VCLNTAVIAPR-I-0008-HP"),_L("SectionAPR") ) );
	
	//
    // APR - NEGATIVE TEST //
    //
    
    AddTestStepL( RTestVclntAprMultRegAtPlay::NewL(_L("MM-MMF-VCLNTAVIAPR-I-0101-HP"),_L("SectionAPR") ) );
    AddTestStepL( RTestVclntAprCancelNoResourceNfn::NewL(_L("MM-MMF-VCLNTAVIAPR-I-0102-HP"),_L("SectionAPR") ) );
    AddTestStepL( RTestVclntAprRegUnsupportedEvent::NewL(_L("MM-MMF-VCLNTAVIAPR-I-0103-HP"),_L("SectionAPR") ) );
    AddTestStepL( RTestVclntAprCancelUnsupportedEvent::NewL(_L("MM-MMF-VCLNTAVIAPR-I-0104-HP"),_L("SectionAPR") ) ); 
	
    //added for CR0779
    // Positive Tests
    AddTestStepL( RTestVclntSetScrIdAndPlayAviFile::NewL( _L("MM-MMF-VCLNTAVI-I-0201-CP"), _L("SectionOne"), _L("filename"), KErrNone, 1) );
    AddTestStepL( RTestVclntSetScrIdAndPlayAviDes::NewL( _L("MM-MMF-VCLNTAVI-I-0202-CP"), _L("SectionOne"), _L("filename"), KErrNotSupported, 1) );
    AddTestStepL( RTestVclntSetScrIdAndPlayAviUrl::NewL( _L("MM-MMF-VCLNTAVI-I-0203-CP"), _L("SectionOne"), _L("filename"), KErrNotSupported, 1) );
    AddTestStepL( RTestVclntSetScrIdAndPlayStopAviFile::NewL( _L("MM-MMF-VCLNTAVI-I-0204-CP"), _L("SectionOne"), _L("filename"), KErrNone, 1, 2) );
    AddTestStepL( RTestVclntSetScrIdAndPlayPauseAviFile::NewL( _L("MM-MMF-VCLNTAVI-I-0205-CP"), _L("SectionOne"), _L("filename"), KErrNone, 1, 2) );
    AddTestStepL( RTestVclntSetScrIdAndPlayStopAviFile::NewL( _L("MM-MMF-VCLNTAVI-I-0206-CP"), _L("SectionFour"), _L("filename"), KErrNone, 0, 1) );
    AddTestStepL( RTestVclntSetScrIdAndPlayPauseAviFile::NewL( _L("MM-MMF-VCLNTAVI-I-0207-CP"), _L("SectionFour"), _L("filename"), KErrNone, 0, 1) );
    AddTestStepL( RTestVclntSetScrIdAfterPlayAndPlayAviFile::NewL( _L("MM-MMF-VCLNTAVI-I-0208-CP"), _L("SectionOne"), _L("filename"), KErrNone, 1) );
    AddTestStepL( RTestVclntSetScrIdAfterOpenAndPlayAviFile::NewL( _L("MM-MMF-VCLNTAVI-I-0209-CP"), _L("SectionOne"), _L("filename"), KErrNone, 1) );
    AddTestStepL( RTestVclntSetScrIdDuringPlayAndPlayAviFile::NewL( _L("MM-MMF-VCLNTAVI-I-0210-CP"), _L("SectionOne"), _L("filename"), KErrNone, 1) );

    // Negative Tests
    AddTestStepL( RTestVclntSetScrIdAndPlayAviFile::NewL( _L("MM-MMF-VCLNTAVI-I-0251-CP"), _L("SectionOne"), _L("filename"), KErrNotSupported, 3) );
        
        
    //
    // CR0958 //
    // POSITIVE  TEST //
    //
    
    AddTestStepL( RTestVclntSetGetPARRec::NewL( _L("MM-MMF-VCLNTAVI-I-0300-HP"), _L("SectionOne"), _L("filename"), KErrNone) );
    AddTestStepL( RTestVclntGetSuppPAR::NewL( _L("MM-MMF-VCLNTAVI-I-0301-HP"), _L("SectionOne"), _L("filename"), KErrNone) );
    AddTestStepL( RTestVclntSetGetPARAftOpenPrepThenRec::NewL( _L("MM-MMF-VCLNTAVI-I-0302-HP"), _L("SectionOne"), _L("filename"), KErrNone) );
    AddTestStepL( RTestVclntSetPARAftOpenWoutSetVidAudCodec::NewL( _L("MM-MMF-VCLNTAVI-I-0303-HP"), _L("SectionOne"), _L("filename"), KErrNone) );
    AddTestStepL( RTestVclntSetGetAudChanRec::NewL( _L("MM-MMF-VCLNTAVI-I-0304-HP"), _L("SectionOne"), _L("filename"), KErrNone) );
    AddTestStepL( RTestVclntGetSuppAudChan::NewL( _L("MM-MMF-VCLNTAVI-I-0305-HP"), _L("SectionOne"), _L("filename"), KErrNone) );
    AddTestStepL( RTestVclntSetGetAudChanAftOpenPrepThenRec::NewL( _L("MM-MMF-VCLNTAVI-I-0306-HP"), _L("SectionOne"), _L("filename"), KErrNone) );
    AddTestStepL( RTestVclntGetSuppAudChanSetGetAudChanBefAudCodRec::NewL( _L("MM-MMF-VCLNTAVI-I-0307-HP"), _L("SectionOne"), _L("filename"), KErrNone) );
    AddTestStepL( RTestVclntSetGetAudSRRec::NewL( _L("MM-MMF-VCLNTAVI-I-0308-HP"), _L("SectionOne"), _L("filename"), KErrNone) );
    AddTestStepL( RTestVclntGetSuppAudSR::NewL( _L("MM-MMF-VCLNTAVI-I-0309-HP"), _L("SectionOne"), _L("filename"), KErrNone) );
    AddTestStepL( RTestVclntSetGetSRAftOpenPrepRec::NewL( _L("MM-MMF-VCLNTAVI-I-0310-HP"), _L("SectionOne"), _L("filename"), KErrNone) );
    AddTestStepL( RTestVclntGetSuppSRSetGetSRBefAudCodRec::NewL( _L("MM-MMF-VCLNTAVI-I-0311-HP"), _L("SectionOne"), _L("filename"), KErrNone) );
    AddTestStepL( RTestVclntGetDefPARAudSRAudChanAftOpenBefPrep::NewL( _L("MM-MMF-VCLNTAVI-I-0312-HP"), _L("SectionOne"), _L("filename"), KErrNone) );
    AddTestStepL( RTestVclntSetGetPARAudVidBRAudGain::NewL( _L("MM-MMF-VCLNTAVI-I-0313-HP"), _L("SectionOne"), _L("filename"), KErrNone) );
    AddTestStepL( RTestVclntSetGetAudSRAudChanWithAudDisable::NewL( _L("MM-MMF-VCLNTAVI-I-0314-HP"), _L("SectionOne"), _L("filename"), KErrNone) );

	//
    // CR0958 //
    // NEGATIVE TEST //
    //
    
    AddTestStepL( RTestVclntSetGetPARRec::NewL( _L("MM-MMF-VCLNTAVI-I-0315-HP"), _L("SectionOne"), _L("filename"), KErrNotSupported, 16, 9) );
    AddTestStepL( RTestVclntGetSuppPARSetGetPARBefOpenThenOpenRec::NewL( _L("MM-MMF-VCLNTAVI-I-0316-HP"), _L("SectionOne"), _L("filename"), KErrNotReady) );
    AddTestStepL( RTestVclntSetGetValidPARWhileRec::NewL( _L("MM-MMF-VCLNTAVI-I-0317-HP"), _L("SectionOne"), _L("filename"), KErrNotReady) );
    AddTestStepL( RTestVclntSetGetAudChanRec::NewL( _L("MM-MMF-VCLNTAVI-I-0318-HP"), _L("SectionOne"), _L("filename"), KErrNotSupported,-1) );
    AddTestStepL( RTestVclntGetSuppChanSetGetAudChanBefOpenThenOpenRec::NewL( _L("MM-MMF-VCLNTAVI-I-0319-HP"), _L("SectionOne"), _L("filename"), KErrNotReady) );
    AddTestStepL( RTestVclntSetGetValidAudChanWhileRec::NewL( _L("MM-MMF-VCLNTAVI-I-0320-HP"), _L("SectionOne"), _L("filename"), KErrNotReady) );
    AddTestStepL( RTestVclntSetGetAudSRRec::NewL( _L("MM-MMF-VCLNTAVI-I-0321-HP"), _L("SectionOne"), _L("filename"), KErrNotSupported, 44100) );
    AddTestStepL( RTestVclntGetSuppSRSetGetSRBefOpenThenOpenRec::NewL( _L("MM-MMF-VCLNTAVI-I-0322-HP"), _L("SectionOne"), _L("filename"), KErrNotReady) );
    AddTestStepL( RTestVclntSetGetValidSRWhileRec::NewL( _L("MM-MMF-VCLNTAVI-I-0323-HP"), _L("SectionOne"), _L("filename"), KErrNotReady) );
    
#ifdef SYMBIAN_BUILD_GCE     
	//
    // PREQ1714 - Video Rendering to a Graphics Surface //
    // POSITIVE TEST //
    //

	AddTestStepL( RTestVclntAutoScale::NewL( _L("MM-MMF-VCLNTAVI-I-0433")));
    AddTestStepL( RTestVclnt2PlayFile::NewL( _L("MM-MMF-VCLNTAVI-I-0400"), _L("SectionOne"), _L("filename"), KErrNone) );        
    AddTestStepL( RTestVclnt2AddWin::NewL( _L("MM-MMF-VCLNTAVI-I-0401"), _L("SectionOne"), _L("filename"), KErrNone) );    
    
    // use long clips for these tests
    AddTestStepL( RTestVclnt2RemoveWin::NewL( _L("MM-MMF-VCLNTAVI-I-0402"), _L("SectionFive"), _L("filename"), KErrNone, ETrue) );    
    AddTestStepL( RTestVclnt2WinPos::NewL( _L("MM-MMF-VCLNTAVI-I-0403"), _L("SectionFive"), _L("filename"), KErrNone) );    
    AddTestStepL( RTestVclnt2VideoExt::NewL( _L("MM-MMF-VCLNTAVI-I-0404"), _L("SectionFive"), _L("filename"), KErrNone) );       
    AddTestStepL( RTestVclnt2ClipRect::NewL( _L("MM-MMF-VCLNTAVI-I-0405"), _L("SectionFive"), _L("filename"), KErrNone) );           
    AddTestStepL( RTestVclnt2ContOffset::NewL( _L("MM-MMF-VCLNTAVI-I-0406"), _L("SectionFive"), _L("filename"), KErrNone));
    AddTestStepL( RTestVclnt2ContOffsetAtPlay::NewL( _L("MM-MMF-VCLNTAVI-I-0407"), _L("SectionFive"), _L("filename"), KErrNone));
    AddTestStepL( RTestVclnt2Align::NewL( _L("MM-MMF-VCLNTAVI-I-0408"), _L("SectionFive"), _L("filename"), KErrNone));
    AddTestStepL( RTestVclnt2ContOffset::NewL( _L("MM-MMF-VCLNTAVI-I-0409"), _L("SectionFive"), _L("filename"), KErrNone));
    AddTestStepL( RTestVclnt2AutoScale::NewL( _L("MM-MMF-VCLNTAVI-I-0410"), _L("SectionFive"), _L("filename"), KErrNone));
    AddTestStepL( RTestVclnt2Scale::NewL( _L("MM-MMF-VCLNTAVI-I-0411"), _L("SectionFive"), _L("filename"), KErrNotSupported) );           

    // Ensure the size TSize(10, 10) entered is valid relative to the video clip to be used       
    AddTestStepL( RTestVclnt2Viewport::NewL( _L("MM-MMF-VCLNTAVI-I-0412"), _L("SectionOne"), _L("filename"), TSize(10, 10)) );
    
    AddTestStepL( RTestVclnt2Viewport::NewL( _L("MM-MMF-VCLNTAVI-I-0413"), _L("SectionOne"), _L("filename"), TSize(-1, 1)) );
    AddTestStepL( RTestVclnt2Viewport::NewL( _L("MM-MMF-VCLNTAVI-I-0414"), _L("SectionOne"), _L("filename"), TSize(1, -1)) );
    AddTestStepL( RTestVclnt2Viewport::NewL( _L("MM-MMF-VCLNTAVI-I-0415"), _L("SectionOne"), _L("filename"), TSize(1, 1)) );
    AddTestStepL( RTestVclnt2Rotation::NewL( _L("MM-MMF-VCLNTAVI-I-0416"), _L("SectionOne"), _L("filename"), KErrNotSupported) );    
    
    AddTestStepL( RTestVclnt2Settings::NewL( _L("MM-MMF-VCLNTAVI-I-0417"), _L("SectionOne"), _L("filename")) );
    AddTestStepL( RTestVclnt2SettingsAndAlignments::NewL( _L("MM-MMF-VCLNTAVI-I-0418"), _L("SectionOne"), _L("filename")) );    
    AddTestStepL( RTestVclnt2SettingsAndAlignments::NewL( _L("MM-MMF-VCLNTAVI-I-0419"), _L("SectionOne"), _L("filename")) );    
    AddTestStepL( RTestVclnt2SecDisplay::NewL( _L("MM-MMF-VCLNTAVI-I-0420"), _L("SectionOne"), _L("filename")) );    

    // use long clip for this test
    AddTestStepL( RTestVclnt2CropRectPause::NewL( _L("MM-MMF-VCLNTAVI-I-0421"), _L("SectionFive"), _L("filename")) );            
    AddTestStepL( RTestVclnt2Overlay::NewL( _L("MM-MMF-VCLNTAVI-I-0422"), _L("SectionFive"), _L("filename"), KErrNotSupported) );
    
    AddTestStepL( RTestVclnt2PlayAfterRemoveWin::NewL( _L("MM-MMF-VCLNTAVI-I-0432"), _L("SectionFive"), _L("filename"), KErrNone) );
    AddTestStepL( RTestVclnt2RemoveWin::NewL( _L("MM-MMF-VCLNTAVI-I-0434"), _L("SectionFive"), _L("filename"), KErrNone, EFalse) );
    
    //RTestMediaClientVideoDisplay
    AddTestStepL( RTestMediaClientVideoDisplay::NewL( _L("MM-MMF-VCLNTAVI-I-0435"), _L("SectionFive"), _L("filename"), KErrNone) );
    
	//
    // PREQ1714 - Video Rendering to a Graphics Surface //
    // NEGATIVE TEST //
    //

    // Ensure the size entered is invalid relative to the video clip to be used
    AddTestStepL( RTestVclnt2InvalidViewport::NewL( _L("MM-MMF-VCLNTAVI-I-0500"), _L("SectionOne"), _L("filename"), KErrNone) );

    AddTestStepL( RTestVclnt2AddWin2::NewL( _L("MM-MMF-VCLNTAVI-I-0501"), _L("SectionOne"), _L("filename"), KErrInUse) );
    AddTestStepL( RTestVclnt2AddWin2WithSettings::NewL( _L("MM-MMF-VCLNTAVI-I-0502"), _L("SectionOne"), _L("filename"), KErrInUse) );

    // use long clips for the following tests
    AddTestStepL( RTestVclnt2ClipRect::NewL( _L("MM-MMF-VCLNTAVI-I-0503"), _L("SectionFive"), _L("filename"), KErrArgument) );
    AddTestStepL( RTestVclnt2ClipRect::NewL( _L("MM-MMF-VCLNTAVI-I-0504"), _L("SectionFive"), _L("filename"), KErrArgument) );
    AddTestStepL( RTestVclnt2AddWin::NewL( _L("MM-MMF-VCLNTAVI-I-0505"), _L("SectionFive"), _L("filename"), KErrArgument) );
    AddTestStepL( RTestVclnt2NoFile::NewL( _L("MM-MMF-VCLNTAVI-I-0506"), _L("SectionFive"), _L("filename"), KErrNotReady) );

    AddTestStepL( RTestVclnt2Viewport::NewL( _L("MM-MMF-VCLNTAVI-I-0508"), _L("SectionOne"), _L("filename"), TSize(100, 100)) );

    AddTestStepL( RTestVclnt2NoGce::NewL( _L("MM-MMF-VCLNTAVI-I-0509"), _L("SectionFive"), _L("filename"), KErrNotSupported) );
    AddTestStepL( RTestVclnt2OldController::NewL( _L("MM-MMF-VCLNTAVI-I-0510"), _L("SectionFive"), _L("filename"), KErrNotSupported) );
    AddTestStepL( RTestVclnt2Alloc::NewL( _L("MM-MMF-VCLNTAVI-I-0511"), _L("SectionFive"), _L("filename"), KErrNone) );
 
    AddTestStepL( RTestVclnt2InvalidScaleFactor::NewL( _L("MM-MMF-VCLNTAVI-I-0512"), _L("SectionFive"), _L("filename"), KErrNone) );
    
	//
    // PREQ1714 - Video Rendering to a Graphics Surface //
    // BINARY COMPATABILITY TESTS //
    //
  
    //These tests are repeats of video player tests above to test the binary compatability of the changes
    //from PREQ1714.  The test cases have been re-numbered to give them unique IDs
    
    //reset value of position
    position = 0;
    
    //************************************* BC Video Player Tests **********************************
    AddTestStepL( RTestVclntPlayAviFile::NewL( _L("MM-MMF-VCLNTAVI-I-9001-CP"), _L("SectionOne"), _L("filename"), KErrNone, ETrue) );
    AddTestStepL( RTestVclntPlayAviDes::NewL( _L("MM-MMF-VCLNTAVI-I-9002-CP"), _L("SectionThree"), _L("filename"), KErrNotSupported, ETrue) );
    AddTestStepL( RTestVclntPlayAviUrl::NewL( _L("MM-MMF-VCLNTAVI-I-9003-CP"), _L("SectionOne"), _L("filename"), KErrNotSupported, ETrue) );
    AddTestStepL( RTestVclntEnqFrameRate::NewL( _L("MM-MMF-VCLNTAVI-I-9004-HP"), _L("SectionThree"), _L("filename"), KErrNone, 10) );
    AddTestStepL( RTestVclntPosition::NewL( _L("MM-MMF-VCLNTAVI-I-9005-HP"), _L("SectionThree"), _L("filename"), KErrNone, position)  ); 
    AddTestStepL( RTestVclntPriority::NewL( _L("MM-MMF-VCLNTAVI-I-9006-HP"), _L("SectionOne"), _L("filename"), KErrNone, -1/*EPriorityMin*/) );
    AddTestStepL( RTestVclntDuration::NewL( _L("MM-MMF-VCLNTAVI-I-9007-HP"), _L("SectionThree"), _L("filename"), KErrNone, position) );
    AddTestStepL( RTestVclntVolume::NewL( _L("MM-MMF-VCLNTAVI-I-9008-HP"), _L("SectionOne"), _L("filename"), KErrNone, -1) );
    AddTestStepL( RTestVclntCloseOpen::NewL( _L("MM-MMF-VCLNTAVI-I-9009-HP"), _L("SectionOne"), _L("filename"), KErrNone) );
    AddTestStepL( RTestVclntPause::NewL( _L("MM-MMF-VCLNTAVI-I-9010-HP"), _L("SectionThree"), _L("filename"), KErrNotSupported) );
    AddTestStepL( RTestVclntBalance::NewL( _L("MM-MMF-VCLNTAVI-I-9011-HP"), _L("SectionOne"), _L("filename"), KErrNone,-10) );
    AddTestStepL( RTestVclntPlayWindow::NewL( _L("MM-MMF-VCLNTAVI-I-9012-HP"), _L("SectionOne"), _L("filename"), KErrNone, position , position) );
    AddTestStepL( RTestVclntMeta::NewL( _L("MM-MMF-VCLNTAVI-I-9013-HP"), _L("SectionOne"), _L("filename"), KErrNotSupported) );
    AddTestStepL( RTestVclntFrameSize::NewL( _L("MM-MMF-VCLNTAVI-I-9014-HP"), _L("SectionOne"), _L("filename"), KErrNone) );
    AddTestStepL( RTestVclntRepeat::NewL( _L("MM-MMF-VCLNTAVI-I-9015-HP"), _L("SectionThree"), _L("filename"), KErrNotSupported, 1) );
    AddTestStepL( RTestVclntMimeType::NewL( _L("MM-MMF-VCLNTAVI-I-9016-HP"), _L("SectionOne"), _L("filename"), KErrNone) );
    AddTestStepL( RTestVclntScale::NewL( _L("MM-MMF-VCLNTAVI-I-9017-HP"), _L("SectionOne"), _L("filename"), KErrNone) );
    AddTestStepL( RTestVclntCrop::NewL( _L("MM-MMF-VCLNTAVI-I-9018-HP"), _L("SectionOne"), _L("filename"), KErrNone) );
    AddTestStepL( RTestVclntGetFrame::NewL( _L("MM-MMF-VCLNTAVI-I-9019-HP"), _L("SectionOne"), _L("filename"), KErrNone, EFalse ) );
    AddTestStepL( RTestVclntGetFrame::NewL( _L("MM-MMF-VCLNTAVI-I-9020-HP"), _L("SectionOne"), _L("filename"), KErrNone, ETrue ) );
    AddTestStepL( RTestVclntRebuffering::NewL( _L("MM-MMF-VCLNTAVI-I-9021-HP"), _L("SectionOne"), _L("filename"), KErrNone) );
    //******************************** BC Video Player RFile Tests ********************************

    AddTestStepL( RTestVclntPlayAviFileHandle::NewL( _L("MM-MMF-VCLNTAVI-I-9022-HP"), _L("SectionOne"), _L("filename"), KErrNone, EFalse ) );
    AddTestStepL( RTestVclntPlayAviFileHandle::NewL( _L("MM-MMF-VCLNTAVI-I-9023-HP"), _L("SectionOne"), _L("filename"), KErrNone, ETrue ) );

    //************************************* BC Video Player Tests **********************************
    AddTestStepL( RTestVclntDelete::NewL( _L("MM-MMF-VCLNTAVI-I-9101-HP"), _L("SectionOne"), _L("filename"), KErrNone) );
    AddTestStepL( RTestVclntFrameSize::NewL( _L("MM-MMF-VCLNTAVI-I-9102-HP"), _L("SectionThree"), _L("filename"), KErrNone) );
    position = 103423423;
    AddTestStepL( RTestVclntPosition::NewL( _L("MM-MMF-VCLNTAVI-I-9103-HP"), _L("SectionThree"), _L("filename"), KErrNone, position) ); 
    position = -12;
    AddTestStepL( RTestVclntPosition::NewL( _L("MM-MMF-VCLNTAVI-I-9104-HP"), _L("SectionThree"), _L("filename"), KErrNone, position) ); 
    AddTestStepL( RTestVclntVolume::NewL( _L("MM-MMF-VCLNTAVI-I-9105-HP"), _L("SectionOne"), _L("filename"), KErrNone, 1000400) );
    AddTestStepL( RTestVclntVolume::NewL( _L("MM-MMF-VCLNTAVI-I-9106-HP"), _L("SectionOne"), _L("filename"), KErrNone, -100) );
    AddTestStepL( RTestVclntBalance::NewL( _L("MM-MMF-VCLNTAVI-I-9107-HP"), _L("SectionOne"), _L("filename"), KErrNone,200) );
    AddTestStepL( RTestVclntBalance::NewL( _L("MM-MMF-VCLNTAVI-I-9108-HP"), _L("SectionOne"), _L("filename"), KErrNone,-200) );
    position = 2000000;
    AddTestStepL( RTestVclntPlayWindow::NewL( _L("MM-MMF-VCLNTAVI-I-9109-HP"), _L("SectionOne"), _L("filename"), KErrNone, position , position) );
    AddTestStepL( RTestVclntPlayAviFile::NewL( _L("MM-MMF-VCLNTAVI-I-9110-HP"), _L("SectionOne"), _L("corruptFile"), KErrCorrupt, ETrue) );
    AddTestStepL( RTestVclntPlayAviFile::NewL( _L("MM-MMF-VCLNTAVI-I-9111-HP"), _L("SectionOne"), _L("NoCodecFile"), KErrNotSupported, ETrue) );
    //******************************* BC Video Player RFile Tests *********************************
#ifdef SYMBIAN_SECURITY_CAF_RFILE_HANDLE
    AddTestStepL( RTestVclntPlayAviFileHandle::NewL( _L("MM-MMF-VCLNTAVI-I-9112-HP"), _L("SectionOne"), _L("textFile"), KErrNotSupported, EFalse) );
    AddTestStepL( RTestVclntPlayAviFileHandle::NewL( _L("MM-MMF-VCLNTAVI-I-9113-HP"), _L("SectionTwo"), _L("filename"), KErrNotFound, EFalse ) );
#endif
    
	//
    // BC APR - POSITIVE TEST //
    //	

	AddTestStepL( RTestVclntAprRegOnce4SameEvent::NewL(_L("MM-MMF-VCLNTAVIAPR-I-9001-HP"),_L("SectionAPR") ) );
	AddTestStepL( RTestVclntAprRegB4Open::NewL(_L("MM-MMF-VCLNTAVIAPR-I-9002-HP"),_L("SectionAPR") ) );
	AddTestStepL( RTestVclntAprRegAftrOpenAndB4Play::NewL(_L("MM-MMF-VCLNTAVIAPR-I-9003-HP"),_L("SectionAPR") ) );
	AddTestStepL( RTestVclntAprRegAftrHpClntCompltPlay::NewL(_L("MM-MMF-VCLNTAVIAPR-I-9004-HP"),_L("SectionAPR") ) ); 
	AddTestStepL( RTestVclntAprRegSamePriorityPlayers::NewL(_L("MM-MMF-VCLNTAVIAPR-I-9005-HP"),_L("SectionAPR") ) );
	AddTestStepL( RTestVclntAprRegAtPlayAndCancelNfn::NewL(_L("MM-MMF-VCLNTAVIAPR-I-9006-HP"),_L("SectionAPR") ) );
	AddTestStepL( RTestVclntAprTestWillResumePlay::NewL(_L("MM-MMF-VCLNTAVIAPR-I-9007-HP"),_L("SectionAPR") ) );
	
	//
    // BC APR - ALLOC TEST //
    //	
	AddTestStepL( RTestVclntAprAllocUsingSharedHeap::NewL(_L("MM-MMF-VCLNTAVIAPR-I-9008-HP"),_L("SectionAPR") ) );
	
	//
    // BC APR - NEGATIVE TEST //
    //
    
    AddTestStepL( RTestVclntAprMultRegAtPlay::NewL(_L("MM-MMF-VCLNTAVIAPR-I-9101-HP"),_L("SectionAPR") ) );
    AddTestStepL( RTestVclntAprCancelNoResourceNfn::NewL(_L("MM-MMF-VCLNTAVIAPR-I-9102-HP"),_L("SectionAPR") ) );
    AddTestStepL( RTestVclntAprRegUnsupportedEvent::NewL(_L("MM-MMF-VCLNTAVIAPR-I-9103-HP"),_L("SectionAPR") ) );
    AddTestStepL( RTestVclntAprCancelUnsupportedEvent::NewL(_L("MM-MMF-VCLNTAVIAPR-I-9104-HP"),_L("SectionAPR") ) ); 
	
    // BC Positive Tests
    AddTestStepL( RTestVclntSetScrIdAndPlayAviFile::NewL( _L("MM-MMF-VCLNTAVI-I-9201-CP"), _L("SectionOne"), _L("filename"), KErrNone, 1) );
    AddTestStepL( RTestVclntSetScrIdAndPlayAviDes::NewL( _L("MM-MMF-VCLNTAVI-I-9202-CP"), _L("SectionOne"), _L("filename"), KErrNotSupported, 1) );
    AddTestStepL( RTestVclntSetScrIdAndPlayAviUrl::NewL( _L("MM-MMF-VCLNTAVI-I-9203-CP"), _L("SectionOne"), _L("filename"), KErrNotSupported, 1) );
    AddTestStepL( RTestVclntSetScrIdAndPlayStopAviFile::NewL( _L("MM-MMF-VCLNTAVI-I-9204-CP"), _L("SectionOne"), _L("filename"), KErrNone, 1, 2) );
    AddTestStepL( RTestVclntSetScrIdAndPlayPauseAviFile::NewL( _L("MM-MMF-VCLNTAVI-I-9205-CP"), _L("SectionOne"), _L("filename"), KErrNone, 1, 2) );
    AddTestStepL( RTestVclntSetScrIdAndPlayStopAviFile::NewL( _L("MM-MMF-VCLNTAVI-I-9206-CP"), _L("SectionFour"), _L("filename"), KErrNone, 0, 1) );
    AddTestStepL( RTestVclntSetScrIdAndPlayPauseAviFile::NewL( _L("MM-MMF-VCLNTAVI-I-9207-CP"), _L("SectionFour"), _L("filename"), KErrNone, 0, 1) );
    AddTestStepL( RTestVclntSetScrIdAfterPlayAndPlayAviFile::NewL( _L("MM-MMF-VCLNTAVI-I-9208-CP"), _L("SectionOne"), _L("filename"), KErrNone, 1) );
    AddTestStepL( RTestVclntSetScrIdAfterOpenAndPlayAviFile::NewL( _L("MM-MMF-VCLNTAVI-I-9209-CP"), _L("SectionOne"), _L("filename"), KErrNone, 1) );
    AddTestStepL( RTestVclntSetScrIdDuringPlayAndPlayAviFile::NewL( _L("MM-MMF-VCLNTAVI-I-9210-CP"), _L("SectionOne"), _L("filename"), KErrNone, 1) );

    // BC Negative Tests
    AddTestStepL( RTestVclntSetScrIdAndPlayAviFile::NewL( _L("MM-MMF-VCLNTAVI-I-9251-CP"), _L("SectionOne"), _L("filename"), KErrNotSupported, 3) );

	// PREQ1804 tests
	AddTestStepL(RTestStepVclnt2CRP::NewL(_L("MM-MMF-VCLNTAVI-I-0700")));
	AddTestStepL(RTestStepVclnt2CRP::NewL(_L("MM-MMF-VCLNTAVI-I-0701")));
	AddTestStepL(RTestStepVclnt2CRP::NewL(_L("MM-MMF-VCLNTAVI-I-0702")));

#endif  // SYMBIAN_BUILD_GCE    
    
#ifdef SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT
	//
	// PREQ1939 - Video Subtitles and Captioning Support//
	// POSITIVE TEST //
	//
	AddTestStepL( RTestVclnt2Subtitle::NewL( _L("MM-MMF-VCLNTAVI-I-0600-CP"), _L("SectionSubtitle"), _L("subtitleLong"), KErrNone));
	AddTestStepL( RTestVclnt2SubtitleEnableDuringPlayback::NewL( _L("MM-MMF-VCLNTAVI-I-0602-CP"), _L("SectionSubtitle"), _L("subtitleAvailable"), KErrNone));
	AddTestStepL( RTestVclnt2SubtitleDisableDuringPlayback::NewL( _L("MM-MMF-VCLNTAVI-I-0603-CP"), _L("SectionSubtitle"), _L("subtitleAvailable"), KErrNone));
	AddTestStepL( RTestVclnt2SubtitleDisableTwice::NewL( _L("MM-MMF-VCLNTAVI-I-0605-CP"), _L("SectionSubtitle"), _L("subtitleAvailable"), KErrNone));
	AddTestStepL( RTestVclnt2SubtitleWindowClipBefore::NewL( _L("MM-MMF-VCLNTAVI-I-0606-CP"), _L("SectionSubtitle"), _L("subtitleAvailable"), KErrNone));
	AddTestStepL( RTestVclnt2SubtitleWindowClipDuring::NewL( _L("MM-MMF-VCLNTAVI-I-0607-CP"), _L("SectionSubtitle"), _L("subtitleAvailable"), KErrNone));
	AddTestStepL( RTestVclnt2SubtitleRotateBefore::NewL( _L("MM-MMF-VCLNTAVI-I-0608-CP"), _L("SectionSubtitle"), _L("subtitleAvailable"), KErrNone));
	AddTestStepL( RTestVclnt2SubtitleRotateDuring::NewL( _L("MM-MMF-VCLNTAVI-I-0609-CP"), _L("SectionSubtitle"), _L("subtitleAvailable"), KErrNone));
	AddTestStepL( RTestVclnt2SubtitleRedraw::NewL( _L("MM-MMF-VCLNTAVI-I-0610-CP"), _L("SectionSubtitle"), _L("subtitleAvailable"), KErrNone));
	AddTestStepL( RTestVclnt2SubtitleRotateClipRedraw::NewL( _L("MM-MMF-VCLNTAVI-I-0611-CP"), _L("SectionSubtitle"), _L("subtitleAvailable"), KErrNone));
	AddTestStepL( RTestVclnt2SubtitleEnableDisableStress::NewL( _L("MM-MMF-VCLNTAVI-I-0612-CP"), _L("SectionSubtitle"), _L("subtitleAvailable"), KErrNone));
	AddTestStepL( RTestVclnt2SubtitleRemoveDisplayDuring::NewL( _L("MM-MMF-VCLNTAVI-I-0613-CP"), _L("SectionSubtitle"), _L("subtitleAvailable"), KErrNone));
	AddTestStepL( RTestVclnt2SubtitleRedraw::NewL( _L("MM-MMF-VCLNTAVI-I-0614-CP"), _L("SectionSubtitle"), _L("subtitleAvailable"), KErrNone, EFalse));
	AddTestStepL( RTestVclnt2SubtitleRedraw::NewL( _L("MM-MMF-VCLNTAVI-I-0615-CP"), _L("SectionSubtitle"), _L("subtitleUnavailable"), KErrNone, EFalse));
	AddTestStepL( RTestVclnt2SubtitleEnableDisableStress::NewL( _L("MM-MMF-VCLNTAVI-I-0616-CP"), _L("SectionSubtitle"), _L("subtitleUnavailable"), KErrNone));
	AddTestStepL( RTestVclnt2SubtitleEnableDisableStress::NewL( _L("MM-MMF-VCLNTAVI-I-0617-CP"), _L("SectionSubtitle"), _L("subtitleCorrupt"), KErrNone));
	
	//
	// PREQ1939 - Video Subtitles and Captioning Support//
	// NEGATIVE TEST //
	//
	AddTestStepL( RTestVclnt2Subtitle::NewL( _L("MM-MMF-VCLNTAVI-I-0700-CP"), _L("SectionSubtitle"), _L("subtitleCorrupt"), KErrNone));
	AddTestStepL( RTestVclnt2SubtitleNotAvailable::NewL( _L("MM-MMF-VCLNTAVI-I-0701-CP"), _L("SectionSubtitle"), _L("subtitleUnavailable"), KErrNone));
	AddTestStepL( RTestVclnt2SubtitleLanguage::NewL( _L("MM-MMF-VCLNTAVI-I-0705-CP"), _L("SectionSubtitle"), _L("subtitleAvailable"), KErrNone));
	AddTestStepL( RTestVclnt2SubtitleAlloc::NewL( _L("MM-MMF-VCLNTAVI-I-0706-CP"), _L("SectionSubtitle"), _L("subtitleAvailable"), KErrNone));
	AddTestStepL( RTestVclnt2SubtitleEnableTwice::NewL( _L("MM-MMF-VCLNTAVI-I-0707-CP"), _L("SectionSubtitle"), _L("subtitleAvailable"), KErrNone));	
	AddTestStepL( RTestVclnt2SubtitleMultipleWindows::NewL( _L("MM-MMF-VCLNTAVI-I-0708-CP"), _L("SectionSubtitle"), _L("subtitleAvailable"), KErrNone));
#endif //SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT
    
    }
    
    #ifdef __WINS__

    /**
     * Wait synchronously for the first WS redraw event...
 */
TBool CTestMmfVclntAviSuite::WaitUntilWindowServerReadyL()
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

/**
 * InitWservL
 */
void CTestMmfVclntAviSuite::InitWservL()
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

/**
 * DeInitWserv
 */
void CTestMmfVclntAviSuite::DeInitWserv()
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
    
/**
 * CreateDependencyFileL
 */
void CTestMmfVclntAviSuite::CreateDependencyFileL()
    {
    // create the file the plugin depends on.- enforce dependency
    User::LeaveIfError(iFileSession.Connect());
    User::LeaveIfError(iFileSession.ShareProtected());

    iFileSession.Delete(KMMCameraPluginName);

    RFile file;
    TInt err = file.Replace(iFileSession, KECamTestCameraPluginName, EFileShareAny);
      file.Close();
      if(err != KErrNone)
          {
        User::LeaveIfError(KErrNotSupported);
          }
    }
    
/**
 * RemoveDependencyFile
 */
void CTestMmfVclntAviSuite::RemoveDependencyFile()
    {
    iFileSession.Delete(KECamTestCameraPluginName);
    iFileSession.Close();
    }

