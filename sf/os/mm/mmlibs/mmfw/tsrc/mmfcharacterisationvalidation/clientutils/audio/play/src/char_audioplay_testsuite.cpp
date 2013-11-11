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

#include "char_audioplay_testsuite.h"
#include "play_teststep.h"

// Entry function - create a test suite object
EXPORT_C CTestSuiteCharAudioPlay* NewTestSuiteL() 
	{ 
	return CTestSuiteCharAudioPlay::NewL();
	}

CTestSuiteCharAudioPlay* CTestSuiteCharAudioPlay::NewL() 
	{ 
	CTestSuiteCharAudioPlay* self = new (ELeave) CTestSuiteCharAudioPlay;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CTestSuiteCharAudioPlay::CTestSuiteCharAudioPlay()
	{
	iSuiteName = _L("CHAR_AUDIOPLAY");
	}

CTestSuiteCharAudioPlay::~CTestSuiteCharAudioPlay()
	{
	}

void CTestSuiteCharAudioPlay::InitialiseL( void )
	{
	/*************************************************************************************
	*    						Test Suite For WAV Controller
	**************************************************************************************/
	
	AddTestStepL(RTestStepAPOpenFile::NewL(_L("MM-MMF-APLCLNT-CHRTZ-WAV-I-0001"),_L("Normal"), _L("filename")));
	AddTestStepL(RTestStepAPOpenCorruptFile::NewL(_L("MM-MMF-APLCLNT-CHRTZ-WAV-I-0002"),_L("Corrupt"), _L("filename")));
	
	AddTestStepL(RTestStepAPOpenDes::NewL(_L("MM-MMF-APLCLNT-CHRTZ-WAV-I-0006"),_L("Small"), _L("filename")));
	
	AddTestStepL(RTestStepAPPlayAudioFile::NewL(_L("MM-MMF-APLCLNT-CHRTZ-WAV-I-0011"),_L("Normal"), _L("filename")));
	AddTestStepL(RTestStepAPPlayAudioFileWhilePlaying::NewL(_L("MM-MMF-APLCLNT-CHRTZ-WAV-I-0012"),_L("Normal"), _L("filename")));
	AddTestStepL(RTestStepAPPlayAudioFileBeforeOpenClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-WAV-I-0013"),_L("Normal"), _L("filename")));
	
	AddTestStepL(RTestStepAPStopWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-WAV-I-0014"),_L("Normal"), _L("filename")));
	AddTestStepL(RTestStepAPPauseWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-WAV-I-0015"),_L("Normal"), _L("filename")));
	AddTestStepL(RTestStepAPCloseWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-WAV-I-0016"),_L("Normal"), _L("filename")));
	
	AddTestStepL(RTestStepAPGetMaxVolume::NewL(_L("MM-MMF-APLCLNT-CHRTZ-WAV-I-0017"),_L("Normal"), _L("filename")));
	AddTestStepL(RTestStepAPGetMaxVolumeWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-WAV-I-0018"),_L("Normal"), _L("filename")));
	
	AddTestStepL(RTestStepAPGetPlaybackVolume::NewL(_L("MM-MMF-APLCLNT-CHRTZ-WAV-I-0020"),_L("Normal"), _L("filename")));
	AddTestStepL(RTestStepAPGetPlaybackVolumeWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-WAV-I-0021"),_L("Normal"), _L("filename")));
	AddTestStepL(RTestStepAPSetPlaybackVolumeToZero::NewL(_L("MM-MMF-APLCLNT-CHRTZ-WAV-I-0023"),_L("Normal"), _L("filename")));
	AddTestStepL(RTestStepAPSetPlaybackVolumeToMax::NewL(_L("MM-MMF-APLCLNT-CHRTZ-WAV-I-0024"),_L("Normal"), _L("filename")));
	AddTestStepL(RTestStepAPSetPlaybackVolumeToZeroWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-WAV-I-0025"),_L("Normal"), _L("filename")));
	AddTestStepL(RTestStepAPSetPlaybackVolumeToMaxWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-WAV-I-0026"),_L("Normal"), _L("filename")));
	
	AddTestStepL(RTestStepAPSetVolumeRamp::NewL(_L("MM-MMF-APLCLNT-CHRTZ-WAV-I-0029"),_L("Normal"), _L("filename")));
	AddTestStepL(RTestStepAPSetVolumeRampWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-WAV-I-0030"),_L("Normal"), _L("filename")));
	
	AddTestStepL(RTestStepAPSetRepeatsToZero::NewL(_L("MM-MMF-APLCLNT-CHRTZ-WAV-I-0032"),_L("Normal"), _L("filename")));
	AddTestStepL(RTestStepAPSetRepeatsToForever::NewL(_L("MM-MMF-APLCLNT-CHRTZ-WAV-I-0033"),_L("Normal"), _L("filename")));
	AddTestStepL(RTestStepAPSetRepeatsToZeroWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-WAV-I-0034"),_L("Normal"), _L("filename")));
	AddTestStepL(RTestStepAPSetRepeatsToForeverWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-WAV-I-0035"),_L("Normal"), _L("filename")));
	
	AddTestStepL(RTestStepAPSetPriority::NewL(_L("MM-MMF-APLCLNT-CHRTZ-WAV-I-0038"),_L("Normal"), _L("filename")));
	AddTestStepL(RTestStepAPSetPriorityWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-WAV-I-0039"),_L("Normal"), _L("filename")));
	
	AddTestStepL(RTestStepAPGetPosition::NewL(_L("MM-MMF-APLCLNT-CHRTZ-WAV-I-0041"),_L("Normal"), _L("filename")));
	AddTestStepL(RTestStepAPGetPositionWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-WAV-I-0042"),_L("Normal"), _L("filename")));
	AddTestStepL(RTestStepAPSetPosition::NewL(_L("MM-MMF-APLCLNT-CHRTZ-WAV-I-0043"),_L("Normal"), _L("filename")));
	AddTestStepL(RTestStepAPSetPositionNegative::NewL(_L("MM-MMF-APLCLNT-CHRTZ-WAV-I-0044"),_L("Normal"), _L("filename")));
	AddTestStepL(RTestStepAPSetPositionPastDuration::NewL(_L("MM-MMF-APLCLNT-CHRTZ-WAV-I-0045"),_L("Normal"), _L("filename")));
	AddTestStepL(RTestStepAPSetPositionWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-WAV-I-0046"),_L("Normal"), _L("filename")));
	AddTestStepL(RTestStepAPSetPositionNegativeWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-WAV-I-0047"),_L("Normal"), _L("filename")));
	AddTestStepL(RTestStepAPSetPositionPastDurationWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-WAV-I-0048"),_L("Normal"), _L("filename")));
	AddTestStepL(RTestStepAPSetPositionWhileClipPaused::NewL(_L("MM-MMF-APLCLNT-CHRTZ-WAV-I-0049"),_L("Normal"), _L("filename")));
	AddTestStepL(RTestStepAPSetPositionNegativeWhileClipPaused::NewL(_L("MM-MMF-APLCLNT-CHRTZ-WAV-I-0050"),_L("Normal"), _L("filename")));
	AddTestStepL(RTestStepAPSetPositionPastDurationWhileClipPaused::NewL(_L("MM-MMF-APLCLNT-CHRTZ-WAV-I-0051"),_L("Normal"), _L("filename")));
	
	AddTestStepL(RTestStepAPSetPlayWindow::NewL(_L("MM-MMF-APLCLNT-CHRTZ-WAV-I-0052"),_L("Normal"), _L("filename")));
	AddTestStepL(RTestStepAPSetPlayWindowStartAfterHeadWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-WAV-I-0053"),_L("Normal"), _L("filename")));
	AddTestStepL(RTestStepAPSetPlayWindowEndBeforeHeadWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-WAV-I-0054"),_L("Normal"), _L("filename")));
	AddTestStepL(RTestStepAPSetPlayWindowStartAfterEndWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-WAV-I-0055"),_L("Normal"), _L("filename")));
	AddTestStepL(RTestStepAPSetPlayWindowStartAfterHeadWhileClipPaused::NewL(_L("MM-MMF-APLCLNT-CHRTZ-WAV-I-0056"),_L("Normal"), _L("filename")));
	AddTestStepL(RTestStepAPSetPlayWindowEndBeforeHeadWhileClipPaused::NewL(_L("MM-MMF-APLCLNT-CHRTZ-WAV-I-0057"),_L("Normal"), _L("filename")));
	AddTestStepL(RTestStepAPSetPlayWindowStartAfterEndWhileClipPaused::NewL(_L("MM-MMF-APLCLNT-CHRTZ-WAV-I-0058"),_L("Normal"), _L("filename")));
	
	AddTestStepL(RTestStepAPClearPlayWindow::NewL(_L("MM-MMF-APLCLNT-CHRTZ-WAV-I-0059"),_L("Normal"), _L("filename")));
	AddTestStepL(RTestStepAPClearPlayWindowWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-WAV-I-0060"),_L("Normal"), _L("filename")));
	
	AddTestStepL(RTestStepAPSetBalance::NewL(_L("MM-MMF-APLCLNT-CHRTZ-WAV-I-0062"),_L("Normal"), _L("filename")));
	AddTestStepL(RTestStepAPSetBalanceLessMaxLeft::NewL(_L("MM-MMF-APLCLNT-CHRTZ-WAV-I-0063"),_L("Normal"), _L("filename")));
	AddTestStepL(RTestStepAPSetBalanceGreaterMaxRight::NewL(_L("MM-MMF-APLCLNT-CHRTZ-WAV-I-0064"),_L("Normal"), _L("filename")));
	AddTestStepL(RTestStepAPSetBalanceWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-WAV-I-0065"),_L("Normal"), _L("filename")));
	AddTestStepL(RTestStepAPSetBalanceLessMaxLeftWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-WAV-I-0066"),_L("Normal"), _L("filename")));
	AddTestStepL(RTestStepAPSetBalanceGreaterMaxRightWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-WAV-I-0067"),_L("Normal"), _L("filename")));
	AddTestStepL(RTestStepAPSetBalanceWhileClipPaused::NewL(_L("MM-MMF-APLCLNT-CHRTZ-WAV-I-0068"),_L("Normal"), _L("filename")));
	AddTestStepL(RTestStepAPSetBalanceLessMaxLeftWhileClipPaused::NewL(_L("MM-MMF-APLCLNT-CHRTZ-WAV-I-0069"),_L("Normal"), _L("filename")));
	AddTestStepL(RTestStepAPSetBalanceGreaterMaxRightWhileClipPaused::NewL(_L("MM-MMF-APLCLNT-CHRTZ-WAV-I-0070"),_L("Normal"), _L("filename")));
	
	AddTestStepL(RTestStepAPGetBalance::NewL(_L("MM-MMF-APLCLNT-CHRTZ-WAV-I-0072"),_L("Normal"), _L("filename")));
	AddTestStepL(RTestStepAPGetBalanceWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-WAV-I-0073"),_L("Normal"), _L("filename")));
	
	AddTestStepL(RTestStepAPGetBitrate::NewL(_L("MM-MMF-APLCLNT-CHRTZ-WAV-I-0075"),_L("Normal"), _L("filename")));
	AddTestStepL(RTestStepAPGetBitrateWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-WAV-I-0076"),_L("Normal"), _L("filename")));
	
	AddTestStepL(RTestStepAPRegisterEventNotification::NewL(_L("MM-MMF-APLCLNT-CHRTZ-WAV-I-0078"),_L("Normal"), _L("filename")));
	AddTestStepL(RTestStepAPRegisterEventNotificationWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-WAV-I-0078"),_L("Normal"), _L("filename")));
	AddTestStepL(RTestStepAPRegisterEventNotificationWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-WAV-I-0079"),_L("Normal"), _L("filename")));
	AddTestStepL(RTestStepAPRegisterEventNotificationBeforeOpenClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-WAV-I-0080"),_L("Normal"), _L("filename")));
	AddTestStepL(RTestStepAPRegisterEventNotificationUidNotSupported::NewL(_L("MM-MMF-APLCLNT-CHRTZ-WAV-I-0082"),_L("Normal"), _L("filename")));
	AddTestStepL(RTestStepAPRegisterEventNotificationUidAlreadyRegistered::NewL(_L("MM-MMF-APLCLNT-CHRTZ-WAV-I-0083"),_L("Normal"), _L("filename")));
	
	AddTestStepL(RTestStepAPCancelRegisteredNotificationEvent::NewL(_L("MM-MMF-APLCLNT-CHRTZ-WAV-I-0084"),_L("Normal"), _L("filename")));
	AddTestStepL(RTestStepAPCancelRegisteredNotificationEventWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-WAV-I-0085"),_L("Normal"), _L("filename")));
	AddTestStepL(RTestStepAPCancelRegisteredNotificationEventUidNotMatched::NewL(_L("MM-MMF-APLCLNT-CHRTZ-WAV-I-0086"),_L("Normal"), _L("filename")));
	AddTestStepL(RTestStepAPCancelRegisteredNotificationEventUidNULL::NewL(_L("MM-MMF-APLCLNT-CHRTZ-WAV-I-0087"),_L("Normal"), _L("filename")));
	
	AddTestStepL(RTestStepAPGetNumberOfMetaDataEntries::NewL(_L("MM-MMF-APLCLNT-CHRTZ-WAV-I-0089"),_L("Normal"), _L("filename"), KErrNotSupported));
	AddTestStepL(RTestStepAPGetMetaDataEntry::NewL(_L("MM-MMF-APLCLNT-CHRTZ-WAV-I-0090"),_L("Normal"), _L("filename"), KErrNotSupported));
	AddTestStepL(RTestStepAPGetNumberOfMetaDataEntriesWhilePlaying::NewL(_L("MM-MMF-APLCLNT-CHRTZ-WAV-I-0091"),_L("Normal"), _L("filename"), KErrNotReady));
	AddTestStepL(RTestStepAPGetMetaDataEntryWhilePlaying::NewL(_L("MM-MMF-APLCLNT-CHRTZ-WAV-I-0092"),_L("Normal"), _L("filename"), KErrNotReady));
	AddTestStepL(RTestStepAPGetNumberOfMetaDataEntriesBeforeOpenClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-WAV-I-0093"),_L("Normal"), _L("filename")));
	AddTestStepL(RTestStepAPGetMetaDataEntryBeforeOpenClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-WAV-I-0094"),_L("Normal"), _L("filename")));
	
	AddTestStepL(RTestStepAPPlayPausePlay::NewL(_L("MM-MMF-APLCLNT-CHRTZ-WAV-I-0095"),_L("Normal"), _L("filename")));
	
	/*************************************************************************************
	*    						Test Suite For OGG Controller
	**************************************************************************************/
	
	AddTestStepL(RTestStepAPOpenFile::NewL(_L("MM-MMF-APLCLNT-CHRTZ-OGG-I-0001"),_L("NormalOgg"), _L("filename")));
	AddTestStepL(RTestStepAPOpenCorruptFile::NewL(_L("MM-MMF-APLCLNT-CHRTZ-OGG-I-0002"),_L("CorruptOgg"), _L("filename")));
	
	AddTestStepL(RTestStepAPOpenDes::NewL(_L("MM-MMF-APLCLNT-CHRTZ-OGG-I-0006"),_L("SmallOgg"), _L("filename")));
	
	AddTestStepL(RTestStepAPPlayAudioFile::NewL(_L("MM-MMF-APLCLNT-CHRTZ-OGG-I-0011"),_L("NormalOgg"), _L("filename")));
	AddTestStepL(RTestStepAPPlayAudioFileWhilePlaying::NewL(_L("MM-MMF-APLCLNT-CHRTZ-OGG-I-0012"),_L("NormalOgg"), _L("filename")));
	AddTestStepL(RTestStepAPPlayAudioFileBeforeOpenClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-OGG-I-0013"),_L("NormalOgg"), _L("filename")));
	
	AddTestStepL(RTestStepAPStopWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-OGG-I-0014"),_L("NormalOgg"), _L("filename")));
	AddTestStepL(RTestStepAPPauseWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-OGG-I-0015"),_L("NormalOgg"), _L("filename")));
	AddTestStepL(RTestStepAPCloseWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-OGG-I-0016"),_L("NormalOgg"), _L("filename")));
	
	AddTestStepL(RTestStepAPGetMaxVolume::NewL(_L("MM-MMF-APLCLNT-CHRTZ-OGG-I-0017"),_L("NormalOgg"), _L("filename")));
	AddTestStepL(RTestStepAPGetMaxVolumeWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-OGG-I-0018"),_L("NormalOgg"), _L("filename")));
	
	AddTestStepL(RTestStepAPGetPlaybackVolume::NewL(_L("MM-MMF-APLCLNT-CHRTZ-OGG-I-0020"),_L("NormalOgg"), _L("filename")));
	AddTestStepL(RTestStepAPGetPlaybackVolumeWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-OGG-I-0021"),_L("NormalOgg"), _L("filename")));
	AddTestStepL(RTestStepAPSetPlaybackVolumeToZero::NewL(_L("MM-MMF-APLCLNT-CHRTZ-OGG-I-0023"),_L("NormalOgg"), _L("filename")));
	AddTestStepL(RTestStepAPSetPlaybackVolumeToMax::NewL(_L("MM-MMF-APLCLNT-CHRTZ-OGG-I-0024"),_L("NormalOgg"), _L("filename")));
	AddTestStepL(RTestStepAPSetPlaybackVolumeToZeroWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-OGG-I-0025"),_L("NormalOgg"), _L("filename")));
	AddTestStepL(RTestStepAPSetPlaybackVolumeToMaxWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-OGG-I-0026"),_L("NormalOgg"), _L("filename")));
	
	AddTestStepL(RTestStepAPSetVolumeRamp::NewL(_L("MM-MMF-APLCLNT-CHRTZ-OGG-I-0029"),_L("NormalOgg"), _L("filename")));
	AddTestStepL(RTestStepAPSetVolumeRampWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-OGG-I-0030"),_L("NormalOgg"), _L("filename")));
	
	AddTestStepL(RTestStepAPSetRepeatsToZero::NewL(_L("MM-MMF-APLCLNT-CHRTZ-OGG-I-0032"),_L("NormalOgg"), _L("filename")));
	AddTestStepL(RTestStepAPSetRepeatsToForever::NewL(_L("MM-MMF-APLCLNT-CHRTZ-OGG-I-0033"),_L("NormalOgg"), _L("filename")));
	AddTestStepL(RTestStepAPSetRepeatsToZeroWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-OGG-I-0034"),_L("NormalOgg"), _L("filename")));
	AddTestStepL(RTestStepAPSetRepeatsToForeverWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-OGG-I-0035"),_L("NormalOgg"), _L("filename")));
	
	AddTestStepL(RTestStepAPSetPriority::NewL(_L("MM-MMF-APLCLNT-CHRTZ-OGG-I-0038"),_L("NormalOgg"), _L("filename")));
	AddTestStepL(RTestStepAPSetPriorityWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-OGG-I-0039"),_L("NormalOgg"), _L("filename")));
	
	AddTestStepL(RTestStepAPGetPosition::NewL(_L("MM-MMF-APLCLNT-CHRTZ-OGG-I-0041"),_L("NormalOgg"), _L("filename")));
	AddTestStepL(RTestStepAPGetPositionWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-OGG-I-0042"),_L("NormalOgg"), _L("filename")));
	AddTestStepL(RTestStepAPSetPosition::NewL(_L("MM-MMF-APLCLNT-CHRTZ-OGG-I-0043"),_L("NormalOgg"), _L("filename")));
	AddTestStepL(RTestStepAPSetPositionNegative::NewL(_L("MM-MMF-APLCLNT-CHRTZ-OGG-I-0044"),_L("NormalOgg"), _L("filename")));
	AddTestStepL(RTestStepAPSetPositionPastDuration::NewL(_L("MM-MMF-APLCLNT-CHRTZ-OGG-I-0045"),_L("NormalOgg"), _L("filename")));
	AddTestStepL(RTestStepAPSetPositionWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-OGG-I-0046"),_L("NormalOgg"), _L("filename")));
	AddTestStepL(RTestStepAPSetPositionNegativeWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-OGG-I-0047"),_L("NormalOgg"), _L("filename")));
	AddTestStepL(RTestStepAPSetPositionPastDurationWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-OGG-I-0048"),_L("NormalOgg"), _L("filename")));
	AddTestStepL(RTestStepAPSetPositionWhileClipPaused::NewL(_L("MM-MMF-APLCLNT-CHRTZ-OGG-I-0049"),_L("NormalOgg"), _L("filename")));
	AddTestStepL(RTestStepAPSetPositionNegativeWhileClipPaused::NewL(_L("MM-MMF-APLCLNT-CHRTZ-OGG-I-0050"),_L("NormalOgg"), _L("filename")));
	AddTestStepL(RTestStepAPSetPositionPastDurationWhileClipPaused::NewL(_L("MM-MMF-APLCLNT-CHRTZ-OGG-I-0051"),_L("NormalOgg"), _L("filename")));
	
	AddTestStepL(RTestStepAPSetPlayWindow::NewL(_L("MM-MMF-APLCLNT-CHRTZ-OGG-I-0052"),_L("NormalOgg"), _L("filename")));
	AddTestStepL(RTestStepAPSetPlayWindowStartAfterHeadWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-OGG-I-0053"),_L("NormalOgg"), _L("filename")));
	AddTestStepL(RTestStepAPSetPlayWindowEndBeforeHeadWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-OGG-I-0054"),_L("NormalOgg"), _L("filename")));
	AddTestStepL(RTestStepAPSetPlayWindowStartAfterEndWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-OGG-I-0055"),_L("NormalOgg"), _L("filename")));
	AddTestStepL(RTestStepAPSetPlayWindowStartAfterHeadWhileClipPaused::NewL(_L("MM-MMF-APLCLNT-CHRTZ-OGG-I-0056"),_L("NormalOgg"), _L("filename")));
	AddTestStepL(RTestStepAPSetPlayWindowEndBeforeHeadWhileClipPaused::NewL(_L("MM-MMF-APLCLNT-CHRTZ-OGG-I-0057"),_L("NormalOgg"), _L("filename")));
	AddTestStepL(RTestStepAPSetPlayWindowStartAfterEndWhileClipPaused::NewL(_L("MM-MMF-APLCLNT-CHRTZ-OGG-I-0058"),_L("NormalOgg"), _L("filename")));
	
	AddTestStepL(RTestStepAPClearPlayWindow::NewL(_L("MM-MMF-APLCLNT-CHRTZ-OGG-I-0059"),_L("NormalOgg"), _L("filename")));
	AddTestStepL(RTestStepAPClearPlayWindowWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-OGG-I-0060"),_L("NormalOgg"), _L("filename")));
	
	AddTestStepL(RTestStepAPSetBalance::NewL(_L("MM-MMF-APLCLNT-CHRTZ-OGG-I-0062"),_L("NormalOgg"), _L("filename")));
	AddTestStepL(RTestStepAPSetBalanceLessMaxLeft::NewL(_L("MM-MMF-APLCLNT-CHRTZ-OGG-I-0063"),_L("NormalOgg"), _L("filename")));
	AddTestStepL(RTestStepAPSetBalanceGreaterMaxRight::NewL(_L("MM-MMF-APLCLNT-CHRTZ-OGG-I-0064"),_L("NormalOgg"), _L("filename")));
	AddTestStepL(RTestStepAPSetBalanceWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-OGG-I-0065"),_L("NormalOgg"), _L("filename")));
	AddTestStepL(RTestStepAPSetBalanceLessMaxLeftWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-OGG-I-0066"),_L("NormalOgg"), _L("filename")));
	AddTestStepL(RTestStepAPSetBalanceGreaterMaxRightWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-OGG-I-0067"),_L("NormalOgg"), _L("filename")));
	AddTestStepL(RTestStepAPSetBalanceWhileClipPaused::NewL(_L("MM-MMF-APLCLNT-CHRTZ-OGG-I-0068"),_L("NormalOgg"), _L("filename")));
	AddTestStepL(RTestStepAPSetBalanceLessMaxLeftWhileClipPaused::NewL(_L("MM-MMF-APLCLNT-CHRTZ-OGG-I-0069"),_L("NormalOgg"), _L("filename")));
	AddTestStepL(RTestStepAPSetBalanceGreaterMaxRightWhileClipPaused::NewL(_L("MM-MMF-APLCLNT-CHRTZ-OGG-I-0070"),_L("NormalOgg"), _L("filename")));
	
	AddTestStepL(RTestStepAPGetBalance::NewL(_L("MM-MMF-APLCLNT-CHRTZ-OGG-I-0072"),_L("NormalOgg"), _L("filename")));
	AddTestStepL(RTestStepAPGetBalanceWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-OGG-I-0073"),_L("NormalOgg"), _L("filename")));
	
	AddTestStepL(RTestStepAPGetBitrate::NewL(_L("MM-MMF-APLCLNT-CHRTZ-OGG-I-0075"),_L("NormalOgg"), _L("filename")));
	AddTestStepL(RTestStepAPGetBitrateWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-OGG-I-0076"),_L("NormalOgg"), _L("filename")));
	
	AddTestStepL(RTestStepAPRegisterEventNotification::NewL(_L("MM-MMF-APLCLNT-CHRTZ-OGG-I-0078"),_L("NormalOgg"), _L("filename")));
	AddTestStepL(RTestStepAPRegisterEventNotificationWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-OGG-I-0078"),_L("NormalOgg"), _L("filename")));
	AddTestStepL(RTestStepAPRegisterEventNotificationWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-OGG-I-0079"),_L("NormalOgg"), _L("filename")));
	AddTestStepL(RTestStepAPRegisterEventNotificationBeforeOpenClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-OGG-I-0080"),_L("NormalOgg"), _L("filename")));
	AddTestStepL(RTestStepAPRegisterEventNotificationUidNotSupported::NewL(_L("MM-MMF-APLCLNT-CHRTZ-OGG-I-0082"),_L("NormalOgg"), _L("filename")));
	AddTestStepL(RTestStepAPRegisterEventNotificationUidAlreadyRegistered::NewL(_L("MM-MMF-APLCLNT-CHRTZ-OGG-I-0083"),_L("NormalOgg"), _L("filename")));
	
	AddTestStepL(RTestStepAPCancelRegisteredNotificationEvent::NewL(_L("MM-MMF-APLCLNT-CHRTZ-OGG-I-0084"),_L("NormalOgg"), _L("filename")));
	AddTestStepL(RTestStepAPCancelRegisteredNotificationEventWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-OGG-I-0085"),_L("NormalOgg"), _L("filename")));
	AddTestStepL(RTestStepAPCancelRegisteredNotificationEventUidNotMatched::NewL(_L("MM-MMF-APLCLNT-CHRTZ-OGG-I-0086"),_L("NormalOgg"), _L("filename")));
	AddTestStepL(RTestStepAPCancelRegisteredNotificationEventUidNULL::NewL(_L("MM-MMF-APLCLNT-CHRTZ-OGG-I-0087"),_L("NormalOgg"), _L("filename")));
	
	AddTestStepL(RTestStepAPGetNumberOfMetaDataEntries::NewL(_L("MM-MMF-APLCLNT-CHRTZ-OGG-I-0089"),_L("MetedataOgg"), _L("filename"), KErrNone));
	AddTestStepL(RTestStepAPGetMetaDataEntry::NewL(_L("MM-MMF-APLCLNT-CHRTZ-OGG-I-0090"),_L("MetedataOgg"), _L("filename"), KErrNone));
	AddTestStepL(RTestStepAPGetNumberOfMetaDataEntriesWhilePlaying::NewL(_L("MM-MMF-APLCLNT-CHRTZ-OGG-I-0091"),_L("MetedataOgg"), _L("filename"), KErrNone));
	AddTestStepL(RTestStepAPGetMetaDataEntryWhilePlaying::NewL(_L("MM-MMF-APLCLNT-CHRTZ-OGG-I-0092"),_L("MetedataOgg"), _L("filename"), KErrNone));
	AddTestStepL(RTestStepAPGetNumberOfMetaDataEntriesBeforeOpenClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-OGG-I-0093"),_L("MetedataOgg"), _L("filename")));
	AddTestStepL(RTestStepAPGetMetaDataEntryBeforeOpenClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-OGG-I-0094"),_L("MetedataOgg"), _L("filename")));
	
	AddTestStepL(RTestStepAPPlayPausePlay::NewL(_L("MM-MMF-APLCLNT-CHRTZ-OGG-I-0095"),_L("NormalOgg"), _L("filename")));

#ifdef SYMBIAN_MULTIMEDIA_A3FDEVSOUND

	/*************************************************************************************
	*    						Test Suite For AMR Controller
	**************************************************************************************/
	
	AddTestStepL(RTestStepAPOpenFile::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AMR-I-0001"),_L("NormalAmr"), _L("filename")));
	AddTestStepL(RTestStepAPOpenCorruptFile::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AMR-I-0002"),_L("CorruptAmr"), _L("filename")));
	
	AddTestStepL(RTestStepAPOpenDes::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AMR-I-0006"),_L("SmallAmr"), _L("filename")));
	
	AddTestStepL(RTestStepAPPlayAudioFile::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AMR-I-0011"),_L("NormalAmr"), _L("filename")));
	AddTestStepL(RTestStepAPPlayAudioFileWhilePlaying::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AMR-I-0012"),_L("NormalAmr"), _L("filename")));
	AddTestStepL(RTestStepAPPlayAudioFileBeforeOpenClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AMR-I-0013"),_L("NormalAmr"), _L("filename")));
	
	AddTestStepL(RTestStepAPStopWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AMR-I-0014"),_L("NormalAmr"), _L("filename")));
	AddTestStepL(RTestStepAPPauseWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AMR-I-0015"),_L("NormalAmr"), _L("filename")));
	AddTestStepL(RTestStepAPCloseWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AMR-I-0016"),_L("NormalAmr"), _L("filename")));
	
	AddTestStepL(RTestStepAPGetMaxVolume::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AMR-I-0017"),_L("NormalAmr"), _L("filename")));
	AddTestStepL(RTestStepAPGetMaxVolumeWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AMR-I-0018"),_L("NormalAmr"), _L("filename")));
	
	AddTestStepL(RTestStepAPGetPlaybackVolume::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AMR-I-0020"),_L("NormalAmr"), _L("filename")));
	AddTestStepL(RTestStepAPGetPlaybackVolumeWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AMR-I-0021"),_L("NormalAmr"), _L("filename")));
	AddTestStepL(RTestStepAPSetPlaybackVolumeToZero::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AMR-I-0023"),_L("NormalAmr"), _L("filename")));
	AddTestStepL(RTestStepAPSetPlaybackVolumeToMax::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AMR-I-0024"),_L("NormalAmr"), _L("filename")));
	AddTestStepL(RTestStepAPSetPlaybackVolumeToZeroWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AMR-I-0025"),_L("NormalAmr"), _L("filename")));
	AddTestStepL(RTestStepAPSetPlaybackVolumeToMaxWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AMR-I-0026"),_L("NormalAmr"), _L("filename")));
	
	AddTestStepL(RTestStepAPSetVolumeRamp::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AMR-I-0029"),_L("NormalAmr"), _L("filename")));
	AddTestStepL(RTestStepAPSetVolumeRampWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AMR-I-0030"),_L("NormalAmr"), _L("filename")));
	
	AddTestStepL(RTestStepAPSetRepeatsToZero::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AMR-I-0032"),_L("NormalAmr"), _L("filename")));
	AddTestStepL(RTestStepAPSetRepeatsToForever::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AMR-I-0033"),_L("NormalAmr"), _L("filename")));
	AddTestStepL(RTestStepAPSetRepeatsToZeroWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AMR-I-0034"),_L("NormalAmr"), _L("filename")));
	AddTestStepL(RTestStepAPSetRepeatsToForeverWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AMR-I-0035"),_L("NormalAmr"), _L("filename")));
	
	AddTestStepL(RTestStepAPSetPriority::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AMR-I-0038"),_L("NormalAmr"), _L("filename")));
	AddTestStepL(RTestStepAPSetPriorityWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AMR-I-0039"),_L("NormalAmr"), _L("filename")));
	
	AddTestStepL(RTestStepAPGetPosition::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AMR-I-0041"),_L("NormalAmr"), _L("filename")));
	AddTestStepL(RTestStepAPGetPositionWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AMR-I-0042"),_L("NormalAmr"), _L("filename")));
	AddTestStepL(RTestStepAPSetPosition::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AMR-I-0043"),_L("NormalAmr"), _L("filename")));
	AddTestStepL(RTestStepAPSetPositionNegative::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AMR-I-0044"),_L("NormalAmr"), _L("filename")));
	AddTestStepL(RTestStepAPSetPositionPastDuration::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AMR-I-0045"),_L("NormalAmr"), _L("filename")));
	AddTestStepL(RTestStepAPSetPositionWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AMR-I-0046"),_L("NormalAmr"), _L("filename")));
	AddTestStepL(RTestStepAPSetPositionNegativeWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AMR-I-0047"),_L("NormalAmr"), _L("filename")));
	AddTestStepL(RTestStepAPSetPositionPastDurationWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AMR-I-0048"),_L("NormalAmr"), _L("filename")));
	AddTestStepL(RTestStepAPSetPositionWhileClipPaused::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AMR-I-0049"),_L("NormalAmr"), _L("filename")));
	AddTestStepL(RTestStepAPSetPositionNegativeWhileClipPaused::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AMR-I-0050"),_L("NormalAmr"), _L("filename")));
	AddTestStepL(RTestStepAPSetPositionPastDurationWhileClipPaused::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AMR-I-0051"),_L("NormalAmr"), _L("filename")));
	
	AddTestStepL(RTestStepAPSetPlayWindow::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AMR-I-0052"),_L("NormalAmr"), _L("filename")));
	AddTestStepL(RTestStepAPSetPlayWindowStartAfterHeadWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AMR-I-0053"),_L("NormalAmr"), _L("filename")));
	AddTestStepL(RTestStepAPSetPlayWindowEndBeforeHeadWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AMR-I-0054"),_L("NormalAmr"), _L("filename")));
	AddTestStepL(RTestStepAPSetPlayWindowStartAfterEndWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AMR-I-0055"),_L("NormalAmr"), _L("filename")));
	AddTestStepL(RTestStepAPSetPlayWindowStartAfterHeadWhileClipPaused::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AMR-I-0056"),_L("NormalAmr"), _L("filename")));
	AddTestStepL(RTestStepAPSetPlayWindowEndBeforeHeadWhileClipPaused::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AMR-I-0057"),_L("NormalAmr"), _L("filename")));
	AddTestStepL(RTestStepAPSetPlayWindowStartAfterEndWhileClipPaused::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AMR-I-0058"),_L("NormalAmr"), _L("filename")));
	
	AddTestStepL(RTestStepAPClearPlayWindow::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AMR-I-0059"),_L("NormalAmr"), _L("filename")));
	AddTestStepL(RTestStepAPClearPlayWindowWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AMR-I-0060"),_L("NormalAmr"), _L("filename")));
	
	AddTestStepL(RTestStepAPSetBalance::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AMR-I-0062"),_L("NormalAmr"), _L("filename")));
	AddTestStepL(RTestStepAPSetBalanceLessMaxLeft::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AMR-I-0063"),_L("NormalAmr"), _L("filename")));
	AddTestStepL(RTestStepAPSetBalanceGreaterMaxRight::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AMR-I-0064"),_L("NormalAmr"), _L("filename")));
	AddTestStepL(RTestStepAPSetBalanceWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AMR-I-0065"),_L("NormalAmr"), _L("filename")));
	AddTestStepL(RTestStepAPSetBalanceLessMaxLeftWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AMR-I-0066"),_L("NormalAmr"), _L("filename")));
	AddTestStepL(RTestStepAPSetBalanceGreaterMaxRightWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AMR-I-0067"),_L("NormalAmr"), _L("filename")));
	AddTestStepL(RTestStepAPSetBalanceWhileClipPaused::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AMR-I-0068"),_L("NormalAmr"), _L("filename")));
	AddTestStepL(RTestStepAPSetBalanceLessMaxLeftWhileClipPaused::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AMR-I-0069"),_L("NormalAmr"), _L("filename")));
	AddTestStepL(RTestStepAPSetBalanceGreaterMaxRightWhileClipPaused::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AMR-I-0070"),_L("NormalAmr"), _L("filename")));
	
	AddTestStepL(RTestStepAPGetBalance::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AMR-I-0072"),_L("NormalAmr"), _L("filename")));
	AddTestStepL(RTestStepAPGetBalanceWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AMR-I-0073"),_L("NormalAmr"), _L("filename")));
	
	AddTestStepL(RTestStepAPGetBitrate::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AMR-I-0075"),_L("NormalAmr"), _L("filename")));
	AddTestStepL(RTestStepAPGetBitrateWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AMR-I-0076"),_L("NormalAmr"), _L("filename")));
	
	AddTestStepL(RTestStepAPRegisterEventNotification::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AMR-I-0078"),_L("NormalAmr"), _L("filename")));
	AddTestStepL(RTestStepAPRegisterEventNotificationWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AMR-I-0078"),_L("NormalAmr"), _L("filename")));
	AddTestStepL(RTestStepAPRegisterEventNotificationWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AMR-I-0079"),_L("NormalAmr"), _L("filename")));
	AddTestStepL(RTestStepAPRegisterEventNotificationBeforeOpenClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AMR-I-0080"),_L("NormalAmr"), _L("filename")));
	AddTestStepL(RTestStepAPRegisterEventNotificationUidNotSupported::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AMR-I-0082"),_L("NormalAmr"), _L("filename")));

	AddTestStepL(RTestStepAPRegisterEventNotificationUidAlreadyRegistered::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AMR-I-0083"),_L("NormalAmr"), _L("filename")));
	
	AddTestStepL(RTestStepAPCancelRegisteredNotificationEvent::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AMR-I-0084"),_L("NormalAmr"), _L("filename")));
	AddTestStepL(RTestStepAPCancelRegisteredNotificationEventWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AMR-I-0085"),_L("NormalAmr"), _L("filename")));
	AddTestStepL(RTestStepAPCancelRegisteredNotificationEventUidNotMatched::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AMR-I-0086"),_L("NormalAmr"), _L("filename")));
	AddTestStepL(RTestStepAPCancelRegisteredNotificationEventUidNULL::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AMR-I-0087"),_L("NormalAmr"), _L("filename")));
	
	AddTestStepL(RTestStepAPGetNumberOfMetaDataEntries::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AMR-I-0089"),_L("MetadataAmr"), _L("filename"), KErrNone));
	AddTestStepL(RTestStepAPGetMetaDataEntry::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AMR-I-0090"),_L("MetadataAmr"), _L("filename"), KErrNone));
	AddTestStepL(RTestStepAPGetNumberOfMetaDataEntriesWhilePlaying::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AMR-I-0091"),_L("MetadataAmr"), _L("filename"), KErrNone));
	AddTestStepL(RTestStepAPGetMetaDataEntryWhilePlaying::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AMR-I-0092"),_L("MetadataAmr"), _L("filename"), KErrNone));
	AddTestStepL(RTestStepAPGetNumberOfMetaDataEntriesBeforeOpenClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AMR-I-0093"),_L("MetadataAmr"), _L("filename")));
	AddTestStepL(RTestStepAPGetMetaDataEntryBeforeOpenClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AMR-I-0094"),_L("MetadataAmr"), _L("filename")));
	
	AddTestStepL(RTestStepAPPlayPausePlay::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AMR-I-0095"),_L("NormalAmr"), _L("filename")));

	/*************************************************************************************
	*    						Test Suite For MP3 Controller
	**************************************************************************************/
	
	AddTestStepL(RTestStepAPOpenFile::NewL(_L("MM-MMF-APLCLNT-CHRTZ-MP3-I-0001"),_L("NormalMp3"), _L("filename")));
	AddTestStepL(RTestStepAPOpenCorruptFile::NewL(_L("MM-MMF-APLCLNT-CHRTZ-MP3-I-0002"),_L("CorruptMp3"), _L("filename")));
	
	AddTestStepL(RTestStepAPOpenDes::NewL(_L("MM-MMF-APLCLNT-CHRTZ-MP3-I-0006"),_L("SmallMp3"), _L("filename")));
	
	AddTestStepL(RTestStepAPPlayAudioFile::NewL(_L("MM-MMF-APLCLNT-CHRTZ-MP3-I-0011"),_L("NormalMp3"), _L("filename")));
	AddTestStepL(RTestStepAPPlayAudioFileWhilePlaying::NewL(_L("MM-MMF-APLCLNT-CHRTZ-MP3-I-0012"),_L("NormalMp3"), _L("filename")));
	AddTestStepL(RTestStepAPPlayAudioFileBeforeOpenClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-MP3-I-0013"),_L("NormalMp3"), _L("filename")));
	
	AddTestStepL(RTestStepAPStopWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-MP3-I-0014"),_L("NormalMp3"), _L("filename")));
	AddTestStepL(RTestStepAPPauseWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-MP3-I-0015"),_L("NormalMp3"), _L("filename")));
	AddTestStepL(RTestStepAPCloseWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-MP3-I-0016"),_L("NormalMp3"), _L("filename")));
	
	AddTestStepL(RTestStepAPGetMaxVolume::NewL(_L("MM-MMF-APLCLNT-CHRTZ-MP3-I-0017"),_L("NormalMp3"), _L("filename")));
	AddTestStepL(RTestStepAPGetMaxVolumeWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-MP3-I-0018"),_L("NormalMp3"), _L("filename")));
	
	AddTestStepL(RTestStepAPGetPlaybackVolume::NewL(_L("MM-MMF-APLCLNT-CHRTZ-MP3-I-0020"),_L("NormalMp3"), _L("filename")));
	AddTestStepL(RTestStepAPGetPlaybackVolumeWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-MP3-I-0021"),_L("NormalMp3"), _L("filename")));
	AddTestStepL(RTestStepAPSetPlaybackVolumeToZero::NewL(_L("MM-MMF-APLCLNT-CHRTZ-MP3-I-0023"),_L("NormalMp3"), _L("filename")));
	AddTestStepL(RTestStepAPSetPlaybackVolumeToMax::NewL(_L("MM-MMF-APLCLNT-CHRTZ-MP3-I-0024"),_L("NormalMp3"), _L("filename")));
	AddTestStepL(RTestStepAPSetPlaybackVolumeToZeroWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-MP3-I-0025"),_L("NormalMp3"), _L("filename")));
	AddTestStepL(RTestStepAPSetPlaybackVolumeToMaxWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-MP3-I-0026"),_L("NormalMp3"), _L("filename")));
	
	AddTestStepL(RTestStepAPSetVolumeRamp::NewL(_L("MM-MMF-APLCLNT-CHRTZ-MP3-I-0029"),_L("NormalMp3"), _L("filename")));
	AddTestStepL(RTestStepAPSetVolumeRampWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-MP3-I-0030"),_L("NormalMp3"), _L("filename")));
	
	AddTestStepL(RTestStepAPSetRepeatsToZero::NewL(_L("MM-MMF-APLCLNT-CHRTZ-MP3-I-0032"),_L("NormalMp3"), _L("filename")));
	AddTestStepL(RTestStepAPSetRepeatsToForever::NewL(_L("MM-MMF-APLCLNT-CHRTZ-MP3-I-0033"),_L("NormalMp3"), _L("filename")));
	AddTestStepL(RTestStepAPSetRepeatsToZeroWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-MP3-I-0034"),_L("NormalMp3"), _L("filename")));
	AddTestStepL(RTestStepAPSetRepeatsToForeverWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-MP3-I-0035"),_L("NormalMp3"), _L("filename")));
	
	AddTestStepL(RTestStepAPSetPriority::NewL(_L("MM-MMF-APLCLNT-CHRTZ-MP3-I-0038"),_L("NormalMp3"), _L("filename")));
	AddTestStepL(RTestStepAPSetPriorityWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-MP3-I-0039"),_L("NormalMp3"), _L("filename")));
	
	AddTestStepL(RTestStepAPGetPosition::NewL(_L("MM-MMF-APLCLNT-CHRTZ-MP3-I-0041"),_L("NormalMp3"), _L("filename")));
	AddTestStepL(RTestStepAPGetPositionWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-MP3-I-0042"),_L("NormalMp3"), _L("filename")));
	AddTestStepL(RTestStepAPSetPosition::NewL(_L("MM-MMF-APLCLNT-CHRTZ-MP3-I-0043"),_L("NormalMp3"), _L("filename")));
	AddTestStepL(RTestStepAPSetPositionNegative::NewL(_L("MM-MMF-APLCLNT-CHRTZ-MP3-I-0044"),_L("NormalMp3"), _L("filename")));
	AddTestStepL(RTestStepAPSetPositionPastDuration::NewL(_L("MM-MMF-APLCLNT-CHRTZ-MP3-I-0045"),_L("NormalMp3"), _L("filename")));
	AddTestStepL(RTestStepAPSetPositionWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-MP3-I-0046"),_L("NormalMp3"), _L("filename")));
	AddTestStepL(RTestStepAPSetPositionNegativeWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-MP3-I-0047"),_L("NormalMp3"), _L("filename")));
	AddTestStepL(RTestStepAPSetPositionPastDurationWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-MP3-I-0048"),_L("NormalMp3"), _L("filename")));
	AddTestStepL(RTestStepAPSetPositionWhileClipPaused::NewL(_L("MM-MMF-APLCLNT-CHRTZ-MP3-I-0049"),_L("NormalMp3"), _L("filename")));
	AddTestStepL(RTestStepAPSetPositionNegativeWhileClipPaused::NewL(_L("MM-MMF-APLCLNT-CHRTZ-MP3-I-0050"),_L("NormalMp3"), _L("filename")));
	AddTestStepL(RTestStepAPSetPositionPastDurationWhileClipPaused::NewL(_L("MM-MMF-APLCLNT-CHRTZ-MP3-I-0051"),_L("NormalMp3"), _L("filename")));
	
	AddTestStepL(RTestStepAPSetPlayWindow::NewL(_L("MM-MMF-APLCLNT-CHRTZ-MP3-I-0052"),_L("NormalMp3"), _L("filename")));
	AddTestStepL(RTestStepAPSetPlayWindowStartAfterHeadWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-MP3-I-0053"),_L("NormalMp3"), _L("filename")));
	AddTestStepL(RTestStepAPSetPlayWindowEndBeforeHeadWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-MP3-I-0054"),_L("NormalMp3"), _L("filename")));
	AddTestStepL(RTestStepAPSetPlayWindowStartAfterEndWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-MP3-I-0055"),_L("NormalMp3"), _L("filename")));
	AddTestStepL(RTestStepAPSetPlayWindowStartAfterHeadWhileClipPaused::NewL(_L("MM-MMF-APLCLNT-CHRTZ-MP3-I-0056"),_L("NormalMp3"), _L("filename")));
	AddTestStepL(RTestStepAPSetPlayWindowEndBeforeHeadWhileClipPaused::NewL(_L("MM-MMF-APLCLNT-CHRTZ-MP3-I-0057"),_L("NormalMp3"), _L("filename")));
	AddTestStepL(RTestStepAPSetPlayWindowStartAfterEndWhileClipPaused::NewL(_L("MM-MMF-APLCLNT-CHRTZ-MP3-I-0058"),_L("NormalMp3"), _L("filename")));
	
	AddTestStepL(RTestStepAPClearPlayWindow::NewL(_L("MM-MMF-APLCLNT-CHRTZ-MP3-I-0059"),_L("NormalMp3"), _L("filename")));
	AddTestStepL(RTestStepAPClearPlayWindowWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-MP3-I-0060"),_L("NormalMp3"), _L("filename")));
	
	AddTestStepL(RTestStepAPSetBalance::NewL(_L("MM-MMF-APLCLNT-CHRTZ-MP3-I-0062"),_L("NormalMp3"), _L("filename")));
	AddTestStepL(RTestStepAPSetBalanceLessMaxLeft::NewL(_L("MM-MMF-APLCLNT-CHRTZ-MP3-I-0063"),_L("NormalMp3"), _L("filename")));
	AddTestStepL(RTestStepAPSetBalanceGreaterMaxRight::NewL(_L("MM-MMF-APLCLNT-CHRTZ-MP3-I-0064"),_L("NormalMp3"), _L("filename")));
	AddTestStepL(RTestStepAPSetBalanceWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-MP3-I-0065"),_L("NormalMp3"), _L("filename")));
	AddTestStepL(RTestStepAPSetBalanceLessMaxLeftWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-MP3-I-0066"),_L("NormalMp3"), _L("filename")));
	AddTestStepL(RTestStepAPSetBalanceGreaterMaxRightWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-MP3-I-0067"),_L("NormalMp3"), _L("filename")));
	AddTestStepL(RTestStepAPSetBalanceWhileClipPaused::NewL(_L("MM-MMF-APLCLNT-CHRTZ-MP3-I-0068"),_L("NormalMp3"), _L("filename")));
	AddTestStepL(RTestStepAPSetBalanceLessMaxLeftWhileClipPaused::NewL(_L("MM-MMF-APLCLNT-CHRTZ-MP3-I-0069"),_L("NormalMp3"), _L("filename")));
	AddTestStepL(RTestStepAPSetBalanceGreaterMaxRightWhileClipPaused::NewL(_L("MM-MMF-APLCLNT-CHRTZ-MP3-I-0070"),_L("NormalMp3"), _L("filename")));
	
	AddTestStepL(RTestStepAPGetBalance::NewL(_L("MM-MMF-APLCLNT-CHRTZ-MP3-I-0072"),_L("NormalMp3"), _L("filename")));
	AddTestStepL(RTestStepAPGetBalanceWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-MP3-I-0073"),_L("NormalMp3"), _L("filename")));
	
	AddTestStepL(RTestStepAPGetBitrate::NewL(_L("MM-MMF-APLCLNT-CHRTZ-MP3-I-0075"),_L("NormalMp3"), _L("filename")));
	AddTestStepL(RTestStepAPGetBitrateWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-MP3-I-0076"),_L("NormalMp3"), _L("filename")));
	
	AddTestStepL(RTestStepAPRegisterEventNotification::NewL(_L("MM-MMF-APLCLNT-CHRTZ-MP3-I-0078"),_L("NormalMp3"), _L("filename")));
	AddTestStepL(RTestStepAPRegisterEventNotificationWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-MP3-I-0078"),_L("NormalMp3"), _L("filename")));
	AddTestStepL(RTestStepAPRegisterEventNotificationWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-MP3-I-0079"),_L("NormalMp3"), _L("filename")));
	AddTestStepL(RTestStepAPRegisterEventNotificationBeforeOpenClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-MP3-I-0080"),_L("NormalMp3"), _L("filename")));
	AddTestStepL(RTestStepAPRegisterEventNotificationUidNotSupported::NewL(_L("MM-MMF-APLCLNT-CHRTZ-MP3-I-0082"),_L("NormalMp3"), _L("filename")));

	AddTestStepL(RTestStepAPRegisterEventNotificationUidAlreadyRegistered::NewL(_L("MM-MMF-APLCLNT-CHRTZ-MP3-I-0083"),_L("NormalMp3"), _L("filename")));
	
	AddTestStepL(RTestStepAPCancelRegisteredNotificationEvent::NewL(_L("MM-MMF-APLCLNT-CHRTZ-MP3-I-0084"),_L("NormalMp3"), _L("filename")));
	AddTestStepL(RTestStepAPCancelRegisteredNotificationEventWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-MP3-I-0085"),_L("NormalMp3"), _L("filename")));
	AddTestStepL(RTestStepAPCancelRegisteredNotificationEventUidNotMatched::NewL(_L("MM-MMF-APLCLNT-CHRTZ-MP3-I-0086"),_L("NormalMp3"), _L("filename")));
	AddTestStepL(RTestStepAPCancelRegisteredNotificationEventUidNULL::NewL(_L("MM-MMF-APLCLNT-CHRTZ-MP3-I-0087"),_L("NormalMp3"), _L("filename")));
	
	AddTestStepL(RTestStepAPGetNumberOfMetaDataEntries::NewL(_L("MM-MMF-APLCLNT-CHRTZ-MP3-I-0089"),_L("MetadataMp3"), _L("filename"), KErrNone));
	AddTestStepL(RTestStepAPGetMetaDataEntry::NewL(_L("MM-MMF-APLCLNT-CHRTZ-MP3-I-0090"),_L("MetadataMp3"), _L("filename"), KErrNone));
	AddTestStepL(RTestStepAPGetNumberOfMetaDataEntriesWhilePlaying::NewL(_L("MM-MMF-APLCLNT-CHRTZ-MP3-I-0091"),_L("MetadataMp3"), _L("filename"), KErrNone));
	AddTestStepL(RTestStepAPGetMetaDataEntryWhilePlaying::NewL(_L("MM-MMF-APLCLNT-CHRTZ-MP3-I-0092"),_L("MetadataMp3"), _L("filename"), KErrNone));
	AddTestStepL(RTestStepAPGetNumberOfMetaDataEntriesBeforeOpenClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-MP3-I-0093"),_L("MetadataMp3"), _L("filename")));
	AddTestStepL(RTestStepAPGetMetaDataEntryBeforeOpenClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-MP3-I-0094"),_L("MetadataMp3"), _L("filename")));
	
	AddTestStepL(RTestStepAPPlayPausePlay::NewL(_L("MM-MMF-APLCLNT-CHRTZ-MP3-I-0095"),_L("NormalMp3"), _L("filename")));


	/*************************************************************************************
	*    						Test Suite For AAC Controller
	**************************************************************************************/
	
	AddTestStepL(RTestStepAPOpenFile::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AAC-I-0001"),_L("NormalAac"), _L("filename")));
	AddTestStepL(RTestStepAPOpenCorruptFile::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AAC-I-0002"),_L("CorruptAac"), _L("filename")));
	
	AddTestStepL(RTestStepAPOpenDes::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AAC-I-0006"),_L("SmallAac"), _L("filename")));
	
	AddTestStepL(RTestStepAPPlayAudioFile::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AAC-I-0011"),_L("NormalAac"), _L("filename")));
	AddTestStepL(RTestStepAPPlayAudioFileWhilePlaying::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AAC-I-0012"),_L("NormalAac"), _L("filename")));
	AddTestStepL(RTestStepAPPlayAudioFileBeforeOpenClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AAC-I-0013"),_L("NormalAac"), _L("filename")));
	
	AddTestStepL(RTestStepAPStopWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AAC-I-0014"),_L("NormalAac"), _L("filename")));
	AddTestStepL(RTestStepAPPauseWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AAC-I-0015"),_L("NormalAac"), _L("filename")));
	AddTestStepL(RTestStepAPCloseWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AAC-I-0016"),_L("NormalAac"), _L("filename")));
	
	AddTestStepL(RTestStepAPGetMaxVolume::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AAC-I-0017"),_L("NormalAac"), _L("filename")));
	AddTestStepL(RTestStepAPGetMaxVolumeWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AAC-I-0018"),_L("NormalAac"), _L("filename")));
	
	AddTestStepL(RTestStepAPGetPlaybackVolume::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AAC-I-0020"),_L("NormalAac"), _L("filename")));
	AddTestStepL(RTestStepAPGetPlaybackVolumeWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AAC-I-0021"),_L("NormalAac"), _L("filename")));
	AddTestStepL(RTestStepAPSetPlaybackVolumeToZero::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AAC-I-0023"),_L("NormalAac"), _L("filename")));
	AddTestStepL(RTestStepAPSetPlaybackVolumeToMax::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AAC-I-0024"),_L("NormalAac"), _L("filename")));
	AddTestStepL(RTestStepAPSetPlaybackVolumeToZeroWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AAC-I-0025"),_L("NormalAac"), _L("filename")));
	AddTestStepL(RTestStepAPSetPlaybackVolumeToMaxWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AAC-I-0026"),_L("NormalAac"), _L("filename")));
	
	AddTestStepL(RTestStepAPSetVolumeRamp::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AAC-I-0029"),_L("NormalAac"), _L("filename")));
	AddTestStepL(RTestStepAPSetVolumeRampWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AAC-I-0030"),_L("NormalAac"), _L("filename")));
	

	AddTestStepL(RTestStepAPSetRepeatsToZero::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AAC-I-0032"),_L("NormalAac"), _L("filename")));
	AddTestStepL(RTestStepAPSetRepeatsToForever::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AAC-I-0033"),_L("NormalAac"), _L("filename")));
	AddTestStepL(RTestStepAPSetRepeatsToZeroWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AAC-I-0034"),_L("NormalAac"), _L("filename")));
	AddTestStepL(RTestStepAPSetRepeatsToForeverWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AAC-I-0035"),_L("NormalAac"), _L("filename")));
	
	AddTestStepL(RTestStepAPSetPriority::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AAC-I-0038"),_L("NormalAac"), _L("filename")));
	AddTestStepL(RTestStepAPSetPriorityWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AAC-I-0039"),_L("NormalAac"), _L("filename")));
	
	AddTestStepL(RTestStepAPGetPosition::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AAC-I-0041"),_L("NormalAac"), _L("filename")));
	AddTestStepL(RTestStepAPGetPositionWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AAC-I-0042"),_L("NormalAac"), _L("filename")));
	AddTestStepL(RTestStepAPSetPosition::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AAC-I-0043"),_L("NormalAac"), _L("filename")));
	AddTestStepL(RTestStepAPSetPositionNegative::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AAC-I-0044"),_L("NormalAac"), _L("filename")));
	AddTestStepL(RTestStepAPSetPositionPastDuration::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AAC-I-0045"),_L("NormalAac"), _L("filename")));
	AddTestStepL(RTestStepAPSetPositionWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AAC-I-0046"),_L("NormalAac"), _L("filename")));
	AddTestStepL(RTestStepAPSetPositionNegativeWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AAC-I-0047"),_L("NormalAac"), _L("filename")));
	AddTestStepL(RTestStepAPSetPositionPastDurationWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AAC-I-0048"),_L("NormalAac"), _L("filename")));
	AddTestStepL(RTestStepAPSetPositionWhileClipPaused::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AAC-I-0049"),_L("NormalAac"), _L("filename")));
	AddTestStepL(RTestStepAPSetPositionNegativeWhileClipPaused::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AAC-I-0050"),_L("NormalAac"), _L("filename")));
	AddTestStepL(RTestStepAPSetPositionPastDurationWhileClipPaused::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AAC-I-0051"),_L("NormalAac"), _L("filename")));
	
	AddTestStepL(RTestStepAPSetPlayWindow::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AAC-I-0052"),_L("NormalAac"), _L("filename")));
	AddTestStepL(RTestStepAPSetPlayWindowStartAfterHeadWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AAC-I-0053"),_L("NormalAac"), _L("filename")));
	AddTestStepL(RTestStepAPSetPlayWindowEndBeforeHeadWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AAC-I-0054"),_L("NormalAac"), _L("filename")));
	AddTestStepL(RTestStepAPSetPlayWindowStartAfterEndWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AAC-I-0055"),_L("NormalAac"), _L("filename")));
	AddTestStepL(RTestStepAPSetPlayWindowStartAfterHeadWhileClipPaused::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AAC-I-0056"),_L("NormalAac"), _L("filename")));
	AddTestStepL(RTestStepAPSetPlayWindowEndBeforeHeadWhileClipPaused::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AAC-I-0057"),_L("NormalAac"), _L("filename")));
	AddTestStepL(RTestStepAPSetPlayWindowStartAfterEndWhileClipPaused::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AAC-I-0058"),_L("NormalAac"), _L("filename")));
	
	AddTestStepL(RTestStepAPClearPlayWindow::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AAC-I-0059"),_L("NormalAac"), _L("filename")));
	AddTestStepL(RTestStepAPClearPlayWindowWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AAC-I-0060"),_L("NormalAac"), _L("filename")));
	
	AddTestStepL(RTestStepAPSetBalance::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AAC-I-0062"),_L("NormalAac"), _L("filename")));
	AddTestStepL(RTestStepAPSetBalanceLessMaxLeft::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AAC-I-0063"),_L("NormalAac"), _L("filename")));
	AddTestStepL(RTestStepAPSetBalanceGreaterMaxRight::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AAC-I-0064"),_L("NormalAac"), _L("filename")));
	AddTestStepL(RTestStepAPSetBalanceWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AAC-I-0065"),_L("NormalAac"), _L("filename")));
	AddTestStepL(RTestStepAPSetBalanceLessMaxLeftWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AAC-I-0066"),_L("NormalAac"), _L("filename")));
	AddTestStepL(RTestStepAPSetBalanceGreaterMaxRightWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AAC-I-0067"),_L("NormalAac"), _L("filename")));
	AddTestStepL(RTestStepAPSetBalanceWhileClipPaused::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AAC-I-0068"),_L("NormalAac"), _L("filename")));
	AddTestStepL(RTestStepAPSetBalanceLessMaxLeftWhileClipPaused::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AAC-I-0069"),_L("NormalAac"), _L("filename")));
	AddTestStepL(RTestStepAPSetBalanceGreaterMaxRightWhileClipPaused::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AAC-I-0070"),_L("NormalAac"), _L("filename")));
	
	AddTestStepL(RTestStepAPGetBalance::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AAC-I-0072"),_L("NormalAac"), _L("filename")));
	AddTestStepL(RTestStepAPGetBalanceWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AAC-I-0073"),_L("NormalAac"), _L("filename")));
	
	AddTestStepL(RTestStepAPGetBitrate::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AAC-I-0075"),_L("NormalAac"), _L("filename")));
	AddTestStepL(RTestStepAPGetBitrateWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AAC-I-0076"),_L("NormalAac"), _L("filename")));
	
	AddTestStepL(RTestStepAPRegisterEventNotification::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AAC-I-0078"),_L("NormalAac"), _L("filename")));
	AddTestStepL(RTestStepAPRegisterEventNotificationWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AAC-I-0078"),_L("NormalAac"), _L("filename")));
	AddTestStepL(RTestStepAPRegisterEventNotificationWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AAC-I-0079"),_L("NormalAac"), _L("filename")));
	AddTestStepL(RTestStepAPRegisterEventNotificationBeforeOpenClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AAC-I-0080"),_L("NormalAac"), _L("filename")));
	AddTestStepL(RTestStepAPRegisterEventNotificationUidNotSupported::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AAC-I-0082"),_L("NormalAac"), _L("filename")));

	AddTestStepL(RTestStepAPRegisterEventNotificationUidAlreadyRegistered::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AAC-I-0083"),_L("NormalAac"), _L("filename")));
	
	AddTestStepL(RTestStepAPCancelRegisteredNotificationEvent::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AAC-I-0084"),_L("NormalAac"), _L("filename")));
	AddTestStepL(RTestStepAPCancelRegisteredNotificationEventWhilePlayingClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AAC-I-0085"),_L("NormalAac"), _L("filename")));
	AddTestStepL(RTestStepAPCancelRegisteredNotificationEventUidNotMatched::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AAC-I-0086"),_L("NormalAac"), _L("filename")));
	AddTestStepL(RTestStepAPCancelRegisteredNotificationEventUidNULL::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AAC-I-0087"),_L("NormalAac"), _L("filename")));
	
	AddTestStepL(RTestStepAPGetNumberOfMetaDataEntries::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AAC-I-0089"),_L("MetadataAac"), _L("filename"), KErrNotSupported));
	AddTestStepL(RTestStepAPGetMetaDataEntry::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AAC-I-0090"),_L("MetadataAac"), _L("filename"), KErrNotSupported));
	AddTestStepL(RTestStepAPGetNumberOfMetaDataEntriesWhilePlaying::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AAC-I-0091"),_L("MetadataAac"), _L("filename"), KErrNotReady));
	AddTestStepL(RTestStepAPGetMetaDataEntryWhilePlaying::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AAC-I-0092"),_L("MetadataAac"), _L("filename"), KErrNotReady));
	AddTestStepL(RTestStepAPGetNumberOfMetaDataEntriesBeforeOpenClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AAC-I-0093"),_L("MetadataAac"), _L("filename")));
	AddTestStepL(RTestStepAPGetMetaDataEntryBeforeOpenClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AAC-I-0094"),_L("MetadataAac"), _L("filename")));
	
	AddTestStepL(RTestStepAPPlayPausePlay::NewL(_L("MM-MMF-APLCLNT-CHRTZ-AAC-I-0095"),_L("NormalAac"), _L("filename")));

#endif // SYMBIAN_MULTIMEDIA_A3FDEVSOUND

	/*************************************************************************************
	*    						Test Suite For No Controller
	**************************************************************************************/

	AddTestStepL(RTestStepAPOpenFileNoController::NewL(_L("MM-MMF-APLCLNT-CHRTZ-NONE-I-0003"),_L("NoController"), _L("filename")));
	AddTestStepL(RTestStepAPOpenNoFile::NewL(_L("MM-MMF-APLCLNT-CHRTZ-NONE-I-0004"),_L("NoFile"), _L("filename")));
	AddTestStepL(RTestStepAPOpenDesNoController::NewL(_L("MM-MMF-APLCLNT-CHRTZ-NONE-I-0007"),_L("NoController"), _L("filename")));
	AddTestStepL(RTestStepAPOpenURL::NewL(_L("MM-MMF-APLCLNT-CHRTZ-NONE-I-0009"),_L(""), _L("")));
	AddTestStepL(RTestStepAPGetMaxVolumeBeforeOpenClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-NONE-I-0019"),_L(""), _L("")));
	AddTestStepL(RTestStepAPGetPlaybackVolumeBeforeOpenClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-NONE-I-0022"),_L(""), _L("")));
	AddTestStepL(RTestStepAPSetPlaybackVolumeToZeroBeforeOpenClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-NONE-I-0027"),_L(""), _L("")));
	AddTestStepL(RTestStepAPSetPlaybackVolumeToMaxBeforeOpenClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-NONE-I-0028"),_L(""), _L("")));
	AddTestStepL(RTestStepAPSetRepeatsToZeroBeforeOpenClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-NONE-I-0036"),_L(""), _L("")));
	AddTestStepL(RTestStepAPSetRepeatsToForeverBeforeOpenClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-NONE-I-0037"),_L(""), _L("")));
	AddTestStepL(RTestStepAPSetPriorityBeforeOpenClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-NONE-I-0040"),_L(""), _L("")));
	AddTestStepL(RTestStepAPClearPlayWindowBeforeOpenClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-NONE-I-0061"),_L(""), _L("")));
	AddTestStepL(RTestStepAPSetBalanceBeforeOpenClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-NONE-I-0071"),_L(""), _L("")));
	AddTestStepL(RTestStepAPGetBalanceBeforeOpenClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-NONE-I-0074"),_L(""), _L("")));
	AddTestStepL(RTestStepAPGetBitrateBeforeOpenClip::NewL(_L("MM-MMF-APLCLNT-CHRTZ-NONE-I-0077"),_L(""), _L("")));
	}
