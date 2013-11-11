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

#include "MMFAudioU8ToS16PcmCodec.h"

/**
*
* Convert
* @param aSrc
* @param aDst
* @param aSamples
* Unsigned 8bit to Signed 16bit
*/
EXPORT_C void TMMFAudioU8ToS16PcmCodec::Convert(TUint8* aSrc, TUint8* aDst, TInt aSamples)
	{
	while (aSamples--)
		{
		*aDst++ = STATIC_CAST( TUint8, *aSrc^KMaskSign8bit );
		*aDst++ = STATIC_CAST( TUint8, *aSrc++^KMaskSign8bit );
		}
	}
