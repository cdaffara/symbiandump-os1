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
// TSU_MMF_OGGCTRL_TestSuit.cpp
// 
//

#include "teststepoggctrlplay.h"
#include "teststepoggctrlrecord.h"

/**
 * NewTestSuite is exported at ordinal 1
 * this provides the interface to allow schedule test
 * to create instances of this test suite
 */
EXPORT_C CTestSuiteOggCtrl* NewTestSuiteOggCtrlL()
    { 
	CTestSuiteOggCtrl* self = new (ELeave) CTestSuiteOggCtrl;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
    }

CTestSuiteOggCtrl::~CTestSuiteOggCtrl()
	{
	}
/**
 * Add a test step into the suite
 *
 * @param test step pointer to add
 */
void CTestSuiteOggCtrl::AddTestStepL( RTestStepOggCtrlBase* aPtrTestStep )
	{
    ASSERT( aPtrTestStep );
	aPtrTestStep->SetSuite(this); 
	CTestSuite::AddTestStepL(aPtrTestStep);
	}
/**
 * Creates all the test steps and stores them inside CTestSuiteOggCtrl 
 */
void CTestSuiteOggCtrl::InitialiseL( void )
	{
	InitSystemPath();
	iSuiteName = _L("tsu_mmf_oggctrl");
	//OggVorbisPlayer tests
	AddTestStepL( new(ELeave) RTestStepOggCtrlPlay(_L("MM-MMF-OGGCTRL-U-0001-HP"), _L("SectionPlayer"), _L("VorbisStereo48KHz224Kbs"), KOggPlayControllerUid));
	AddTestStepL( new(ELeave) RTestStepOggCtrlVolume(_L("MM-MMF-OGGCTRL-U-0002-HP"), _L("SectionPlayer"), _L("VorbisStereo48KHz224Kbs"), KOggPlayControllerUid));
	AddTestStepL( new(ELeave) RTestStepOggCtrlPlayBalance(_L("MM-MMF-OGGCTRL-U-0003-HP"), _L("SectionPlayer"), _L("VorbisStereo48KHz224Kbs"), KOggPlayControllerUid));
	AddTestStepL( new(ELeave) RTestStepOggCtrlPosition(_L("MM-MMF-OGGCTRL-U-0004-HP"), _L("SectionPlayer"), _L("VorbisStereo44KHz128Kbs"), KOggPlayControllerUid));
	AddTestStepL( new(ELeave) RTestStepOggCtrlDuration(_L("MM-MMF-OGGCTRL-U-0005-HP"), _L("SectionPlayer"), _L("VorbisStereo44KHz128Kbs"), KOggPlayControllerUid));
	AddTestStepL( new(ELeave) RTestStepOggCtrlPause(_L("MM-MMF-OGGCTRL-U-0006-HP"), _L("SectionPlayer"), _L("VorbisStereo32KHz120Kbs"), KOggPlayControllerUid));
	AddTestStepL( new(ELeave) RTestStepOggCtrlStop(_L("MM-MMF-OGGCTRL-U-0007-HP"), _L("SectionPlayer"), _L("VorbisStereo32KHz120Kbs"), KOggPlayControllerUid));
	AddTestStepL( new(ELeave) RTestStepOggCtrlMetaData(_L("MM-MMF-OGGCTRL-U-0008-HP"), _L("SectionPlayer"), _L("VorbisStereo32KHz120Kbs"), KOggPlayControllerUid));
	AddTestStepL( new(ELeave) RTestStepOggCtrlPlayWindow(_L("MM-MMF-OGGCTRL-U-0009-HP"), _L("SectionPlayer"), _L("VorbisStereo44KHz128Kbs"), KOggPlayControllerUid));
	AddTestStepL( new(ELeave) RTestStepOggCtrlGetSetConfig(_L("MM-MMF-OGGCTRL-U-0010-HP"), _L("SectionPlayer"), _L("VorbisStereo44KHz128Kbs"), KOggPlayControllerUid));
	AddTestStepL( new(ELeave) RTestStepOggCtrlPlayInvalid(_L("MM-MMF-OGGCTRL-U-0011-HP"), _L("SectionPlayer"), _L("InvalidOgg"), KOggPlayControllerUid));
	AddTestStepL( new(ELeave) RTestStepOggCtrlSetInvalidConfig(_L("MM-MMF-OGGCTRL-U-0012-HP"), _L("SectionPlayer"), _L("VorbisStereo44KHz128Kbs"), KOggPlayControllerUid));
	AddTestStepL( new(ELeave) RTestStepOggCtrlPositionNegative(_L("MM-MMF-OGGCTRL-U-0013-HP"), _L("SectionPlayer"), _L("VorbisStereo44KHz128Kbs"), KOggPlayControllerUid));
	//OggVorbisRecoder tests
	AddTestStepL( new(ELeave) RTestStepOggCtrlRecord(_L("MM-MMF-OGGCTRL-U-0021-HP"), _L("SectionRecorder"), _L("RecordOgg"), KOggRecordControllerUid));
	AddTestStepL( new(ELeave) RTestStepOggCtrlGain(_L("MM-MMF-OGGCTRL-U-0022-HP"), _L("SectionRecorder"), _L("RecordOgg"), KOggRecordControllerUid));
	AddTestStepL( new(ELeave) RTestStepOggCtrlRecordBalance(_L("MM-MMF-OGGCTRL-U-0023-HP"), _L("SectionRecorder"), _L("RecordOgg"), KOggRecordControllerUid));
	AddTestStepL( new(ELeave) RTestStepOggCtrlSetMaxFileSize(_L("MM-MMF-OGGCTRL-U-0024-HP"), _L("SectionRecorder"), _L("RecordOgg"), KOggRecordControllerUid));
	AddTestStepL( new(ELeave) RTestStepOggCtrlRecordTimeAvailable(_L("MM-MMF-OGGCTRL-U-0025-HP"), _L("SectionRecorder"), _L("RecordOgg"), KOggRecordControllerUid));
	AddTestStepL( new(ELeave) RTestStepOggCtrlGetSetSinkChannels(_L("MM-MMF-OGGCTRL-U-0026-HP"), _L("SectionRecorder"), _L("RecordOgg"), KOggRecordControllerUid));
	AddTestStepL( new(ELeave) RTestStepOggCtrlGetSetSinkSampleRate(_L("MM-MMF-OGGCTRL-U-0027-HP"), _L("SectionRecorder"), _L("RecordOgg"), KOggRecordControllerUid));
	AddTestStepL( new(ELeave) RTestStepOggCtrlGetSetSinkBitRate(_L("MM-MMF-OGGCTRL-U-0028-HP"), _L("SectionRecorder"), _L("RecordOgg"), KOggRecordControllerUid));
	AddTestStepL( new(ELeave) RTestStepOggCtrlGetSetSinkDataType(_L("MM-MMF-OGGCTRL-U-0029-HP"), _L("SectionRecorder"), _L("RecordOgg"), KOggRecordControllerUid));
	AddTestStepL( new(ELeave) RTestStepOggCtrlGetSetMetaData(_L("MM-MMF-OGGCTRL-U-0030-HP"), _L("SectionRecorder"), _L("RecordOgg"), KOggRecordControllerUid));
	AddTestStepL( new(ELeave) RTestStepOggCtrlGetSupportedSinkNoChannels(_L("MM-MMF-OGGCTRL-U-0031-HP"), _L("SectionRecorder"), _L("RecordOgg"), KOggRecordControllerUid));
	AddTestStepL( new(ELeave) RTestStepOggCtrlGetSupportedSinkSampleRates(_L("MM-MMF-OGGCTRL-U-0032-HP"), _L("SectionRecorder"), _L("RecordOgg"), KOggRecordControllerUid));
	AddTestStepL( new(ELeave) RTestStepOggCtrlGetSupportedSinkBitRates(_L("MM-MMF-OGGCTRL-U-0033-HP"), _L("SectionRecorder"), _L("RecordOgg"), KOggRecordControllerUid));
	AddTestStepL( new(ELeave) RTestStepOggCtrlRecordCrop(_L("MM-MMF-OGGCTRL-U-0034-HP"), _L("SectionRecorder"), _L("VorbisStereo44KHz128Kbs"), _L("CropOgg"), KOggRecordControllerUid));
	AddTestStepL( new(ELeave) RTestStepOggCtrlAppend(_L("MM-MMF-OGGCTRL-U-0035-HP"), _L("SectionRecorder"), _L("RecordOgg"), KOggRecordControllerUid));
	AddTestStepL( new(ELeave) RTestStepOggCtrlFailsWithAccessDenied(_L("MM-MMF-OGGCTRL-U-0036-HP"), _L("SectionRecorder"), _L("RecordOgg"), KOggRecordControllerUid));
	AddTestStepL( new(ELeave) RTestStepOggCtrlRecord12(_L("MM-MMF-OGGCTRL-U-0037-HP"), _L("SectionRecorder"), _L("RecordOgg"), KOggRecordControllerUid));
	AddTestStepL( new(ELeave) RTestStepOggCtrlRecord24(_L("MM-MMF-OGGCTRL-U-0038-HP"), _L("SectionRecorder"), _L("RecordOgg"), KOggRecordControllerUid));
	}
/**
 * Initializes Systempath
 */
void CTestSuiteOggCtrl::InitSystemPath()
	{
	_LIT(KTestSourcePathOnC,"c:\\mm\\mmf\\testfiles\\oggctrl\\");
	RFs fs;
	TUint att;
	TInt ret = fs.Connect();
	__ASSERT_ALWAYS(ret == KErrNone,User::Panic(_L("Sys path not setup"),ret));
	if (fs.Att(KTestSourcePathOnC, att) == KErrNone)
		iDefaultPath = KTestSourcePathOnC;
	else
		User::Panic(_L("Test files not found"),KErrNotFound);

	fs.Close();
	}
