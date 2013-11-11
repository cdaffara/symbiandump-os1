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


#ifndef __MMF_CODEC_BASE_DEFINITIONS_H__
#define __MMF_CODEC_BASE_DEFINITIONS_H__

#include <mmf/server/mmfcodec.h>
#include <mmf/server/mmfdatabuffer.h>
#include <e32std.h>
// IMA ADPCM Codecs

// Internal to Symbian
class TMMFImaAdpcmCodecStateOld
	{
public:
	inline TMMFImaAdpcmCodecStateOld();
	inline TMMFImaAdpcmCodecStateOld(TInt16 aPredicted, TUint8 aIndex);
public:
	TInt16 iPredicted; // Previous output value
	TUint8 iIndex; // Index into stepsize table
	};

// Internal to Symbian
class TMMFAudioCodecBaseOld
	{
public:
	inline TMMFAudioCodecBaseOld();
	//
	virtual void Convert(TUint8* aSrc, TUint8* aDst, TInt aSamples)=0;
	};


// Internal to Symbian
class TMMFImaAdpcmBaseCodecOld : public TMMFAudioCodecBaseOld
	{
public:
	inline TMMFImaAdpcmBaseCodecOld(const TInt aChannels);
	inline void SetState(const TMMFImaAdpcmCodecStateOld& aState);
	inline void SetState(const TMMFImaAdpcmCodecStateOld& aState,const TInt aChannel);
	inline const TMMFImaAdpcmCodecStateOld& GetState();
	inline const TMMFImaAdpcmCodecStateOld& GetState(const TInt aChannel);
	void ResetBuffer();
	TBool OutputStep();
protected:
	TMMFImaAdpcmCodecStateOld iState[2];	//for mono & stereo
	static const TInt iIndexTable[16];
	static const TInt iStepSizeTable[89];
	TBool iBufferStep;
	TInt iBuffer;
	TInt iChannels;
	};

// Internal to Symbian
class TMMFImaAdpcmTo16PcmCodecOld : public TMMFImaAdpcmBaseCodecOld
	{
public:
	inline TMMFImaAdpcmTo16PcmCodecOld(const TInt aChannels);
	virtual void Convert(TUint8* aSrc, TUint8* aDst, TInt aSamples);
	};


// Internal to Symbian
class TMMF16PcmToImaAdpcmCodecOld : public TMMFImaAdpcmBaseCodecOld
	{
public:
	inline TMMF16PcmToImaAdpcmCodecOld(const TInt aChannels);
	virtual void Convert(TUint8* aSrc, TUint8* aDst, TInt aSamples);
	};


inline TMMFAudioCodecBaseOld::TMMFAudioCodecBaseOld()
	{}

inline TMMFImaAdpcmBaseCodecOld::TMMFImaAdpcmBaseCodecOld(const TInt aChannels)
: iBufferStep(ETrue), iBuffer(0), iChannels(aChannels) {}

inline TMMFImaAdpcmCodecStateOld::TMMFImaAdpcmCodecStateOld()
	: iPredicted(0), iIndex(0) {}

inline TMMFImaAdpcmCodecStateOld::TMMFImaAdpcmCodecStateOld(TInt16 aPredicted, TUint8 aIndex)
	: iPredicted(aPredicted), iIndex(aIndex) {}

inline void TMMFImaAdpcmBaseCodecOld::SetState(const TMMFImaAdpcmCodecStateOld& aState)
	{ iState[0] = aState; }

inline const TMMFImaAdpcmCodecStateOld& TMMFImaAdpcmBaseCodecOld::GetState()
	{ return iState[0]; }

inline void TMMFImaAdpcmBaseCodecOld::SetState(const TMMFImaAdpcmCodecStateOld& aState, const TInt aChannel)
	{ iState[aChannel] = aState; }

inline const TMMFImaAdpcmCodecStateOld& TMMFImaAdpcmBaseCodecOld::GetState(const TInt aChannel)
	{ return iState[aChannel]; }

inline TMMFImaAdpcmTo16PcmCodecOld::TMMFImaAdpcmTo16PcmCodecOld(const TInt aChannels)
	:TMMFImaAdpcmBaseCodecOld(aChannels)
	{}
inline TMMF16PcmToImaAdpcmCodecOld::TMMF16PcmToImaAdpcmCodecOld(const TInt aChannels)
	:TMMFImaAdpcmBaseCodecOld(aChannels)
	{}

#endif
