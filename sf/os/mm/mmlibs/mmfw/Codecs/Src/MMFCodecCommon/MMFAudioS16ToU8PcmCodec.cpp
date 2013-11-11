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

#include "MMFAudioS16ToU8PcmCodec.h"

/**
*
* Convert
* @param aSrc 
* @param aDst
* @param aSamples
* Signed 16bit to Unsigned 8bit
*/
EXPORT_C void TMMFAudioS16ToU8PcmCodec::Convert(TUint8* aSrc, TUint8* aDst, TInt aSamples)
	{
	while (aSamples--)
		{
		aSrc++;
		*aDst++ = STATIC_CAST( TUint8, (*aSrc++) - KMaskSign8bit);
		}
	}
