/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  EffectPresetsTest test module header.
*
*/


#ifndef AUDIO_EFFECT_TEST_CONSTANTS
#define AUDIO_EFFECT_TEST_CONSTANTS
 
 
// enum
enum TObject
{
	EENone=0,
	EEqualizer,
	EEnvironmentalReverb,
	EStereoWidening,
};

enum TAssignEffects
{
	ENotAssigned=0,
	EPlayer,
	ERecorder,
	EConverter,
	ETone,
	EDevsound,
	ECustomCommand,
	ECustomInterface,
	EInputStream,
	EOutputStream,
	EMidiClient,
	EDrmPlayer,
	EVideoPlayer,
};
 

// Constants

const TInt KErrConfigInvalid = KErrNone;	//-1100;
const TInt KErrExpectedValueDifferent = -1502;	//-1101
const TInt KErrSkippedEvents = -1102;
const TInt KErrNotExecuted = -1003;
const TInt KErrExecuted = KErrNone;
const TInt KErrLeft = -1503;


#ifdef __WINSCW__
_LIT(KAMRTestFile,"c:\\testing\\data\\test.amr");
_LIT(KWAVTestFile,"c:\\testing\\data\\test.wav");
_LIT(KWAVConvertFile,"c:\\testing\\data\\test1.wav");
_LIT(KAMRConvertFile,"c:\\testing\\data\\test1.amr");
_LIT(K3gpTestFile,"c:\\testing\\data\\videoTest.3gp");
#else
_LIT(KAMRTestFile,"e:\\testing\\data\\test.amr");
_LIT(KWAVTestFile,"e:\\testing\\data\\test.wav");
_LIT(KWAVConvertFile,"e:\\testing\\data\\test1.wav");
_LIT(KAMRConvertFile,"e:\\testing\\data\\test1.amr");
_LIT(K3gpTestFile,"e:\\testing\\data\\videoTest.3gp");
#endif

_LIT(KSectionStart, "[general]");
_LIT(KSectionEnd, "[general_end]");

_LIT(KTestCaseConfigStart, "[TestCase]");
_LIT(KTestCaseConfigEnd, "[End_TestCase]");

// TAGs
_LIT(KTagId, "id");
_LIT(KTagTitle, "Title");
_LIT(KTagAudioEffectUtility, "AudioEffectUtility");
_LIT(KTagEffectObject, "EffectObject");
_LIT(KTagCreatePreset, "CreatePreset");
_LIT(KTagNumberOfPreDefinePresets, "NumberOfPreDefinePresets");
_LIT(KTagApplyPreset, "ApplyPreset");
_LIT(KTagModifyPreset, "ModifyPreset");
_LIT(KTagResetPreset, "ResetPreset");
_LIT(KTagDeletePreset, "DeletePreset");
_LIT(KTagGetPreset, "GetPreset");
_LIT(KTagDisable, "DisableEffect");
_LIT(KTagBassBoostTest, "BassBoostTest");

_LIT(KTagAssignEffect, "AssignEffect");

_LIT(KLogFileTag, "logFileName");
_LIT(KLogPathTag, "logPath");

_LIT(KDefLogFile, "tEffectPresetDefault.txt");
_LIT(KDefLogPath, "");

_LIT(KTestCaseTitleFormat, "%$2$S - %$1$d");
//_LIT(KDefaultLogger, "Using Default Logger");
_LIT(KLoggerStarted, "Started logger for INI file [%S]");
_LIT(KOpeningConfig, "Opening configuration file [%S]");
_LIT(KConfigRequired, "TestCases need to be configured in file");
_LIT(KConfigInvalid, "TestCases need to be configured properly (file might have change between GetTestCases and RunTestCase)");

//Result
_LIT(KTestCaseResultNotImplemented, "TestCase not implemented");
_LIT(KTestCaseResultSuccess, "successful");
_LIT(KTestCaseResultFail, "Failed");
_LIT(KTestCaseResultLeft, "A LEAVE occurred");
_LIT(KTestCaseResultFailExpectedValueDifferent, "Expected result is different from actual result");
_LIT(KTestCaseResultFailSkippedEvents, "Not enough events took place to complete test");
_LIT(KTestCaseNotFound, "The TestCase id is invalid");
_LIT(KTestCaseInvalidTag, "The Tag is not recognized");

// Audio Effect Utility
_LIT(KEqualizerUtility, "EqualizerUtility");
_LIT(KEnvironmentalReverbUtility, "EnvironmantalReverbUtility");
_LIT(KStereoWideningUtility, "StereoWideningUtility");

// Assign Effect Object
_LIT(KPlayer, "Player");
_LIT(KRecorder, "Recorder");
_LIT(KConverter, "Converter");
_LIT(KTone, "Tone");
_LIT(KDevSound, "DevSound");
_LIT(KCustomCommand, "CustomCommand");
_LIT(KCustomInterface, "CustomInterface");
_LIT(KInputStream, "InputStream");
_LIT(KOutputStream, "OutputStream");
_LIT(KMidiClient, "MidiClient");
_LIT(KDrmPlayer, "DrmPlayer");
_LIT(KVideoPlayer, "VideoPlayer");

_LIT(KAudioNewLine,"\n");
_LIT(KAudioIndex,"%d. ");

#endif  // AUDIO_EFFECT_TEST_CONSTANTS