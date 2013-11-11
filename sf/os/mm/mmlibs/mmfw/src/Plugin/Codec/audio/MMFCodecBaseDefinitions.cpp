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

#include "MMFCodecBaseDefinitions.h"
#include "MMFAudioCodecBase.h"
#include <mmf/common/mmfpaniccodes.h>

// Base of Audio codecs
// These T Classes are "wrapped" by derived MMFCodecs, not exposed directly.


void Panic(TInt aPanicCode)
	{
	_LIT(KMMFCodecBaseDefinitionsPanicCategory, "MMFCodecBaseDefinitions");
	User::Panic(KMMFCodecBaseDefinitionsPanicCategory, aPanicCode);
	}


void TMMFImaAdpcmBaseCodecOld::ResetBuffer()
	{
	iBufferStep = ETrue;
	iBuffer = 0;
	}

TBool TMMFImaAdpcmBaseCodecOld::OutputStep()
	{
	return !iBufferStep;
	}

void TMMFImaAdpcmTo16PcmCodecOld::Convert(TUint8* aSrc, TUint8* aDst, TInt aSamples)
	{
    TInt delta;			// Current adpcm output value 
    TInt step;			// Stepsize
    TInt valpred;		// Predicted value 
    TInt vpdiff;		// Current change to valpred 
    TInt index;			// Current step change index 

	TInt channelCount=16;//for stereo only

	aSamples*=iChannels;

	//Read first sample and index from block header
	iState[0].iPredicted = *aSrc++;
	iState[0].iPredicted |= STATIC_CAST(TInt16, ((*aSrc++) << 8));
	iState[0].iIndex = *aSrc++;

	aSrc++; //skip reserved header byte

	valpred = iState[0].iPredicted;
	index = iState[0].iIndex;
	TUint8* dst=aDst;

	//Write first sample to dest
	*aDst++ = STATIC_CAST( TUint8, valpred);
	*aDst++ = STATIC_CAST( TUint8, valpred >> 8);
	dst += 2;
	aSamples --;

	if (iChannels==2)
		{
		iState[1].iPredicted = *aSrc++;
		iState[1].iPredicted |= STATIC_CAST(TInt16, ((*aSrc++) << 8));
		iState[1].iIndex = *aSrc++;
		aSrc++;

		*aDst++ = STATIC_CAST( TUint8, iState[1].iPredicted);
		*aDst++ = STATIC_CAST( TUint8, iState[1].iPredicted >> 8);
		dst += 2;
		aSamples --;
		}

    for ( ; aSamples > 0 ; aSamples-- ) 
		{ 
		// Step 1 - get the delta value
		if (iBufferStep) 
			{
			iBuffer = *aSrc++;
			delta = iBuffer & 0xf;
			} 
		else 
			{
			delta = (iBuffer >> 4) & 0xf;
			}

		iBufferStep = !iBufferStep;

		ASSERT(index >= 0);
		step = iStepSizeTable[index];

		vpdiff = step>>3;
		if ( delta & 4 ) 
			vpdiff += step;
		if ( delta & 2 ) 
			vpdiff += step>>1;
		if ( delta & 1 ) 
			vpdiff += step>>2;

		if ( delta & 8 )
			valpred -= vpdiff;
		else
			valpred += vpdiff;

		if ( valpred > (KClamp - 1) )
			valpred = (KClamp - 1);
		else if ( valpred < -KClamp )
			valpred = -KClamp;

		index += iIndexTable[delta];
		if ( index < 0 ) 
			index = 0;
		if ( index > KMaxImaAdpcmTableEntries ) 
			index = KMaxImaAdpcmTableEntries;

		*dst++ = STATIC_CAST( TUint8, valpred&KAndMask8bit);
		*dst++ = STATIC_CAST( TUint8, (valpred>>8)&KAndMask8bit);

		if (iChannels==2)
			{
			dst+=2;
			if (--channelCount == 8)
				{
				dst=aDst+2;	//right channel
				iState[0].iPredicted=STATIC_CAST(TInt16, valpred);
				iState[0].iIndex=STATIC_CAST(TUint8,index);
				valpred = iState[1].iPredicted;
				index = iState[1].iIndex;
				}
			else
				{
				if (!channelCount)
					{
					aDst+=32;
					dst=aDst;
					channelCount=16;
					iState[1].iPredicted=STATIC_CAST(TInt16, valpred);
					iState[1].iIndex=STATIC_CAST(TUint8, index);
					valpred = iState[0].iPredicted;
					index = iState[0].iIndex;
					}
				}
			}
		}
	if (iChannels==1)
		{
		iState[0].iPredicted=STATIC_CAST(TInt16,valpred);
		iState[0].iIndex=STATIC_CAST(TUint8,index);
		}
	}

void TMMF16PcmToImaAdpcmCodecOld::Convert(TUint8* aSrc, TUint8* aDst, TInt aSamples)
	{
	TInt val;			// Current input sample value 
    TInt sign;			// Current adpcm sign bit 
    TInt delta;			// Current adpcm output value 
	TInt diff;			// Difference between val and valprev 
	TInt step;			// Stepsize
    TInt valpred;		// Predicted value 
    TInt vpdiff;		// Current change to valpred 
    TInt index;			// Current step change index 
	
	TInt16* srcPtr=REINTERPRET_CAST(TInt16*, aSrc);
	TInt16* src=srcPtr;

	TInt bufferCount=16;//for stereo only

	if (iChannels==2)
		{
		aSamples*=2;
		iBufferStep=ETrue;
		}

	iState[0].iPredicted = *aSrc++;
	iState[0].iPredicted |= STATIC_CAST(TInt16, ((*aSrc++) << 8));

    valpred = iState[0].iPredicted;
    index = iState[0].iIndex;
    ASSERT(index >= 0);
    step = iStepSizeTable[index];

	//Write block header
	*aDst++ = STATIC_CAST( TUint8, valpred);
	*aDst++ = STATIC_CAST( TUint8, valpred >> 8);
	*aDst++ = STATIC_CAST( TUint8, index);
	*aDst++ = 0; //reserved byte
	src++;
	aSamples --;	

	if (iChannels==2)
		{
		iState[1].iPredicted = *aSrc++;
		iState[1].iPredicted |= STATIC_CAST(TInt16, ((*aSrc++) << 8));

		//Write header for second channel
		*aDst++ = STATIC_CAST( TUint8, iState[1].iPredicted);
		*aDst++ = STATIC_CAST( TUint8, iState[1].iPredicted >> 8);
		*aDst++ = STATIC_CAST( TUint8, iState[1].iIndex);
		*aDst++ = 0;
		src ++;
		aSamples --;
		}

	for (; aSamples > 0; aSamples--) 
		{ 
		val = *src;
		src += iChannels;

		ASSERT(index >= 0);
	    step = iStepSizeTable[index];

		// Step 1 - compute difference with previous value 
		diff = val - valpred;
		sign = (diff < 0) ? 8 : 0;
		if ( sign ) diff = (-diff);

		// Step 2 - Divide and clamp 
		// Note:
		// This code *approximately* computes:
		//    delta = diff*4/step;
		//    vpdiff = (delta+0.5)*step/4;
		// but in shift step bits are dropped. The net result of this is
		// that even if you have fast mul/div hardware you cannot put it to
		// good use since the fixup would be too expensive.
		//
		delta = 0;
		vpdiff = (step >> 3);
		
		if ( diff >= step ) 
			{
			delta = 4;
			diff -= step;
			vpdiff += step;
			}
		step >>= 1;
		if ( diff >= step  ) 
			{
			delta |= 2;
			diff -= step;
			vpdiff += step;
			}
		step >>= 1;
		if ( diff >= step ) 
			{
			delta |= 1;
			vpdiff += step;
			}

		// Step 3 - Update previous value 
		if ( sign )
		  valpred -= vpdiff;
		else
		  valpred += vpdiff;

		// Step 4 - Clamp previous value to 16 bits 
		if ( valpred > KClamp - 1 )
		  valpred = KClamp - 1;
		else if ( valpred < - KClamp )
		  valpred = - KClamp;

		// Step 5 - Assemble value, update index and step values 
		delta |= sign;
		
		index += iIndexTable[delta];
		if ( index < 0 ) index = 0;
		if ( index > 88 ) index = 88;

		// Step 6 - Output value 
		if (iBufferStep) 
			iBuffer = delta & 0x0f;
		else 
			*aDst++ = STATIC_CAST( TInt8, ((delta << 4) & 0xf0) | iBuffer);

		iBufferStep = !iBufferStep;
		
		if (iChannels==2)
			{
			if (--bufferCount==8)
				{
				src=srcPtr+1;	//right channel
				iState[0].iPredicted = STATIC_CAST(TInt16, valpred);
				iState[0].iIndex = STATIC_CAST(TUint8, index);
				valpred = iState[1].iPredicted;
				index = iState[1].iIndex;
				}
			else
				{
				if (!bufferCount)
					{
					iState[1].iPredicted = STATIC_CAST(TInt16, valpred);
					iState[1].iIndex = STATIC_CAST(TUint8, index);
					valpred = iState[0].iPredicted;
					index = iState[0].iIndex;
					bufferCount=16;
					srcPtr+=16;//32bytes
					src=srcPtr;
					}
				}
			}
		}

	if (iChannels==1)
		{
		iState[0].iPredicted = STATIC_CAST(TInt16, valpred);
		iState[0].iIndex = STATIC_CAST(TUint8, index);
		}
	}

// IMA-ADPCM step variation table 
const TInt TMMFImaAdpcmBaseCodecOld::iIndexTable[16] =
 	{
    -1, -1, -1, -1, 2, 4, 6, 8,
    -1, -1, -1, -1, 2, 4, 6, 8
	};

const TInt TMMFImaAdpcmBaseCodecOld::iStepSizeTable[89] = 
	{
    7, 8, 9, 10, 11, 12, 13, 14, 16, 17,
    19, 21, 23, 25, 28, 31, 34, 37, 41, 45,
    50, 55, 60, 66, 73, 80, 88, 97, 107, 118,
    130, 143, 157, 173, 190, 209, 230, 253, 279, 307,
    337, 371, 408, 449, 494, 544, 598, 658, 724, 796,
    876, 963, 1060, 1166, 1282, 1411, 1552, 1707, 1878, 2066,
    2272, 2499, 2749, 3024, 3327, 3660, 4026, 4428, 4871, 5358,
    5894, 6484, 7132, 7845, 8630, 9493, 10442, 11487, 12635, 13899,
    15289, 16818, 18500, 20350, 22385, 24623, 27086, 29794, 32767
	};


