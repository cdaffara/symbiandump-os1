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

#include "MMFAudioPcm8ToPcm16Codec.h"

#include <e32base.h>
#include <e32math.h>
#include <f32file.h>
#include <s32file.h>

/**
*
* Convert
* @param aSrc
* @param aDst
* @param aSamples
* Signed 8bit to Signed 16bit
*/
EXPORT_C void TMMFAudioPcm8ToPcm16Codec::Convert(TUint8* aSrc, TUint8* aDst, TInt aSamples)
	{
	TInt16 s16;
	while (aSamples--)
		{ 
		s16 = static_cast<TInt16>((*aSrc++)<<8);
		*aDst++ = STATIC_CAST( TUint8, s16&KAndMask8bit);
		*aDst++ = STATIC_CAST( TUint8, (s16>>8)&KAndMask8bit);		
		}
	}
