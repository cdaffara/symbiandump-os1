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
// MMFAudioImaAdpcmToS16PcmCodec.cpp
// 
//

#include "MMFAudioImaAdpcmToS16PcmCodec.h"

/**
*
* Convert
* @param aSrc
* @param aDst
* @param aSamples
*
*/
EXPORT_C void TMMFAudioImaAdpcmToS16PcmCodec::Convert(TUint8* aSrc, TUint8* aDst, TInt aSamples)
	{
	TInt delta;			// Current adpcm output value 
    TInt step;			// Stepsize
    TInt valpred;		// Predicted value 
    TInt vpdiff;		// Current change to valpred 
    TInt index;			// Current step change index 

	//[Read first sample and index from block header
	// we do not need to store the information across calls 
	//since we process the entire block here]
	valpred = (*aSrc++) & KAndMask8bit;
	valpred |= STATIC_CAST(TInt16, ((*aSrc++) << 8));
	index = *aSrc++;
	TUint8* dst=aDst;

	aSrc++; //skip reserved header byte

	//Write first sample to dest
	*aDst++ = STATIC_CAST( TUint8, valpred);
	*aDst++ = STATIC_CAST( TUint8, valpred >> 8);
	dst += 2;
	aSamples --;

	TBool theBufferStep = ETrue;
    TInt  bufferValue = 0;
	for ( ; aSamples > 0 ; aSamples-- ) 
		{ 
		// Step 1 - get the delta value
		if ( theBufferStep) 
			{
			bufferValue = *aSrc++;
			delta = bufferValue & 0xf;
			} 
		else 
			{
			delta = (bufferValue >> 4) & 0xf;
			}

		theBufferStep = !theBufferStep;

		ASSERT(index >= 0);
		step = KStepSizeTable[index];

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

		index += KIndexTable[delta];
		if ( index < 0 ) 
			index = 0;
		if ( index > KMaxImaAdpcmTableEntries ) 
			index = KMaxImaAdpcmTableEntries;

		*dst++ = STATIC_CAST( TUint8, valpred&KAndMask8bit);
		*dst++ = STATIC_CAST( TUint8, (valpred>>8) );
		}
	}



// IMA-ADPCM step variation table 
const TInt TMMFAudioImaAdpcmToS16PcmCodec::KIndexTable[] =
 	{
    -1, -1, -1, -1, 2, 4, 6, 8,
    -1, -1, -1, -1, 2, 4, 6, 8
	};

const TInt TMMFAudioImaAdpcmToS16PcmCodec::KStepSizeTable[89] = 
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
