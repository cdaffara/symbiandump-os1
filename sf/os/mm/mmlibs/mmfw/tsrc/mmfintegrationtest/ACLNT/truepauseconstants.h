// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef TRUEPAUSECONSTANTS_H
#define TRUEPAUSECONSTANTS_H

#include <testframework.h>

/*
 *========================================================================================================
 * State/Event enumerations
 *========================================================================================================
 */

// MdaAudioUtility events
enum TMdaAudioUtilityEvent
	{
	EEventInitialize = 0,
	EEventPrepareComplete,
	EEventInitComplete,
	EEventPlayComplete,
	EEventTimerComplete
	};

// MdaAudioUtility states
enum TMdaAudioUtilityState
	{
	EStateInitial = 0,
	EStateCreated,
	EStateInitializing,
	EStatePlaying,
	EStateRecording,
	EStatePause,
	EStateStopped
	};

// MdaAudioOutputStream events	
enum TMdaAOSEvent
	{
	EEventAOSInitialize = 0,
	EEventAOSOpenComplete,
	EEventAOSPlayComplete,
	EEventAOSBufferCopied,
	EEventAOSTimerComplete
	};

// MdaAudioOutputStream states
enum TMdaAOSState
	{
	EStateAOSInitial = 0,
	EStateAOSCreated,
	EStateAOSInitializing,
	EStateAOSPlaying,
	EStateAOSRecording,
	EStateAOSPause,
	EStateAOSStopped
	};
	
enum TPanicCodes
	{
	EFsmIncorrectErrorPassed = 0,
	EInvalidCallbackCall
	};
	
// User defined "MdaAudioUtility client" error codes
enum TMdaAudioUtilityClientErrorCodes
	{
	EInvalidClientFSMEvent = 1,
	EInvalidClientFSMState,
	EReadFileErrorInClient,
	EWriteFileErrorInClient
	};

enum TMdaAudioUtilityTestFormat
	{
	EPCM8Format = 0,
	EPCMU16Format,
	EPCMU16BEFormat,
	EALAWFormat,
	EMULAWFormat,
	EGSM610Format,
	EIMADFormat,
	EPCMU8Format,
	EOGGFormat
	};


/*
 *========================================================================================================
 * Constant declarations
 *========================================================================================================
 */

const TInt KSizeBuff = 256;
const TInt K25Percent = 4;
const TInt KPanicLength = 16;
const TInt KMicroSecInOneSec = 1000000;
const TInt KMiliSecInOneSec = 1000;
const TInt KMicroSecInHalfOneSec = 500000;
const TInt KBalanceTolerance = 2;
const TInt KFileFormatSize = 20;
const TInt KVariation = KOneSecond/2; //Allow a half second variation
const TInt KRepeatThrice = 3;
const TInt KTimeout = 10;
const TInt KFileDivision = 4;

_LIT(KMsgErrorGetParameter,"Error in getting parameter %S from INI file");

// Section name keys
_LIT(KFilenameWAV, "FilenameWAV");
_LIT(KFilenameSQN, "FilenameSQN");
_LIT(KFilenamePCMU8, "FilenamePCMU8");
_LIT(KFilenamePCM8, "FilenamePCM8");
_LIT(KFilenameAlaw, "FilenameAlaw");
_LIT(KFilenameMulaw, "FilenameMulaw");
_LIT(KFilenamePCMU16BE, "FilenamePCMU16BE");
_LIT(KFilenamePCMU16, "FilenamePCMU16");
_LIT(KFilenamePCM16, "FilenamePCM16");
_LIT(KFilenameGSM610, "FilenameGSM610");
_LIT(KFilenameIMAD, "FilenameIMAD");
_LIT(KFilenameOGG, "FilenameOGG");
_LIT(KMsgDelete, "---- Deleting AudioPlayerUtility object ----");
_LIT(KBalance, "Balance");
_LIT(KVolume, "Volume");
_LIT(KDuration1, "Duration1");
_LIT(KDurationMiliSec, "DurationMiliSec");
_LIT(KSequence, "Sequence");
_LIT(KDuration, "Duration");
_LIT(KPause, "Pause");
_LIT(KRawFile, "RawFile");
_LIT(KOggFile, "OggFile");

#endif// __TRUEPAUSECONSTANTS_H__
