// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This file contains an implementation of the ToneGenerator interface
// that converts all tone generation requests in to sampled audio 
// data to be played through the normal local sampled audio interface
// 
//

#include "ToneGenerator.h"
#include <e32math.h>
#include <mda/common/resource.h>

/******************************************************************************
*	Tone Generators
*
*	The following classes are used to generate simple frequency/duration tones,
*	DTMF, and SymbianOS tone sequences in a WINS environment.  The below code
*	should only be considered for WINS.
******************************************************************************/

// this defines the maximum possible amplitude allowed for TSineGen::SetFrequency()
const TInt KMaxAmplitude = 0x8000;

// default number of samples for trailing silence following a Tone
const TInt KDefaultTrailingSilenceSamples = 20;

//
// Sine tone generator
//

const TInt16 TSineGen::SineTable[KMaxSineTable] =
	{
		 0,   804,  1607,  2410,  3211,  4011,  4807,  5601,
	  6392,  7179,  7961,  8739,  9511, 10278, 11038, 11792,
	 12539, 13278, 14009, 14732, 15446, 16150, 16845, 17530,
	 18204, 18867, 19519, 20159, 20787, 21402, 22004, 22594,
	 23169, 23731, 24278, 24811, 25329, 25831, 26318, 26789,
	 27244, 27683, 28105, 28510, 28897, 29268, 29621, 29955,
	 30272, 30571, 30851, 31113, 31356, 31580, 31785, 31970,
	 32137, 32284, 32412, 32520, 32609, 32678, 32727, 32757,
	 32767, 32757, 32727, 32678, 32609, 32520, 32412, 32284,
	 32137, 31970, 31785, 31580, 31356, 31113, 30851, 30571,
	 30272, 29955, 29621, 29268, 28897, 28510, 28105, 27683,
	 27244, 26789, 26318, 25831, 25329, 24811, 24278, 23731,
	 23169, 22594, 22004, 21402, 20787, 20159, 19519, 18867,
	 18204, 17530, 16845, 16150, 15446, 14732, 14009, 13278,
	 12539, 11792, 11038, 10278,  9511,  8739,  7961,  7179,
	  6392,  5601,  4807,  4011,  3211,  2410,  1607,   804,
		 0,  -804, -1607, -2410, -3211, -4011, -4807, -5601,
	 -6392, -7179, -7961, -8739, -9511,-10278,-11038,-11792,
	-12539,-13278,-14009,-14732,-15446,-16150,-16845,-17530,
	-18204,-18867,-19519,-20159,-20787,-21402,-22004,-22594,
	-23169,-23731,-24278,-24811,-25329,-25831,-26318,-26789,
	-27244,-27683,-28105,-28510,-28897,-29268,-29621,-29955,
	-30272,-30571,-30851,-31113,-31356,-31580,-31785,-31970,
	-32137,-32284,-32412,-32520,-32609,-32678,-32727,-32757,
	-32767,-32757,-32727,-32678,-32609,-32520,-32412,-32284,
	-32137,-31970,-31785,-31580,-31356,-31113,-30851,-30571,
	-30272,-29955,-29621,-29268,-28897,-28510,-28105,-27683,
	-27244,-26789,-26318,-25831,-25329,-24811,-24278,-23731,
	-23169,-22594,-22004,-21402,-20787,-20159,-19519,-18867,
	-18204,-17530,-16845,-16150,-15446,-14732,-14009,-13278,
	-12539,-11792,-11038,-10278, -9511, -8739, -7961, -7179,
	 -6392, -5601, -4807, -4011, -3211, -2410, -1607,  -804,
	};

const TInt16 TSineGen::IncTable[KMaxSineTable] =
	{
			804,  803,  803,  801,  800,  796,  794,
	  791,  787,  782,  778,  772,  767,  760,  754,
	  747,  739,  731,  723,  714,  704,  695,  685,
	  674,  663,  652,  640,  628,  615,  602,  590,
	  575,  562,  547,  533,  518,  502,  487,  471,
	  455,  439,  422,  405,  387,  371,  353,  334,
	  317,  299,  280,  262,  243,  224,  205,  185,
	  167,  147,  128,  108,   89,   69,   49,   30,
	   10,  -10,  -30,  -49,  -69,  -89, -108, -128,
	 -147, -167, -185, -205, -224, -243, -262, -280,
	 -299, -317, -334, -353, -371, -387, -405, -422,
	 -439, -455, -471, -487, -502, -518, -533, -547,
	 -562, -575, -590, -602, -615, -628, -640, -652,
	 -663, -674, -685, -695, -704, -714, -723, -731,
	 -739, -747, -754, -760, -767, -772, -778, -782,
	 -787, -791, -794, -796, -800, -801, -803, -803,
	 -804, -804, -803, -803, -801, -800, -796, -794,
	 -791, -787, -782, -778, -772, -767, -760, -754,
	 -747, -739, -731, -723, -714, -704, -695, -685,
	 -674, -663, -652, -640, -628, -615, -602, -590,
	 -575, -562, -547, -533, -518, -502, -487, -471,
	 -455, -439, -422, -405, -387, -371, -353, -334,
	 -317, -299, -280, -262, -243, -224, -205, -185,
	 -167, -147, -128, -108,  -89,  -69,  -49,  -30,
	  -10,   10,   30,   49,   69,   89,  108,  128,
	  147,  167,  185,  205,  224,  243,  262,  280,
	  299,  317,  334,  353,  371,  387,  405,  422,
	  439,  455,  471,  487,  502,  518,  533,  547,
	  562,  575,  590,  602,  615,  628,  640,  652,
	  663,  674,  685,  695,  704,  714,  723,  731,
	  739,  747,  754,  760,  767,  772,  778,  782,
	  787,  791,  794,  796,  800,  801,  803,  803,
	  804
	};

void TSineGen::SetFrequency(TInt aFrequency,TInt aAmplitude)
//
// Given the frequency set iStep.
// Reset iPosition to the equivalent of 0 degrees.
// In the special case of aFrequency==4KHz set iPosition to 90 degrees.
//
	{

	if (aAmplitude>(1<<15))
		iAmplitude=(1<<15);
	else if (aAmplitude<-(1<<15))
		iAmplitude=-(1<<15);
	else
		iAmplitude=aAmplitude;
//
// There are 256 entries in the sine table to traverse 360 degrees.
// The codec requires samples at a rate of 8000 per second.
// Thus for a 1Hz tone the step will be 256/8000 or 4/125.
// Now we need need the integer part of the result to end up in
// the MSB so we need to multiply by 2^24. This gives the formula
// step = (f*4*2^24)/125 or (f*2^26)/125.
// Our highest frequency is 4KHz so that the term (f*2^26) exceeds
// a 32 bit result by 4000/2^6 (2^6 is the number of significant bits
// left after a multiply by 2^26). i.e. 6 bits. We overcome this by
// having 6 bits less in the fraction, so the new formula becomes
// ((f*2^20)/125)*2^6. This still gives us 20 significant bits in the
// fraction.
//
	
	iStep=(((TUint)aFrequency<<20)/125)<<6;
	iPosition=(aFrequency==4000 ? 0x40000000 : 0);
	}

TInt TSineGen::NextSample()
//
// Generate the next sample using linear interpolation
//
	{
	TUint pos=iPosition>>24;
	TInt amp=((IncTable[pos]*((iPosition&0x00ffffff)>>20)));
	amp>>=4;
	amp+=SineTable[pos];
	amp=(amp*iAmplitude)>>15;
	iPosition+=iStep;
	return(amp);
	}

void TSineWave::Generate(TInt16* aDest,TInt aCount)
//
// Called when more samples need to be generated.
//
	{
	while (aCount--)
		{
		*aDest++=STATIC_CAST(TInt16,iGen1.NextSample()+iGen2.NextSample());
		}
	}

void TSineWave::SetFrequency(TInt aFrequency,TInt aAmplitude)
//
// Set to generate a single frequency
//
	{
	SetFrequency(aFrequency,aAmplitude,0,0);
	}

void TSineWave::SetFrequency(TInt aFrequency1,TInt aAmplitude1,TInt aFrequency2,TInt aAmplitude2)
//
// Set to generate two frequencies
//
	{
	iGen1.SetFrequency(aFrequency1,aAmplitude1);
	iGen2.SetFrequency(aFrequency2,aAmplitude2);
	}

//
// TMdaToneGenerator
//

void TMdaToneGenerator::Configure(TInt aRate, TInt aChannels, TInt aRepeats, TInt aSilence, TInt aRampUp)
//
// Set up this tone generator to generate data at the desired sample rate
// and number of channels (typically mono/stereo)
// 
	{
	iRate = aRate;
	iChannels = aChannels;
	iSamplesLeft = 0;
	iIncompleteVolume = 0;
	iRampUpRemainder = 0;
	iRampUp = ETrue; // Default ramping to on as it is normally useful
	iRampDown = ETrue;
	iIncompleteRampDown = EFalse;
	iIncompleteRampUp = EFalse;
	iRepeats = aRepeats;
	iSilenceBetweenRepeats = aSilence;
	iRampUpCount = aRampUp;
	iRampUpLeft = aRampUp;
	iAfterRepeatSilence = EFalse;
	}

LOCAL_C void RampVolume(TInt16* aData,TInt aCount,TInt aStartVol,TInt aEndVol)
//
// Simple function to ramp down the volume of some samples 
// Typically used to prevent "clicking" artifacts at the beginning/end of tones
//
	{
	TInt step = (aEndVol - aStartVol)/aCount;
	while (aCount--)
		{
		TInt data = TInt(*aData) * aStartVol;
		*aData++ = TInt16(data>>15);
		aStartVol += step;
		}
	}

TInt TMdaToneGenerator::FillBuffer(TDes8& aBuffer)
//
// Fill the supplied buffer with tone data
// Sets the buffer length to zero if there is no more data to play
// The buffer must have a max length of at least one sample * channels
// e.g. 2 bytes mono, 4 bytes stereo
//
	{
	const TInt KRampUpSamples = 50;
	const TInt KRampDownSamples = 50;	
	
	ASSERT(aBuffer.MaxLength()>= (iChannels<<1));
	aBuffer.SetMax();

	TBool silence;
	TInt samples = 0; // 
	TInt used = 0; // Data used
	TInt avail = aBuffer.Length(); // Data filled
	TInt count = 0; // Data to be converted
	TBool rampUp = EFalse;

	TMdaPtr8 fill;
	fill.Set(aBuffer); // Pointer to data left to be filled

	// 
	// The rest of this function will loop around continually until the buffer
	// is filled or there is no more data to play
	//

Restart:
	silence = EFalse; // Reset
	if (iSamplesLeft == 0)
		{
		if (iTrailingSilence == 0)
			{
			TInt error = GetNextTone();
			if (error)
				return error;
			
			rampUp = ETrue;
			if ((iSamplesLeft==0)&&(iTrailingSilence==0))
				{ 
				if ((iSilenceBetweenRepeats)&&(!iAfterRepeatSilence))
					{
					iTrailingSilence = iSilenceBetweenRepeats;
					iAfterRepeatSilence = ETrue;
					goto Restart;
					}
				else
					{
					if ((iRepeats>0)||(iRepeats==KMdaRepeatForever))
						{
						iAfterRepeatSilence = EFalse;
						if (iRepeats>0)
							iRepeats--;
	
						Reset();
						goto Restart;
						}
					}
				// No more to play
				goto Finished;
				}
			goto Restart;
			}
		else
			{
			silence = ETrue;
			samples = iTrailingSilence;
			}
		}
	else
		samples = iSamplesLeft;
	
	count = Min(samples,avail>>1);
	fill.SetLength(count<<1);

	if (!silence)
		{ // Generate wave
		iSineWave.Generate(REINTERPRET_CAST(TInt16*,&fill[0]),count);
		
		if (iRampUp)
			{ 
			// Ramp up volume at beginning of tone
			if (rampUp)
				{ // Fade in first few samples
				if(count < KRampUpSamples)
					{
					// Partial rampup due to being at the end of the buffer
					TInt fadeInLength = Min(KRampUpSamples,(fill.Length()>>1));
					iIncompleteVolume = (count*((1<<15)/KRampUpSamples));
					RampVolume(CONST_CAST(TInt16*,REINTERPRET_CAST(const TInt16*,(&fill[0]))),fadeInLength,0,iIncompleteVolume);
					iRampUpRemainder = fadeInLength;
					iIncompleteRampUp = ETrue;
					}
				else
					{
					// Normal rampup
					TInt fadeInLength = Min(Min(KRampUpSamples,iSamplesLeft),(fill.Length()>>1));
					RampVolume(CONST_CAST(TInt16*,REINTERPRET_CAST(const TInt16*,(&fill[0]))),fadeInLength,0,1<<15);	
					iIncompleteRampUp = EFalse;
					}				
				}
			else if (iIncompleteRampUp)	
				{
				// Completing partial rampup at the start of a new buffer
				TInt fadeInLength = Min(Min((KRampUpSamples-iRampUpRemainder),iSamplesLeft),(fill.Length()>>1));
				RampVolume(CONST_CAST(TInt16*,REINTERPRET_CAST(const TInt16*,(&fill[0]))),fadeInLength,iIncompleteVolume,1<<15);
				iIncompleteRampUp = EFalse;
				}								
			}
		if (iRampDown)
			{ // Ramp down volume at end of tone
			if ((iSamplesLeft-count) < KRampDownSamples)
				{ 
				if(iSamplesLeft-count == 0)
					{
					// Fade out last few samples
					TInt startVolume = 1<<15;;
					TInt fadeOutLength = Min(Min(KRampDownSamples,iSamplesLeft),(fill.Length()>>1));

					if(iIncompleteRampDown)
						{
						// Completing partial rampdown at the start of a new buffer
						startVolume -= iIncompleteVolume;
						iIncompleteRampDown = EFalse;
						}

					RampVolume(CONST_CAST(TInt16*,REINTERPRET_CAST(const TInt16*,(&(fill.Right(fadeOutLength<<1))[0]))),fadeOutLength,startVolume,0);
					}					
				else if(iSamplesLeft-count > 0)
					{
					// Partial rampdown due to being at the end of the buffer
					TInt rampDifference = (KRampDownSamples-(iSamplesLeft-count));
					TInt fadeOutLength = Min(Min(rampDifference,iSamplesLeft),(fill.Length()>>1));
					iIncompleteVolume = ((rampDifference*(1<<15))/KRampDownSamples);

					RampVolume(CONST_CAST(TInt16*,REINTERPRET_CAST(const TInt16*,(&(fill.Right(fadeOutLength<<1))[0]))),fadeOutLength,1<<15,(1<<15)-iIncompleteVolume);

					iIncompleteRampDown = ETrue;
					}  					
				}
			}					
		iSamplesLeft -= count;
		}
	else
		{ // Generate silence
		fill.FillZ(count<<1);
		iTrailingSilence -= count;
		}

	used += count<<1;
	avail -= count<<1;
	fill.Shift(count<<1);	
	
	if (avail>(iChannels<<1))
		goto Restart;

Finished:

	aBuffer.SetLength(used);

	// Do any ramp up that is required
	if (iRampUpLeft>0)
		{
		TInt words = iRampUpLeft * iChannels;
		words = Min(words,used>>1);
		if (words>0) // In case buffer has zero length...
			{
			TInt left = iRampUpLeft * iChannels;
			TInt rampup = iRampUpCount * iChannels;
			iRampUpLeft -= words/iChannels;
			TInt16* sample = REINTERPRET_CAST(TInt16*,&aBuffer[0]);
			while (words--)
				{
				TInt32 sampleValue =   static_cast<TInt32>((*sample)*(rampup-(left--)));
				*sample++ = static_cast<TInt16>(sampleValue/rampup);
				}
			}
		}

	return KErrNone;
	}

TInt TMdaToneGenerator::DurationToSamples(const TTimeIntervalMicroSeconds& aDuration)
//
// Convert the given duration to a sample count using the current settings
//
	{
	const TInt64 KTInt64OneMilion = 1000000;

	// Calculate duration as samples
	TInt64 microSeconds(aDuration.Int64());  // MSVC doesn't like "aDuration.Int64()" in line below
	TInt64 dur = ((TInt64(iRate) * TInt64(iChannels) * microSeconds) / KTInt64OneMilion);
	if (I64HIGH(dur)>0)
		return KMaxTInt; // Ridiculous!
	else
		return I64LOW(dur);
	}

//
// TMdaSimpleToneGenerator
//

void TMdaSimpleToneGenerator::Reset()
	{
	iPlayed = EFalse;
	}

void TMdaSimpleToneGenerator::SetFrequencyAndDuration(TInt aFrequency, const TTimeIntervalMicroSeconds& aDuration)
//
// Store the frequency and duration of the specified sine tone
//
	{
	iFrequency = aFrequency;
	iDuration = aDuration;
	iPlayed = EFalse;
	}

TInt TMdaSimpleToneGenerator::GetNextTone()
//
// Simple implementation - just sets the supplied frequency and duration
//
	{
	// This class only plays one tone for the specified duration
	if (!iPlayed)
		{
		iSamplesLeft = I64LOW((iDuration.Int64() * TInt64(iRate))/1000000);
		iSineWave.SetFrequency(iFrequency,1<<14);
		iPlayed = ETrue;
		iTrailingSilence = 20; // Just to stop clicking
		}
	return KErrNone;
	}

//
// TMdaDualToneGenerator
//

void TMdaDualToneGenerator::Reset()
	{
	iPlayed = EFalse;
	}

void TMdaDualToneGenerator::SetFrequencyAndDuration(TInt aFrequencyOne, TInt aFrequencyTwo, const TTimeIntervalMicroSeconds& aDuration)
	{
	// Store the frequencies and duration of the specified dual tone
	iFrequencyOne = aFrequencyOne;
	iFrequencyTwo = aFrequencyTwo;
	iDuration = aDuration;
	iPlayed = EFalse;
	}

// 
// This is called by TMdaToneGenerator::FillBuffer() 
// to calculate the number of samples (iSamplesLeft) that will be needed 
// for the tone to be played and to initialize the sine wave generator.
// If the tone has already been played, then leaves iSamplesLeft 
// unmodified (should be zero) to indicate that it has finished.
//
TInt TMdaDualToneGenerator::GetNextTone()
	{
	// This class only plays one tone for the specified duration
	if (!iPlayed)
		{
		iSamplesLeft = I64LOW((iDuration.Int64() * TInt64(iRate))/KOneMillionMicroSeconds);
		iSineWave.SetFrequency(iFrequencyOne, KMaxAmplitude/2, iFrequencyTwo, KMaxAmplitude/2);
		iPlayed = ETrue;
		iTrailingSilence = KDefaultTrailingSilenceSamples; // Just to stop clicking
		}
	return KErrNone;
	}
//
// TMdaDTMFGenerator
//

const TInt KRecalculateToneLengths = KMinTInt;

void TMdaDTMFGenerator::Reset()
	{
	iChar = 0;
	}

void TMdaDTMFGenerator::SetToneDurations(const TTimeIntervalMicroSeconds32 aOn,
							const TTimeIntervalMicroSeconds32 aOff,
							const TTimeIntervalMicroSeconds32 aPause)
//
// Setup the DTMF tone durations
// aOn can be == -1 indicating should play first tone indefinately
//
	{
	ASSERT(aOn.Int() >=-1);
	ASSERT(aOff.Int()>=0);
	ASSERT(aPause.Int()>=0);

	iOn = aOn;
	iOff = aOff;
	iPause = aPause;

	iOnSamples = KRecalculateToneLengths; // Must recalculate these later
	}

void TMdaDTMFGenerator::SetString(const TDesC& aDTMFString)
//
// Store the DTMF string to be played
// No need to validate it as it will already have been checked 
//
	{
	iChar = 0;
	iDTMFString = &aDTMFString;
	}

const TUint8 KDtmfVolumeTable[4][4]=
//
// Relative strengths to assign to different DTMF tones
//
// This is only important if DTMFs are being played through a speaker
// and need to be machine-recognisable. This table compensates for frequency
// drop-off in the speaker and can boost the relative volume of some 
// frequencies so they are still within tolerance.
// 
// The values normally need to be determined using a frequency analyser on 
// the hardware
// 
// Each column == same low frequency (697, 770, 852, 941 Hz)
// Each row == same high frequency (1209, 1336, 1477, 1633 Hz)
//
// The value are interpreted as ratios:
//		0  == 100% low
//		7f == 50% low, 50% high
//		ff == 100% high
//
	{
	{38,27,29,37},
	{46,36,36,46},
	{62,47,49,58},
	{70,56,60,68}
	};

const TUint8 KDtmfTone697=0x0;
const TUint8 KDtmfTone770=0x1;
const TUint8 KDtmfTone852=0x2;
const TUint8 KDtmfTone941=0x3;

const TUint8 KDtmfTone1209=0x00;
const TUint8 KDtmfTone1336=0x10;
const TUint8 KDtmfTone1477=0x20;
const TUint8 KDtmfTone1633=0x30;

const TUint8 KDtmfToneTable[16]=
	{
	KDtmfTone941|KDtmfTone1336,//0
	KDtmfTone697|KDtmfTone1209,//1
	KDtmfTone697|KDtmfTone1336,//2
	KDtmfTone697|KDtmfTone1477,//3
	KDtmfTone770|KDtmfTone1209,//4
	KDtmfTone770|KDtmfTone1336,//5
	KDtmfTone770|KDtmfTone1477,//6
	KDtmfTone852|KDtmfTone1209,//7
	KDtmfTone852|KDtmfTone1336,//8
	KDtmfTone852|KDtmfTone1477,//9

	KDtmfTone697|KDtmfTone1633,//A
	KDtmfTone770|KDtmfTone1633,//B
	KDtmfTone852|KDtmfTone1633,//C
	KDtmfTone941|KDtmfTone1633,//D
	KDtmfTone941|KDtmfTone1209,//E or *
	KDtmfTone941|KDtmfTone1477,//F or #
	};

TInt TMdaDTMFGenerator::GetNextTone()
//
// Setup frequency/duration/silence settings for next DTMF tone
// Supported characters are 0-9 A-F * # , and any kind of white space
//
	{
	TBool onlyPlayFirstTone = EFalse;

	if (iOnSamples == KRecalculateToneLengths)
		{
		// Must recalculate tone durations as samples

		// Handle special case where tone on duration negative
		// - meaning play first character indefinately
		if (iOn.Int()>=0)
			iOnSamples = DurationToSamples(TInt64(iOn.Int()));
		else 
			{
			onlyPlayFirstTone = ETrue;
			iOnSamples = -1; 
			}

		iOffSamples = DurationToSamples(TInt64(iOff.Int()));
		iPauseSamples = DurationToSamples(TInt64(iPause.Int()));
		}

	ASSERT(iDTMFString);

	if (iChar==iDTMFString->Length())
		return KErrNone; // Finished. Nothing to do

	TInt highFrequency = 0;
	TInt highVolume = 0;
	TInt lowFrequency = 0; 
	TInt lowVolume =0;

Retry:
   	TChar c((*iDTMFString)[iChar++]);
   	if ((TUint)c=='#' || (TUint)c=='*' || c.IsHexDigit())
   		{
    	TInt tableIndex;
		switch ((TUint)c)
			{
		case '*':
			tableIndex=14;
			break;
		case '#':
			tableIndex=15;
			break;
		default:
			if (c.IsDigit())
    			tableIndex=(TUint)c-'0';
			else //letter
		   		{
				c.UpperCase();
    			tableIndex=(TUint)c-'A'+10;
				}
			}
		TInt high=KDtmfToneTable[tableIndex]&0xf0;
		TInt low=KDtmfToneTable[tableIndex]&0x0f;
		switch(high)
			{
		case KDtmfTone1209:
			highFrequency=1209;
			break;
		case KDtmfTone1336:
			highFrequency=1336;
			break;
		case KDtmfTone1477:
			highFrequency=1477;
			break;
		default://KDtmfTone1633:
			highFrequency=1633;
			break;
			}
		switch(low)
			{
		case KDtmfTone697:
			lowFrequency=697;
			break;
		case KDtmfTone770:
			lowFrequency=770;
			break;
		case KDtmfTone852:
			lowFrequency=852;
			break;
		default://KDtmfTone941:
			lowFrequency=941;
			break;
			}
		high>>=4;
		const TUint8* dtmfVolumes=&KDtmfVolumeTable[0][0];
		TInt volume=dtmfVolumes[((low)<<2)+(high)]<<7;
		highVolume = volume;
		lowVolume = (1<<15)-volume;

		iTrailingSilence = iOffSamples;
		iSamplesLeft = iOnSamples;
		}
   	else if ((TUint)c==',')
		{
  		iTrailingSilence = iPauseSamples;
 		iSamplesLeft = 0;
    	}
	else if (c.IsSpace())
		{
		if (iChar < iDTMFString->Length())
			goto Retry;
		}
	else
		return KErrCorrupt;

	if (iOnSamples < 0) // Play only first character for ever
		{
		iTrailingSilence = 0;
		iSamplesLeft = iRate * iChannels; // One second of samples
		iChar = 0; // Reset so this character is played again next time
		iRampDown = EFalse;
		if (!onlyPlayFirstTone)
			{
			iRampUp = EFalse;
			// This is not the first time around so we should not
			// reset the tone generator - it will already have the
			// correct settings and setting them again would cause
			// an audible discontinuity
			return KErrNone; 
			}
		}

	iSineWave.SetFrequency(highFrequency,highVolume,lowFrequency,lowVolume);
	return KErrNone;
	}

//
// TMdaSequenceGenerator
//

//
// Sequence constants
// 

//const TInt KMaxFixedSequenceStack=KMaxSequenceStack;//Max nesting level of FixedSequences * 2 
#ifdef _DEBUG
const TInt16 KFixedSequenceSignatureOne='S'+('Q'<<8); 
const TInt16 KFixedSequenceSignatureTwo='N'+('C'<<8);
#endif // _DEBUG

const TInt KFixedSequenceFunctionReturn=-1;
const TInt KFixedSequenceFunctionStartLoop=-2;
const TInt KFixedSequenceFunctionEndLoop=-3;

void TMdaSequenceGenerator::Reset()
	{
	iInstructionPtr = REINTERPRET_CAST(const TInt16*,&((*iSequenceData)[0]));
	iInstructionPtr += 2; // Skip signature
	iStackIndex = 0;
	}
	
void TMdaSequenceGenerator::SetSequenceData(const TDesC8& aSequenceData)
//
// Store the sequence data to be played
// No need to validate it as it will already have been checked 
//
	{
	iSequenceData = &aSequenceData;
	iInstructionPtr = REINTERPRET_CAST(const TInt16*,&aSequenceData[0]);
	iLastInstruction = iInstructionPtr + (iSequenceData->Length()>>1) - 1;

	// These are asserts because this should not be called if signature not present
	ASSERT(*iInstructionPtr == KFixedSequenceSignatureOne);
	ASSERT(*(iInstructionPtr+1) == KFixedSequenceSignatureTwo);

	iInstructionPtr += 2; // Skip signature

	iStackIndex = 0;
	}

TInt TMdaSequenceGenerator::GetNextTone()
//
//
	{
	ASSERT(iInstructionPtr); // Sanity check

	TInt ret = KRequestPending;
	while (ret == KRequestPending)
		{
		if (iInstructionPtr > iLastInstruction)
			ret = KErrCorrupt;
		else if (*iInstructionPtr<=0)
	   		{
	   		switch (*iInstructionPtr)
	   			{
	   		case KFixedSequenceFunctionReturn: // End of sequence
				ret = KErrNone;
				break;

	   		case KFixedSequenceFunctionStartLoop:
				if (iStackIndex>2) // Validate - can only nest twice
					ret = KErrCorrupt;
				else if ((iInstructionPtr+2) > iLastInstruction)
					ret = KErrCorrupt; // Don't run off end of sequence
				else
					{
		   			iStack[iStackIndex++]=(TInt)(iInstructionPtr+2);
		   			iStack[iStackIndex++]=(TInt)*(iInstructionPtr+1);
	   				iInstructionPtr+=2;
					}
	   			break;

	   		case KFixedSequenceFunctionEndLoop:
				if (iStackIndex==0) // Validate - must already be nested
					ret = KErrCorrupt;
				else
					{
		   			if ((--iStack[iStackIndex-1])!=0)
		   				iInstructionPtr=(TInt16*)iStack[iStackIndex-2];
		   			else
		   				{
		   				iStackIndex-=2;
		   				iInstructionPtr++;
		   				}
					}
	   			break;

	   		default: // Bad sequence
				ret = KErrCorrupt;
	   			}
			}
		else
			{
			if ((iInstructionPtr+5) > iLastInstruction)
				ret = KErrCorrupt; // Don't run off end of sequence
			else
				{
				iSamplesLeft = *iInstructionPtr++;
				TInt freqOne = *iInstructionPtr++;
				TInt volOne  = *iInstructionPtr++;
				TInt freqTwo = *iInstructionPtr++;
				TInt volTwo  = *iInstructionPtr++;

				if ((volOne> 1<<15)||(volTwo > 1<<15))
					ret = KErrCorrupt;
				else	
					{
					iSineWave.SetFrequency(freqOne,volOne,freqTwo,volTwo);
					ret = KErrNone;
					}
				}
			}
		}
	return ret;
	}

// ---------------------------------
// Code to generate sine table files used by tone generator
// Optionally called from InitL()
// #define GENERATE_SINE_TABLES 1
#ifdef GENERATE_SINE_TABLES
LOCAL_C GenerateSineTableL()
	{
	_LIT(KSineFile,"sine.txt");
	_LIT(KSineIncFile,"sineinc.txt");

	RFile file;
	file.Replace(MdaManager::Fs(),KSineFile,EFileWrite);
	CleanupClosePushL(file);

	RFile file2;
	file2.Replace(MdaManager::Fs(),KSineIncFile,EFileWrite);
	CleanupClosePushL(file2);

	const TReal pi=3.141592653589;
	const TReal twopi=pi*2;
	const TReal samples = 256.0;
	const TReal step = twopi/samples;

	TBuf8<128> sinebuffer;
	TBuf8<128> incbuffer;
	TReal res;
	TInt first=0;
	TInt last=KMaxTInt;
	TInt current;
	_LIT8(KFormat,"%6d,");
	_LIT8(KNewLine,"\n");

	for(TReal angle=0.0;angle<=(twopi-step);) // Copes with rounding errors
		{
		sinebuffer.Zero();
		incbuffer.Zero();
		for (int i=0;i<8;i++)
			{
			User::LeaveIfError(Math::Sin(res,angle));
			current = TInt(KMaxTInt16*res);
			sinebuffer.AppendFormat(KFormat,current);
			if (last != KMaxTInt)
				incbuffer.AppendFormat(KFormat,current-last);
			else
				first = current;
			last = current;
			angle += step;
			}
		sinebuffer.Append(KNewLine);
		incbuffer.Append(KNewLine);
		file.Write(sinebuffer);
		file2.Write(incbuffer);
		}

	// Write fine difference to incbuffer - differnece between first and last
	incbuffer.Zero();
	incbuffer.AppendFormat(KFormat,first-last);
	incbuffer.Append(KNewLine);
	file2.Write(incbuffer);

	CleanupStack::PopAndDestroy(2);
	}
#endif
//-------------------------------
