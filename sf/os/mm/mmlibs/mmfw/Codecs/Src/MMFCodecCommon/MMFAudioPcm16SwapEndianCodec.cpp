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

#include "MMFAudioPcm16SwapEndianCodecCommon.h"

/**
*
* Convert
* @param aSrc
* @param aDst
* @param aSamples
* Signed 16bit to Signed 16bit, reversing endianness
*/
EXPORT_C void TMMFAudioPcm16SwapEndianCodec::Convert(TUint8* aSrc, TUint8* aDst, TInt aSamples)

	{
	while (aSamples--)
		{ 
		*aDst++ = aSrc[1];
		*aDst++ = aSrc[0];
		aSrc += 2;
		}
	}
