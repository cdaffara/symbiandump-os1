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
//

#ifndef __TONEGENERATOR_H__
#define __TONEGENERATOR_H__

#include <e32base.h>

//
// Sine tone generator
// 

const TInt KMaxSineTable = 256;
const TUint KToneBufferSize = 8192;

// one second in microseconds
const TInt KOneMillionMicroSeconds = 1000000;


class TSineGen
/**
*@internalTechnology
*/
	{
public:
	void SetFrequency(TInt aFrequency,TInt aAmplitude);
	TInt NextSample();
private:
	TUint iPosition;
	TUint iStep;
	TInt iAmplitude;
	static const TInt16 SineTable[KMaxSineTable];
	static const TInt16 IncTable[KMaxSineTable];
	};

class TSineWave
/**
*@internalTechnology
*/
	{
public:
	void Generate(TInt16* aDest,TInt aCount);
	void SetFrequency(TInt aFrequency,TInt aAmplitude);
	void SetFrequency(TInt aFrequency1,TInt aAmplitude1,TInt aFrequency2,TInt aAmplitude2);
private:
	TSineGen iGen1;
	TSineGen iGen2;
	};

//
// Tone synthesis interface
// Defines the abstract interface for tone synthesis
// Capable of filling buffers with audio data
//

class MMdaToneSynthesis
/**
*@internalTechnology
*/
	{
public:
	// Allocate necessary resources for this kind of synthesis
	virtual void Configure(TInt aRate, TInt aChannels, TInt aRepeats, TInt aSilence, TInt aRampUp)=0;
	// Begin generating data from start again
	virtual void Reset()=0;
	// Fill supplied buffer with next block of 16bit PCM audio data
	virtual TInt FillBuffer(TDes8& aBuffer)=0;
	};

//
// Tone generator base class
//

class TMdaToneGenerator : public MMdaToneSynthesis
	{
public:
	virtual void Configure(TInt aRate, TInt aChannels, TInt aRepeats, TInt aSilence, TInt aRampUp);
	virtual TInt FillBuffer(TDes8& aBuffer);
protected:
	virtual TInt GetNextTone()=0;
	//
	TInt DurationToSamples(const TTimeIntervalMicroSeconds& aDuration);
protected:
	TSineWave iSineWave;
	TInt iRate;
	TInt iChannels;
	TInt iSamplesLeft;
	TInt iTrailingSilence;
	TBool iRampUp;
	TBool iRampDown;
	TBool iIncompleteRampDown;
	TBool iIncompleteRampUp;
	TInt iIncompleteVolume;
	TInt iRampUpRemainder;
	TInt iRepeats;
	TInt iSilenceBetweenRepeats;
	TBool iAfterRepeatSilence;
	TInt iRampUpCount;
	TInt iRampUpLeft;
	};

//
// Simple tone synthesis
//

class TMdaSimpleToneGenerator : public TMdaToneGenerator
/**
*@internalTechnology
*/
	{
public:
	void SetFrequencyAndDuration(TInt aFrequency, const TTimeIntervalMicroSeconds& aDuration);
	virtual void Reset();
	virtual TInt GetNextTone();
private:
	TTimeIntervalMicroSeconds iDuration;
	TInt iFrequency;
	TBool iPlayed;
	};


/**
 * Dual tone synthesis
 * Generates a tone consisting of two sine waves of different 
 * frequencies summed together.
 * 
 * @internalTechnology
 */
class TMdaDualToneGenerator : public TMdaToneGenerator
	{
public:
	void SetFrequencyAndDuration(TInt aFrequencyOne, TInt aFrequencyTwo, const TTimeIntervalMicroSeconds& aDuration);
	virtual void Reset();
	virtual TInt GetNextTone();
private:
	TTimeIntervalMicroSeconds iDuration;
	TInt iFrequencyOne;
	TInt iFrequencyTwo;
	TBool iPlayed;
	};

//
// DTMF tone synthesis
//

class TMdaDTMFGenerator : public TMdaToneGenerator
/**
*@internalTechnology
*/
	{
public:
	virtual void Reset();
	void SetToneDurations(	const TTimeIntervalMicroSeconds32 aOn,
							const TTimeIntervalMicroSeconds32 aOff,
							const TTimeIntervalMicroSeconds32 aPause);
	void SetString(const TDesC& aDTMFString); 
private:
	virtual TInt GetNextTone();
private:
	const TDesC* iDTMFString;
	TTimeIntervalMicroSeconds32 iOn;
	TTimeIntervalMicroSeconds32 iOff;
	TTimeIntervalMicroSeconds32 iPause;
	TInt iOnSamples;
	TInt iOffSamples;
	TInt iPauseSamples;
	TInt iChar;
	TBool iPlayToneOff;
	};

//
// Tone sequence synthesis
//

const TInt KMaxSequenceStack = 6;
class TMdaSequenceGenerator : public TMdaToneGenerator
/**
*@internalTechnology
*/
	{
public:
	virtual void Reset();
	void SetSequenceData(const TDesC8& aSequenceData); 
private:
	virtual TInt GetNextTone();
private:
	const TDesC8* iSequenceData;
	const TInt16* iInstructionPtr;
	const TInt16* iLastInstruction;
	TInt iStack[KMaxSequenceStack];
	TInt iStackIndex;
	};

const TInt KBufferLength = 0x1000;

// Public Media Server includes

class TMdaPtr8 : public TPtr8 //needed for this WINS Impl of Tone Gen
	{
public:
	TMdaPtr8()
		: TPtr8(0,0,0) {};
	inline void Set(const TDes8& aDes)
		{ TPtr8::Set((TUint8*)(aDes.Ptr()),aDes.Length(),aDes.MaxLength()); };
	inline void SetLengthOnly(const TDes8& aDes)
		{ TPtr8::Set((TUint8*)(aDes.Ptr()),aDes.Length(),aDes.Length()); };
	inline void Set(const TPtrC8& aDes)
		{ TPtr8::Set((TUint8*)(aDes.Ptr()),aDes.Length(),aDes.Length()); };
	inline void Shift(TInt aOffset)
		{ SetLength(Length()-aOffset); iMaxLength-=aOffset; iPtr+=aOffset; };
	};


#endif
