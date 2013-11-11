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
//

// System includes
#include <e32base.h>

// Test system includes
#include "tsi_mvs_agents_suite.h"
#include "tsi_mvs_agents_stepbase.h"
#include "tsi_mvs_audio.h"
#include "tsi_mvs_video.h"
#include "tsi_mvs_plugininfo.h"
#include "tsi_mvs_secdisp.h"
#include "tsi_mvs_apr.h"


/** 
 *
 * NewTestSuiteL
 *     NewTestSuite is exported at ordinal 1
 *     this provides the interface to allow schedule test
 *     to create instances of this test suite
 *
 */
EXPORT_C CTestSuiteMVSAgent* NewTestSuiteL()
    { 
    CTestSuiteMVSAgent* result = new (ELeave) CTestSuiteMVSAgent;
    CleanupStack::PushL(result);
    result->ConstructL();
    CleanupStack::Pop(); // result
    return result;
    }

/**
 *
 * ~CTestSuiteMVSAgent
 *
 */
CTestSuiteMVSAgent::~CTestSuiteMVSAgent() 
    {}

/**
 *
 * GetVersion
 *     Get Test Suite version
 *
 */
TPtrC CTestSuiteMVSAgent::GetVersion() const
    {
    _LIT(KTxtVersion,"0.01");
    return KTxtVersion();
    }


/**
 *
 * AddTestStepL
 *     Add a test step into the suite
 *
 */
void CTestSuiteMVSAgent::AddTestStepL( RTestStepMVSBase* aTestStep )
    {
    // test steps contain a pointer back to the suite which owns them
    aTestStep->SetTestSuite(this);
    
    // add the step using the base class method
    CTestSuite::AddTestStepL(aTestStep);
    }

void CTestSuiteMVSAgent::AddTestStepL( RTestMVSVideoAgentSetScrIdAndPlayBase* aTestStep )
    {
    // test steps contain a pointer back to the suite which owns them
    aTestStep->SetTestSuite(this);
    
    // add the step using the base class method
    CTestSuite::AddTestStepL(aTestStep);
    }
void CTestSuiteMVSAgent::AddTestStepL( RTestStepMVSAgentsAPRBase* aTestStep )
	{
	// test steps contain a pointer back to the suite which owns them
	aTestStep->SetTestSuite(this);

	// add the step using the base class method
	CTestSuite::AddTestStepL(aTestStep);
	}
/**
 *
 * InitialiseL
 *     Constructor for test suite
 *     this creates all the test steps and 
 *     stores them inside CTestSuiteMVSAgent
 *
 */
void CTestSuiteMVSAgent::InitialiseL()
    {
    // store the name of this test suite 
    iSuiteName = _L("TSI_MVS_AGENTS");
    
    // AUDIO CLIENT AGENTS :: Positive Testing
    AddTestStepL(new (ELeave) RTestStepMVSAudPositionPlayRec(_L("MM-MVS-AUDIOAGENT-I-0001-HP"), _L("Ogg"), _L("RecordFile")) );
    AddTestStepL(new (ELeave) RTestStepMVSAudRptPlayRec(_L("MM-MVS-AUDIOAGENT-I-0002-HP"), _L("Wav"), _L("RecordFile")) );
    AddTestStepL(new (ELeave) RTestStepMVSAudOtherPlayRec(_L("MM-MVS-AUDIOAGENT-I-0003-HP"), _L("Ogg"), _L("RecordFile")) );
    AddTestStepL(new (ELeave) RTestStepMVSAudRecNew(_L("MM-MVS-AUDIOAGENT-I-0004-HP"), _L("Ogg"), _L("NewRecordFile1"), KTestOggRecordControllerUid,KTestOggPlayControllerUid) );
    AddTestStepL(new (ELeave) RTestStepMVSAudRecNew(_L("MM-MVS-AUDIOAGENT-I-0005-HP"), _L("Wav"), _L("NewRecordFile1"), KTestMmfAudioControllerUid,KTestMmfAudioControllerUid) );
    AddTestStepL(new (ELeave) RTestStepMVSAudPlaySettings(_L("MM-MVS-AUDIOAGENT-I-0006-HP"), _L("Ogg"), _L("PlayFile")) );
    AddTestStepL(new (ELeave) RTestStepMVSAudRecConfig(_L("MM-MVS-AUDIOAGENT-I-0007-HP"), _L("Ogg"), _L("NewRecordFile2")) );
    AddTestStepL(new (ELeave) RTestStepMVSAudRecCrop(_L("MM-MVS-AUDIOAGENT-I-0008-HP"), _L("Ogg"), _L("RecordFile")) );
    AddTestStepL(new (ELeave) RTestStepMVSAudRecMeta(_L("MM-MVS-AUDIOAGENT-I-0009-HP"), _L("Ogg"), _L("RecordFile")) );
    AddTestStepL(new (ELeave) RTestStepMVSAudioRecordAgentBasic(_L("MM-MVS-AUDIOAGENT-I-0010-HP"), _L("Ogg"), _L("NewRecordFile3"), _L("ALLOC"), KTestOggRecordControllerUid, KTestOggPlayControllerUid) );
    AddTestStepL(new (ELeave) RTestStepMVSAudioPlayAgentBasic(_L("MM-MVS-AUDIOAGENT-I-0011-HP"), _L("Ogg"), _L("NewRecordFile3"), _L("ALLOC")) );
    AddTestStepL(new (ELeave) RTestStepMVSAudioRecordAgentBasic(_L("MM-MVS-AUDIOAGENT-I-0012-HP"), _L("Ogg"), _L("NewRecordFile4"), _L("LOG"), KTestOggRecordControllerUid, KTestOggPlayControllerUid) );
    AddTestStepL(new (ELeave) RTestStepMVSAudioPlayAgentBasic(_L("MM-MVS-AUDIOAGENT-I-0013-HP"), _L("Ogg"), _L("NewRecordFile4"), _L("LOG")) );
    AddTestStepL(new (ELeave) RTestStepMVSAudPlayPausePlay(_L("MM-MVS-AUDIOAGENT-I-0019-HP"), _L("Wav"), _L("RecordFile")) );

        
    // AUDIO CLIENT AGENTS :: Negative Testing
    AddTestStepL(new (ELeave) RTestStepMVSAudOpenWhenOpen(_L("MM-MVS-AUDIOAGENT-I-0014-HP"), _L("Ogg"), _L("PlayFile"), _L("RecordFile")) );
    AddTestStepL(new (ELeave) RTestStepMVSAudOpenUnsupportedFile(_L("MM-MVS-AUDIOAGENT-I-0015-HP"), _L("Ogg"), _L("MP3File")) );
    AddTestStepL(new (ELeave) RTestStepMVSAudOpenWhenPlaying(_L("MM-MVS-AUDIOAGENT-I-0016-HP"), _L("Ogg"), _L("PlayFile")) );
    AddTestStepL(new (ELeave) RTestStepMVSAudRecUnsupportedFile(_L("MM-MVS-AUDIOAGENT-I-0017-HP"), _L("Ogg"), _L("MP3File")) );
    AddTestStepL(new (ELeave) RTestStepMVSAudRecUnsupportedFile(_L("MM-MVS-AUDIOAGENT-I-0018-HP"), _L("Ogg"), _L("TextFile")) );
    
    // VIDEO CLIENT AGENTS :: Positive Testing
    AddTestStepL(new (ELeave) RTestStepMVSVidAviPlayPos(_L("MM-MVS-VIDEOAGENT-I-0001-HP"), _L("XviD"), _L("PlayFileVideo")) );
    AddTestStepL(new (ELeave) RTestStepMVSVidAviPlayRot(_L("MM-MVS-VIDEOAGENT-I-0002-HP"), _L("XviD"), _L("PlayFileVideo")) );
    AddTestStepL(new (ELeave) RTestStepMVSVidAviPlayScl(_L("MM-MVS-VIDEOAGENT-I-0003-HP"), _L("XviD"), _L("PlayFileVideo")) );
    AddTestStepL(new (ELeave) RTestStepMVSVidPlaySettings(_L("MM-MVS-VIDEOAGENT-I-0004-HP"), _L("XviD"), _L("PlayFileAudio")) );
    AddTestStepL(new (ELeave) RTestStepMVSVidAviRecPlay(_L("MM-MVS-VIDEOAGENT-I-0005-HP"), _L("XviD_RecNew"), _L("RecordFile")) );
    AddTestStepL(new (ELeave) RTestStepMVSVidRecSettings(_L("MM-MVS-VIDEOAGENT-I-0006-HP"), _L("XviD"), _L("RecordFile")) );
    AddTestStepL(new (ELeave) RTestStepMVSVidRecMeta(_L("MM-MVS-VIDEOAGENT-I-0007-HP"), _L("XviD"), _L("RecordFile")) );
    AddTestStepL(new (ELeave) RTestStepMVSVideoRecordAgentBasic(_L("MM-MVS-VIDEOAGENT-I-0008-HP"), _L("XviD_RecNew"), _L("RecordFile"), _L("ALLOC")) );
    AddTestStepL(new (ELeave) RTestStepMVSVideoPlayAgentBasic(_L("MM-MVS-VIDEOAGENT-I-0009-HP"), _L("XviD_RecNew"), _L("RecordFile"), _L("ALLOC")) );
    AddTestStepL(new (ELeave) RTestStepMVSVideoRecordAgentBasic(_L("MM-MVS-VIDEOAGENT-I-0010-HP"), _L("XviD_RecNew"), _L("RecordFile"), _L("LOG")) );
    AddTestStepL(new (ELeave) RTestStepMVSVideoPlayAgentBasic(_L("MM-MVS-VIDEOAGENT-I-0011-HP"), _L("XviD_RecNew"), _L("RecordFile"), _L("LOG")) );
    AddTestStepL(new (ELeave) RTestStepMVSVidRecPause(_L("MM-MVS-VIDEOAGENT-I-0017"), _L("XviD"), _L("RecordFile")) );    
    AddTestStepL(new (ELeave) RTestStepMVSVidAviPlayDisplaySetting(_L("MM-MVS-VIDEOAGENT-I-0018-HP"), _L("XviD"), _L("PlayFileVideo")) );    
    AddTestStepL(new (ELeave) RTestStepMVSVidAviPlayChangeWin(_L("MM-MVS-VIDEOAGENT-I-0019-HP"), _L("XviD"), _L("PlayFileVideo")) );    
    AddTestStepL(new (ELeave) RTestStepMVSVidAviPlayOverlayText(_L("MM-MVS-VIDEOAGENT-I-0020-HP"), _L("XviD"), _L("PlayFileVideo")) );    
    AddTestStepL(new (ELeave) RTestStepMVSVidAviPlayCropRegion(_L("MM-MVS-VIDEOAGENT-I-0021-HP"), _L("XviD"), _L("PlayFileVideo")) );        
    
    // VIDEO CLIENT AGENTS :: Negative Testing
    AddTestStepL(new (ELeave) RTestStepMVSVidPlayUnsupportedFile(_L("MM-MVS-VIDEOAGENT-I-0012-HP"), _L("XviD"), _L("CorruptFile"), KErrNotSupported) );
    AddTestStepL(new (ELeave) RTestStepMVSVidPlayUnsupportedFile(_L("MM-MVS-VIDEOAGENT-I-0013-HP"), _L("XviD"), _L("InvalidCodecFile"), KErrNotSupported) );
    AddTestStepL(new (ELeave) RTestStepMVSVidPlayUnsupportedFile(_L("MM-MVS-VIDEOAGENT-I-0014-HP"), _L("XviD"), _L("TextFile"), KErrNotSupported) );
    AddTestStepL(new (ELeave) RTestStepMVSVidRecUnsupportedFile(_L("MM-MVS-VIDEOAGENT-I-0015-HP"), _L("XviD"), _L("InvalidCodecFile")) );
    AddTestStepL(new (ELeave) RTestStepMVSVidRecUnsupportedFile(_L("MM-MVS-VIDEOAGENT-I-0016-HP"), _L("XviD"), _L("TextFile")) );
    
    // MVS PLUG-IN SELECTION :: Positive Testing
    AddTestStepL(new (ELeave) RTestStepMVSPluginInfoPos(_L("MM-MVS-PLUGIN-I-0001-HP"), _L("PLUGIN"), _L("AudioFile"), KUidMediaTypeAudio, KTestOggPlayControllerUid) );
    AddTestStepL(new (ELeave) RTestStepMVSPluginInfoPos(_L("MM-MVS-PLUGIN-I-0002-HP"), _L("PLUGIN"), _L("VideoFile"), KUidMediaTypeVideo, KTestAviPlayControllerUid) );
    AddTestStepL(new (ELeave) RTestStepMVSPluginInfoPos(_L("MM-MVS-PLUGIN-I-0003-HP"), _L("PLUGIN"), _L("SjauFile"), KUidMediaTypeAudio, KTestMmfAudioControllerUid) );
    
    // MVS PLUG-IN SELECTION :: Negative Testing
    AddTestStepL(new (ELeave) RTestStepMVSPluginInfoNeg(_L("MM-MVS-PLUGIN-I-1001-HP"), _L("PLUGIN"), _L("TextFile"), KNullUid, KNullUid) );
        
    // MVS SecDisp SELECTION :: Positive Testing
    AddTestStepL(RTestMVSVideoAgentSetScrIdAndPlay::NewL(_L("MM-MVS-VIDEOAGENT-I-4001"),  _L("SECDISP"), _L("filename"), KErrNone, 1) );
    AddTestStepL(RTestMVSVideoAgentSetScrIdDuringPlayAndPlayVideoFile::NewL(_L("MM-MVS-VIDEOAGENT-I-4002"),  _L("SECDISP"), _L("filename"), KErrNone, 2) );
    
    // MVS SecDisp SELECTION :: Negative Testing
    AddTestStepL(RTestMVSVideoAgentSetScrIdAndPlay::NewL(_L("MM-MVS-VIDEOAGENT-I-5001"),  _L("SECDISP"), _L("filename"), KErrNotSupported, -1) );
    
    //MVS APR SECTION :: Positive Testing
 	AddTestStepL(RTestMVSAgentAPRRegOnce4SameEvent::NewL(_L("MM-MVS-VIDEOAGENT-I-2001"),  _L("APR")) );
  	AddTestStepL(RTestMVSAgentAPRRegB4Open::NewL(_L("MM-MVS-VIDEOAGENT-I-2002"),  _L("APR")) );
   	AddTestStepL(RTestMVSAgentAPRRegAfterOpenB4Play::NewL(_L("MM-MVS-VIDEOAGENT-I-2003"),  _L("APR")) );
   	AddTestStepL(RTestMVSAgentAPRRegAftrHpClntCompltPlay::NewL(_L("MM-MVS-VIDEOAGENT-I-2004"),  _L("APR")) );
    AddTestStepL(RTestMVSAgentAPRRegSamePriorityPlayers::NewL(_L("MM-MVS-VIDEOAGENT-I-2005"),  _L("APR")) );
	AddTestStepL(RTestMVSAgentAPRRegAtPlayAndCancelNfn::NewL(_L("MM-MVS-VIDEOAGENT-I-2006"),  _L("APR")) );
    AddTestStepL(RTestMVSAgentAPRTestWillResumePlay::NewL(_L("MM-MVS-VIDEOAGENT-I-2007"),  _L("APR")) );
    
    // MVS APR SELECTION :: Negative Testing
    AddTestStepL(RTestMVSAgentAPRCancelNoResourceNfn::NewL(_L("MM-MVS-VIDEOAGENT-I-3001"),  _L("APR")) );
    AddTestStepL(RTestMVSAgentAPRTestWillResumePlayCancelNfn::NewL(_L("MM-MVS-VIDEOAGENT-I-3004"),  _L("APR")) );    
    AddTestStepL(RTestMVSAgentAPRMulRegAtPlay::NewL(_L("MM-MVS-VIDEOAGENT-I-3005"),  _L("APR")) );  
    
    // VIDEO CLIENT AGENTS :: Compatibility Testing
    AddTestStepL(new (ELeave) RTestStepMVSVidAviPlayDisplaySetting(_L("MM-MVS-VIDEOAGENT-I-9018-HP"), _L("XviD"), _L("PlayFileVideo")) );
    AddTestStepL(new (ELeave) RTestStepMVSVidAviPlayOverlayText(_L("MM-MVS-VIDEOAGENT-I-9020-HP"), _L("XviD"), _L("PlayFileVideo")) ); 
    }

