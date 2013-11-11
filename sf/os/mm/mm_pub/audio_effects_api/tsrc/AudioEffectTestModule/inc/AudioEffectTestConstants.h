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
* Description:  AudioEffectTestConstants test module header
*
*/


#ifndef AUDIO_EFFECT_TEST_CONSTANTS_H
#define AUDIO_EFFECT_TEST_CONSTANTS_H
#include <e32err.h>
#include <e32def.h>
 
 
// enum
enum TAudioEffects
{
	EENone=0,
	EEqualizer,
	EBassBoost,
	EDistanceAttenuation,
	EListenerDoppler,
	ESourceDoppler,
	EEnvironmentalReverb,
	EListenerLocation,
	ESourceLocation,
	ELoudness,
	EListenerOrientation,
	ESourceOrientation,
	EStereoWidening,
	ERoomLevel,
};

enum TAssignEffects
{
	ENotAssigned=0,
	EIndependent,
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
//_LIT(KWAVTestFile,"C:\\test.wav");
//_LIT(KAMRTestFile,"C:\\test.amr");
//_LIT(KAMRTest1File,"C:\\test1.amr");
_LIT(KWAVConvertTestFile,"C:\\convertTest.wav");
_LIT(KAMRTestFile,"c:\\Sounds\\test.amr");
_LIT(KWAVTestFile,"c:\\Sounds\\test.wav");
_LIT(KWAVConvertFile,"c:\\Sounds\\test1.wav");
_LIT(KAMRTest1File,"c:\\Sounds\\test1.amr");
_LIT(K3gpTestFile,"c:\\Sounds\\videoTest.3gp");
#else
_LIT(KWAVTestFile,"e:\\testing\\data\\test.wav");
_LIT(KAMRTestFile,"e:\\testing\\data\\test.amr");
_LIT(KAMRTest1File,"e:\\testing\\data\\test1.amr");
_LIT(KWAVConvertTestFile,"e:\\testing\\data\\convertTest.wav");
_LIT(K3gpTestFile,"e:\\testing\\data\\videoTest.3gp");
#endif

_LIT(KSectionStart, "[general]");
_LIT(KSectionEnd, "[general_end]");

_LIT(KTestCaseConfigStart, "[TestCase]");
_LIT(KTestCaseConfigEnd, "[End_TestCase]");

// TAGs
_LIT(KTagId, "id");
_LIT(KTagTitle, "Title");
_LIT(KTagAudioEffect, "AudioEffect");
_LIT(KTagAssignEffect, "AssignEffect");
_LIT(KTagRegisterEffect, "RegisterEffect");
_LIT(KTagUnRegisterEffect, "UnRegisterEffect");
_LIT(KTagApplySettings, "ApplySettings");
_LIT(KTagEnable, "Enable");
_LIT(KTagDisable, "Disable");
_LIT(KTagEnforce, "Enforce");
_LIT(KTagDisplayData, "DisplayData");
_LIT(KTagBandIdLevel, "BandIdLevel");
_LIT(KTagBassBoostLevel, "BassBoostLevel");
_LIT(KTagDistanceAttenuation, "DistanceAttenuation");
_LIT(KTagCartesianVelocity, "CartesianVelocity");
_LIT(KTagSphericalVelocity, "SphericalVelocity");
_LIT(KTagFactor, "Factor");
_LIT(KTagDecayHFRatio, "DecayHFRatio");
_LIT(KTagDecayTime, "DecayTime");
_LIT(KTagDensity, "Density");
_LIT(KTagDiffusion, "Diffusion");
_LIT(KTagReflectionsDelay, "ReflectionsDelay");
_LIT(KTagReflectionsLevel, "ReflectionsLevel");
_LIT(KTagReverbDelay, "ReverbDelay");
_LIT(KTagReverbLevel, "ReverbLevel");
_LIT(KTagRoomHFLevel, "RoomHFLevel");
_LIT(KTagRoomLevel, "RoomLevel");
_LIT(KTagLocationCartesian, "LocationCartesian");
_LIT(KTagLocationSpherical, "LocationSpherical");
_LIT(KTagOrientation, "Orientation");
_LIT(KTagOrientationVectors, "OrientationVectors");
_LIT(KTagStereoWideningLevel, "StereoWideningLevel");


_LIT(KLogFileTag, "logFileName");
_LIT(KLogPathTag, "logPath");

_LIT(KDefLogFile, "tmmftsPlayDefault.txt");
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

// Audio Effect
_LIT(KEqualizer, "Equalizer");
_LIT(KBassBoost, "BassBoost");
_LIT(KDistanceAttenuation, "DistanceAttenuation");
_LIT(KListenerDoppler, "ListenerDoppler");
_LIT(KSourceDoppler, "SourceDoppler");
_LIT(KEnvironmentalReverb, "EnvironmantalReverb");
_LIT(KListenerLocation, "ListenerLocation");
_LIT(KSourceLocation, "SourceLocation");
_LIT(KLoudness, "Loudness");
_LIT(KListenerOrientation, "ListenerOrientation");
_LIT(KSourceOrientation, "SourceOrientation");
_LIT(KStereoWidening, "StereoWidening");
_LIT(KRoomLevel, "RoomLevel");

// Assign Effect
_LIT(KIndependent, "Independent");
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


#endif  // AUDIO_EFFECT_TEST_CONSTANTS_H