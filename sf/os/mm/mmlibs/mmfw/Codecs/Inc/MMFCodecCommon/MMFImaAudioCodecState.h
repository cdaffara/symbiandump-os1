/*
* Copyright (c) 2002-2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/

#ifndef __MMF_TMMFIMA_ADPCM_CODEC_STATE_H__
#define __MMF_TMMFIMA_ADPCM_CODEC_STATE_H__
#include <e32std.h>

/*
* 
* class TMMFImaAdpcmCodecState
*
*/
class TMMFImaAdpcmCodecState
	{
public:
	inline TMMFImaAdpcmCodecState();
	inline TMMFImaAdpcmCodecState(TInt16 aPredicted, TUint8 aIndex);
public:
	TInt16 iPredicted; // Previous output value
	TUint8 iIndex; // Index into stepsize table
	};

inline TMMFImaAdpcmCodecState::TMMFImaAdpcmCodecState()
	: iPredicted(0), iIndex(0) 
	{
	}
inline TMMFImaAdpcmCodecState::TMMFImaAdpcmCodecState(TInt16 aPredicted, TUint8 aIndex)
: iPredicted(aPredicted), iIndex(aIndex) 
	{
	}

#endif //__MMF_TMMFIMA_ADPCM_CODEC_STATE_H__
