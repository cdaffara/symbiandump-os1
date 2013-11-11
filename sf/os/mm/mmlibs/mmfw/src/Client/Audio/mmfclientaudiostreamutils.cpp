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

#include "mmfclientaudiostreamutils.h"
#include <mmf/common/mmfstandardcustomcommands.h>
#include <mda/common/audio.h>

const TInt KSampleRate8000 = 8000;
const TInt KSampleRate11025 = 11025;
const TInt KSampleRate12000 = 12000;
const TInt KSampleRate16000 = 16000;
const TInt KSampleRate22050 = 22050;
const TInt KSampleRate24000 = 24000;
const TInt KSampleRate32000 = 32000;
const TInt KSampleRate44100 = 44100;
const TInt KSampleRate48000 = 48000;
const TInt KSampleRate64000 = 64000;
const TInt KSampleRate88200 = 88200;
const TInt KSampleRate96000 = 96000;


_LIT(KStreamUtilsPanicCategory, "MMFStreamUtils");

enum TStreamUtilsPanic
	{
	EPanicBadArgument,
	EPanicPostConditionViolation,
	EPanicUnknownSampleRate,
	EPanicUnknownChannelSetting,
	EPanicUnknownEncoding
	};

LOCAL_C void Panic(const TStreamUtilsPanic aReason)
	{
	User::Panic(KStreamUtilsPanicCategory, aReason);
	}

/**
 *
 * Function to map the channels enum defined in Mda to the one
 * defined in MMF
 *
 */
TUint StreamUtils::MapChannelsMdaToMMFL(TInt aMdaChannels)
	{
	if (aMdaChannels >= 0)
		{
		if (aMdaChannels & TMdaAudioDataSettings::EChannelsStereo)
			{
			return EMMFStereo;
			}
		else if ((aMdaChannels == 0) ||		// zeroed settings (it's valid.) return a default value
				 (aMdaChannels & TMdaAudioDataSettings::EChannelsMono))
			{
			return EMMFMono;
			}
		}
	// Invalid value
	User::Leave(KErrNotSupported);
	return 0;
	}

/**
 *
 * Function to map the sample rate enum defined in Mda to the one
 * defined in MMF
 *
 */
TUint StreamUtils::MapSampleRateMdaToMMFL(TInt aMdaSampleRate)
	{
	switch (aMdaSampleRate)
		{
		case TMdaAudioDataSettings::ESampleRate8000Hz:
			return EMMFSampleRate8000Hz;
		case TMdaAudioDataSettings::ESampleRate11025Hz:
			return EMMFSampleRate11025Hz;
		case TMdaAudioDataSettings::ESampleRate12000Hz:
			return EMMFSampleRate12000Hz;
		case TMdaAudioDataSettings::ESampleRate16000Hz:
			return EMMFSampleRate16000Hz;
		case TMdaAudioDataSettings::ESampleRate22050Hz:
			return EMMFSampleRate22050Hz;
		case TMdaAudioDataSettings::ESampleRate24000Hz:
			return EMMFSampleRate24000Hz;
		case TMdaAudioDataSettings::ESampleRate32000Hz:
			return EMMFSampleRate32000Hz;
		case TMdaAudioDataSettings::ESampleRate44100Hz:
			return EMMFSampleRate44100Hz;
		case TMdaAudioDataSettings::ESampleRate48000Hz:
			return EMMFSampleRate48000Hz;
		case TMdaAudioDataSettings::ESampleRate64000Hz:
			return EMMFSampleRate64000Hz;
		case TMdaAudioDataSettings::ESampleRate96000Hz:
			return EMMFSampleRate96000Hz;
		case TMdaAudioDataSettings::ESampleRateFixed:
		case TMdaAudioDataSettings::ESampleRateAnyInRange:
			User::Leave(KErrNotSupported);
			return 0;
		case 0: // zeroed settings (it's valid.) return a default value
			return EMMFSampleRate8000Hz;
		default:
			User::Leave(KErrNotSupported);
			return 0;
		}
	}

/**
 * Function to convert the mmf sample rate enum to an actual sample rate value
 */
TInt StreamUtils::SampleRateAsValue(const TMMFCapabilities& aCaps)
	{
	switch (aCaps.iRate)
		{
		case EMMFSampleRate8000Hz:
			return KSampleRate8000;
		case EMMFSampleRate11025Hz:
			return KSampleRate11025;
		case EMMFSampleRate12000Hz:
			return KSampleRate12000;
		case EMMFSampleRate16000Hz:
			return KSampleRate16000;
		case EMMFSampleRate22050Hz:
			return KSampleRate22050;
		case EMMFSampleRate24000Hz:
			return KSampleRate24000;
		case EMMFSampleRate32000Hz:
			return KSampleRate32000;
		case EMMFSampleRate44100Hz:
			return KSampleRate44100;
		case EMMFSampleRate48000Hz:
			return KSampleRate48000;
		case EMMFSampleRate64000Hz:
			return KSampleRate64000;
		case EMMFSampleRate88200Hz:
			return KSampleRate88200;
		case EMMFSampleRate96000Hz:
			return KSampleRate96000;
		default:
			Panic(EPanicUnknownSampleRate);
			return 0;
		}
	}

/**
 * Return the current number of bytes required to render each sample, based
 * on the given capabilities. This depends on the current encoding and 
 * whether the sample is mono or stereo
 */
TInt StreamUtils::BytesPerSample(const TMMFCapabilities& aCaps)
	{
	TInt noOfChannels = (aCaps.iChannels == EMMFStereo) ? 2 : 1;
	switch (aCaps.iEncoding)
		{
		case EMMFSoundEncoding8BitPCM:
		case EMMFSoundEncoding8BitALaw:
		case EMMFSoundEncoding8BitMuLaw:
			return (1 * noOfChannels);
		case EMMFSoundEncoding16BitPCM:
			return (2 * noOfChannels);
		default:
			Panic(EPanicUnknownEncoding);
			return 0;
		}
	}
/**
* CalculateLeftRightBalance
* @param aLeft
* @param aRight
* @param aBalance
* Preconditions:
* !(aBalance < KMMFBalanceMaxLeft || aBalance > KMMFBalanceMaxRight)
* y = m x + c
* aLeft = m ( aBalance ) + c
* when aBalance = KMMFBalanceMaxLeft   aLeft = 100
* when aBalance = KMMFBalanceMaxRight  aLeft = 0
* 100 = m( KMMFBalanceMaxLeft ) + c
* 0   = m( KMMFBalanceMaxRight ) + c 
* c = -(KMMFBalanceMaxRight) m
* 100 = m(KMMFBalanceMaxLeft ) - m(KMMFBalanceMaxRight)
* m = 100/(KMMFBalanceMaxLeft - KMMFBalanceMaxRight )
* c = -(KMMFBalanceMaxRight) * 100 /(KMMFBalanceMaxLeft - KMMFBalanceMaxRight )
* aLeft = ( aBalance - KMMFBalanceMaxRight ) * 100 /( KMMFBalanceMaxLeft - KMMFBalanceMaxRight )
*/
void StreamUtils::CalculateLeftRightBalance( TInt& aLeft, TInt& aRight, TInt aBalance )
	{
	// [ assert precondition that aBalance is within limits ]
    __ASSERT_DEBUG( !(aBalance < KMMFBalanceMaxLeft || aBalance > KMMFBalanceMaxRight), Panic(EPanicBadArgument));
	
	//[ Now separate percentage balances out from aBalance ]
	 aLeft = (100 * (aBalance-KMMFBalanceMaxRight)) / (KMMFBalanceMaxLeft-KMMFBalanceMaxRight);
     aRight = 100 - aLeft;

	 //[ assert post condition that left and right are within range ]
	 __ASSERT_DEBUG( ( (aLeft <= 100) && (aLeft >= 0) ), Panic(EPanicPostConditionViolation));
	 __ASSERT_DEBUG( ( (aRight <= 100) && (aRight >= 0) ), Panic(EPanicPostConditionViolation));
	}

/**
* CalculateBalance
* @param aBalance
* @param aLeft
* @param aRight
*
* follows a simple straight line transformation
* y = m x + c
* m = (KMMFBalanceMaxLeft-KMMFBalanceMaxRight)/ 100 
* c = KMMFBalanceMaxRight
* by substitution
* when aLeft = 0
*   KMMFBalanceMaxRight = m * 0 + c
*   c = KMMFBalanceMaxRight
* when aLeft = 100
* KMMFBalanceMaxLeft = m * 100 + KMMFBalanceMaxRight
* m = ( KMMFBalanceMaxLeft - KMMFBalanceMaxRight ) /100
*/
#ifdef _DEBUG
void StreamUtils::CalculateBalance( TInt& aBalance, TInt aLeft, TInt aRight )
#else
void StreamUtils::CalculateBalance( TInt& aBalance, TInt aLeft, TInt /*aRight*/ )
#endif // _DEBUG
	{
	//[ assert pre conditions ]
	__ASSERT_DEBUG( (( aLeft + aRight ) == 100 ), Panic( EPanicBadArgument ));
	__ASSERT_DEBUG( (( 0 <= aLeft) && ( 100 >= aLeft)), Panic( EPanicBadArgument) );
	__ASSERT_DEBUG( (( 0 <= aRight) && ( 100 >= aRight)), Panic( EPanicBadArgument) );

	aBalance = (aLeft * (KMMFBalanceMaxLeft-KMMFBalanceMaxRight))/100 + KMMFBalanceMaxRight;

    //[ assert post condition that aBalance is within limits ]
	__ASSERT_DEBUG( !(aBalance < KMMFBalanceMaxLeft || aBalance > KMMFBalanceMaxRight), Panic(EPanicPostConditionViolation));
	
	}
