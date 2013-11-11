// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#if !defined __MMFCLIENTAUDIOSTREAMUTILS_H__
#define __MMFCLIENTAUDIOSTREAMUTILS_H__

#include <e32std.h>
#include <mmf/server/sounddevice.h>

class StreamUtils
	{
public:
	static TUint MapChannelsMdaToMMFL(TInt aMdaChannels);
	static TUint MapSampleRateMdaToMMFL(TInt aMdaSampleRate);
	static TInt SampleRateAsValue(const TMMFCapabilities& aCaps);
	static TInt BytesPerSample(const TMMFCapabilities& aCaps);
//
	static void CalculateLeftRightBalance(TInt& aLeft, TInt& aRight, TInt aBalance);
	static void CalculateBalance(TInt& aBalance, TInt aLeft, TInt aRight);
	};



#endif
