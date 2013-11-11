// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @publishedAll
 @released
*/

#ifndef REMCONCOREAPI_H
#define REMCONCOREAPI_H

#include <e32base.h>
											
/**
The operation-specific data field for operations in the Core API has the 
following format.
For commands, there is 1 byte containing the button action. There is optional 
further data, depending on the operation itself. For instance, Play carries an 
extra 4 bytes of 'play speed' information. Hence the operation-specific data 
for a Play command is 5 bytes long.
For responses, there are 4 bytes of 'results' data. This is followed by 
whatever operation-specific data was attached to the originating command (so 
that the client can identify if necessary what the response is a response to). 
Hence the operation-specific data for a Play response is 9 bytes long.
*/

/** A buffer size for Core API operations' operation-specific data. */
const TUint KRemConCoreApiMaxOperationSpecificDataSize = 16;

/** Length of the results data in bytes. */
const TUint KRemConCoreApiResultDataLength = 4;

/** String of same length as result data.*/
_LIT8(KRemConCoreApiResultPad, "    ");

/** Offset for operation data for commands */
const TUint KRemConCoreApiCommandDataOffset = 0; 

/** Offset for operation data for responses */
const TUint KRemConCoreApiResponseDataOffset = KRemConCoreApiResultDataLength;

/** Offset of button data from beginning of data section.
ie the offset of the data for commands is KRemConCoreApiCommandDataOffset
+KRemConCoreApiButtonDataOffset. The offset of the button data for 
responses is KRemConCoreApiResponseDataOffset+KRemConCoreApiButtonDataOffset.
*/
const TUint KRemConCoreApiButtonDataOffset = 0;

/** Length of the button action data in bytes. */
const TUint KRemConCoreApiButtonDataLength = 1;

/** Button actions. */
enum TRemConCoreApiButtonAction
	{
	/** A button has been pressed. */
	ERemConCoreApiButtonPress		= 0,

	/** A button has been released. */
	ERemConCoreApiButtonRelease 	= 1,

	/** A button has been clicked. */
	ERemConCoreApiButtonClick		= 2,
	};

/** Button press refresh interval.
If the client wants to emulate a button being held down, they send a press. 
The client must re-post the press within this time, and continue to re-post 
at least this frequently, otherwise the bearer may assume that the hold has 
finished. The client may finish the hold by sending a release.*/
const TUint KRemConCoreApiPressRefreshInterval = 1000000; 

/**
The UID identifying this outer-layer RemCon interface.
*/
const TInt KRemConCoreApiUid = 0x10205E60;

/**
Operation ids belonging to the Core API.
These values are not numbered sequentially as a facility to AVRCP, which may 
assume that they are numbered the same as the operation IDs in the AV/C Panel 
Subunit Specification 1.21 Table 9.21.
*/
enum TRemConCoreApiOperationId
	{
	/** Select. */
	ERemConCoreApiSelect						= 0x00,
	/** Up. */
	ERemConCoreApiUp							= 0x01,
	/** Down. */
	ERemConCoreApiDown							= 0x02,
	/** Left. */
	ERemConCoreApiLeft							= 0x03,
	/** Right. */
	ERemConCoreApiRight 						= 0x04,
	/** RightUp. */
	ERemConCoreApiRightUp						= 0x05,
	/** RightDown. */
	ERemConCoreApiRightDown 					= 0x06,
	/** LeftUp. */
	ERemConCoreApiLeftUp						= 0x07,
	/** LeftDown. */
	ERemConCoreApiLeftDown						= 0x08,
	/** RootMenu. */
	ERemConCoreApiRootMenu						= 0x09,
	/** SetupMenu. */
	ERemConCoreApiSetupMenu 					= 0x0a,
	/** ContentsMenu. */
	ERemConCoreApiContentsMenu					= 0x0b,
	/** FavoriteMenu. */
	ERemConCoreApiFavoriteMenu					= 0x0c,
	/** Exit. */
	ERemConCoreApiExit							= 0x0d,
	/** 0. */
	ERemConCoreApi0 							= 0x20,
	/** 1. */
	ERemConCoreApi1 							= 0x21,
	/** 2. */
	ERemConCoreApi2 							= 0x22,
	/** 3. */
	ERemConCoreApi3 							= 0x23,
	/** 4. */
	ERemConCoreApi4 							= 0x24,
	/** 5. */
	ERemConCoreApi5 							= 0x25,
	/** 6. */
	ERemConCoreApi6 							= 0x26,
	/** 7. */
	ERemConCoreApi7 							= 0x27,
	/** 8. */
	ERemConCoreApi8 							= 0x28,
	/** 9. */
	ERemConCoreApi9 							= 0x29,
	/** Dot. */
	ERemConCoreApiDot							= 0x2a,
	/** Enter. */
	ERemConCoreApiEnter 						= 0x2b,
	/** Clear. */
	ERemConCoreApiClear 						= 0x2c,
	/** ChannelUp. */
	ERemConCoreApiChannelUp 					= 0x30,
	/** ChannelDown. */
	ERemConCoreApiChannelDown					= 0x31,
	/** PreviousChannel. */
	ERemConCoreApiPreviousChannel				= 0x32,
	/** SoundSelect. */
	ERemConCoreApiSoundSelect					= 0x33,
	/** InputSelect. */
	ERemConCoreApiInputSelect					= 0x34,
	/** DisplayInformation. */
	ERemConCoreApiDisplayInformation			= 0x35,
	/** Help. */
	ERemConCoreApiHelp							= 0x36,
	/** PageUp. */
	ERemConCoreApiPageUp						= 0x37,
	/** PageDown. */
	ERemConCoreApiPageDown						= 0x38,
	/** Power. */
	ERemConCoreApiPower 						= 0x40,
	/** VolumeUp. */
	ERemConCoreApiVolumeUp						= 0x41,
	/** VolumeDown. */
	ERemConCoreApiVolumeDown					= 0x42, 
	/** Mute. */
	ERemConCoreApiMute							= 0x43, 
	/** Play. */
	ERemConCoreApiPlay							= 0x44,
	/** Stop. */
	ERemConCoreApiStop							= 0x45,
	/** Pause. */
	ERemConCoreApiPause 						= 0x46,
	/** Record. */
	ERemConCoreApiRecord						= 0x47,
	/** Rewind. */
	ERemConCoreApiRewind						= 0x48,
	/** FastForward. */
	ERemConCoreApiFastForward					= 0x49,
	/** Eject. */
	ERemConCoreApiEject 						= 0x4a,
	/** Forward. */
	ERemConCoreApiForward						= 0x4b,
	/** Backward. */
	ERemConCoreApiBackward						= 0x4c,
	/** Angle. */
	ERemConCoreApiAngle 						= 0x50,
	/** Subpicture. */
	ERemConCoreApiSubpicture					= 0x51,
	/** PausePlayFunction. */
	ERemConCoreApiPausePlayFunction 			= 0x61,
	/** RestoreVolumeFunction. */
	ERemConCoreApiRestoreVolumeFunction 		= 0x66,
	/** TuneFunction. */
	ERemConCoreApiTuneFunction					= 0x67,
	/** SelectDiskFunction. */
	ERemConCoreApiSelectDiskFunction			= 0x68,
	/** SelectAvInputFunction. */
	ERemConCoreApiSelectAvInputFunction 		= 0x69,
	/** SelectAudioInputFunction. */
	ERemConCoreApiSelectAudioInputFunction		= 0x6a,
	/** F1. */
	ERemConCoreApiF1							= 0x71,
	/** F2. */
	ERemConCoreApiF2							= 0x72,
	/** F3. */
	ERemConCoreApiF3							= 0x73,
	/** F4. */
	ERemConCoreApiF4							= 0x74,
	/** F5. */
	ERemConCoreApiF5							= 0x75,

	/* No-operation */
	ENop                                        = 0x76,
	
	/**
	 * This enum is created for internal purpose to have the largest value in TRemConCoreApiOperationId.
	 * If anyone needs an value for no-operation then we suggest to use 'ENop' instead. 
	 * If new enums values are added in TRemConCoreApiOperationId then value of ELargestOperationId
	 * may change resulting in a BC break. 
	 * */
	ELargestOperationId                         = 0x76,
	};

/** Speeds associated with Play APIs. 
These values are not numbered sequentially as a facility to AVRCP, which may 
assume that they are numbered the same as the playback speeds in the AV/C 
Panel Subunit Specification 1.21 Table 9.26.
*/
enum TRemConCoreApiPlaybackSpeed
	{
	/** NextFrame. */
	ERemConCoreApiPlaybackSpeedNextFrame		= 0x30,
	/** SlowestForward. */
	ERemConCoreApiPlaybackSpeedSlowestForward	= 0x31,
	/** SlowForward6. */
	ERemConCoreApiPlaybackSpeedSlowForward6 	= 0x32,
	/** SlowForward5. */
	ERemConCoreApiPlaybackSpeedSlowForward5 	= 0x33,
	/** SlowForward4. */
	ERemConCoreApiPlaybackSpeedSlowForward4 	= 0x34,
	/** SlowForward3. */
	ERemConCoreApiPlaybackSpeedSlowForward3 	= 0x35,
	/** SlowForward2. */
	ERemConCoreApiPlaybackSpeedSlowForward2 	= 0x36,
	/** SlowForward1. */
	ERemConCoreApiPlaybackSpeedSlowForward1 	= 0x37,
	/** X1. */
	ERemConCoreApiPlaybackSpeedX1				= 0x38,
	/** FastForward1. */
	ERemConCoreApiPlaybackSpeedFastForward1 	= 0x39,
	/** FastForward2. */
	ERemConCoreApiPlaybackSpeedFastForward2 	= 0x3a,
	/** FastForward3. */
	ERemConCoreApiPlaybackSpeedFastForward3 	= 0x3b,
	/** FastForward4. */
	ERemConCoreApiPlaybackSpeedFastForward4 	= 0x3c,
	/** FastForward5. */
	ERemConCoreApiPlaybackSpeedFastForward5 	= 0x3d,
	/** FastForward6. */
	ERemConCoreApiPlaybackSpeedFastForward6 	= 0x3e,
	/** FastestForward. */
	ERemConCoreApiPlaybackSpeedFastestForward	= 0x3f,
	/** PreviousFrame. */
	ERemConCoreApiPlaybackSpeedPreviousFrame	= 0x40,
	/** SlowestReverse. */
	ERemConCoreApiPlaybackSpeedSlowestReverse	= 0x41,
	/** SlowReverse6. */
	ERemConCoreApiPlaybackSpeedSlowReverse6 	= 0x42,
	/** SlowReverse5. */
	ERemConCoreApiPlaybackSpeedSlowReverse5 	= 0x43,
	/** SlowReverse4. */
	ERemConCoreApiPlaybackSpeedSlowReverse4 	= 0x44,
	/** SlowReverse3. */
	ERemConCoreApiPlaybackSpeedSlowReverse3 	= 0x45,
	/** SlowReverse2. */
	ERemConCoreApiPlaybackSpeedSlowReverse2 	= 0x46,
	/** SlowReverse1. */
	ERemConCoreApiPlaybackSpeedSlowReverse1 	= 0x47,
	/** X1Reverse. */
	ERemConCoreApiPlaybackSpeedX1Reverse		= 0x48,
	/** FastReverse1. */
	ERemConCoreApiPlaybackSpeedFastReverse1 	= 0x49,
	/** FastReverse2. */
	ERemConCoreApiPlaybackSpeedFastReverse2 	= 0x4a,
	/** FastReverse3. */
	ERemConCoreApiPlaybackSpeedFastReverse3 	= 0x4b,
	/** FastReverse4. */
	ERemConCoreApiPlaybackSpeedFastReverse4 	= 0x4c,
	/** FastReverse5. */
	ERemConCoreApiPlaybackSpeedFastReverse5 	= 0x4d,
	/** FastReverse6. */
	ERemConCoreApiPlaybackSpeedFastReverse6 	= 0x4e,
	/** FastestReverse. */
	ERemConCoreApiPlaybackSpeedFastestReverse	= 0x4f,
	/** Reverse. */
	ERemConCoreApiPlaybackSpeedReverse			= 0x65,
	/** ReversePause. */
	ERemConCoreApiPlaybackSpeedReversePause 	= 0x6d,
	/** Forward. */
	ERemConCoreApiPlaybackSpeedForward			= 0x75,
	/** ForwardPause. */
	ERemConCoreApiPlaybackSpeedForwardPause 	= 0x7d,
	};

#endif // REMCONCOREAPI_H
