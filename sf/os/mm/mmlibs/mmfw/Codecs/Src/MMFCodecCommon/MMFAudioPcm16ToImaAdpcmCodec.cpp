// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "MMFAudioPcm16ToImaAdpcmCodec.h"


EXPORT_C void TMMFAudioPcm16ToImaAdpcmCodec::SetState(const TMMFImaAdpcmCodecState& aState)
	{
     iState = aState ;
	}

EXPORT_C const TMMFImaAdpcmCodecState& TMMFAudioPcm16ToImaAdpcmCodec::GetState()
	{
	return iState;
	}

/**
*
* Convert
* @param aSrc
* @param aDst
* @param aSamples
*
*/
EXPORT_C void TMMFAudioPcm16ToImaAdpcmCodec::Convert(TUint8* aSrc, TUint8* aDst, TInt aSamples)
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
	
	iState.iPredicted = *aSrc++;
	iState.iPredicted |= STATIC_CAST(TInt16, ((*aSrc++) << 8));
	
    valpred = iState.iPredicted;
    index = iState.iIndex;
    ASSERT(index >= 0);
    step = KStepSizeTable[index];
	
	//Write block header
	*aDst++ = STATIC_CAST( TUint8, valpred);
	*aDst++ = STATIC_CAST( TUint8, valpred >> 8);
	*aDst++ = STATIC_CAST( TUint8, index);
	*aDst++ = 0; //reserved byte
	src++;
	aSamples --;	
	
	for (; aSamples > 0; aSamples--) 
		{ 
		val = *src;
		src++;
		
		step = KStepSizeTable[index];
		
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
		
		index += KIndexTable[delta];
		if ( index < 0 ) index = 0;
		if ( index > 88 ) index = 88;
		
		// Step 6 - Output value 
		if (iBufferStep) 
			iBuffer = delta & 0x0f;
		else 
			*aDst++ = STATIC_CAST( TInt8, ((delta << 4) & 0xf0) | iBuffer);
		
		iBufferStep = !iBufferStep;
		}
	
	iState.iPredicted = STATIC_CAST(TInt16, valpred);
	iState.iIndex = STATIC_CAST(TUint8, index);
	}

// IMA-ADPCM step variation table 
const TInt TMMFAudioPcm16ToImaAdpcmCodec::KIndexTable[] =
	{
	-1, -1, -1, -1, 2, 4, 6, 8,
	-1, -1, -1, -1, 2, 4, 6, 8
	};
	
const TInt TMMFAudioPcm16ToImaAdpcmCodec::KStepSizeTable[] = 
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
	



