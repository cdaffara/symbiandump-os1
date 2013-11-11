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

#include <e32std.h>
#include "MMFAlawToPcm16Codec.h"
#include "MMFPcm16ToAlawCodec.h"
#include "MMFPcm8ToPcm16Codec.h"
#include "MMFAudioS16ToPcms16Codec.h"
#include "MMFAudioS16ToPcmS8Codec.h"
#include "MMFAudioS16ToPcmU16BeCodec.h"
#include "MMFAudioS16ToPcmU16Codec.h"
#include "MMFAudioS16ToPcmU8Codec.h"
#include "MMFAudioS8ToPcmS16Codec.h"
#include "MMFAudioU16BeToPcmS16Codec.h"
#include "MMFAudioU16ToPcmS16Codec.h"
#include "MMFAudioU8ToPcmS16Codec.h"
#include "MMFImaAdPcmToPcm16Codec.h"
#include "MMFMuLawToPcm16Codec.h"
#include "MMFPcm16ToImaAdPcmCodec.h"
#include "MMFPcm16ToMuLawCodec.h"
#include "MMFImaAdStereoPcmToPcm16Codec.h"
#include "MMFPcm16ToImaAdPcmStereoCodec.h"
#include "MMFAudioPCM16SwapEndianCodec.h"

#include <ecom/implementationproxy.h>
#include <mmf/plugin/mmfcodecimplementationuids.hrh>


// __________________________________________________________________________
// Exported proxy for instantiation method resolution
// Define the interface UIDs

const TImplementationProxy ImplementationTable[] = 
	{
		IMPLEMENTATION_PROXY_ENTRY(KMmfUidCodecAlawToPCM16,	CMMFAlawPcm16Codec::NewL),
		IMPLEMENTATION_PROXY_ENTRY(KMmfUidCodecPCM16ToAlaw,	CMMFPcm16AlawCodec::NewL),
		IMPLEMENTATION_PROXY_ENTRY(KMmfUidCodecPCM8ToPCM16,	CMMFPcm8Pcm16Codec::NewL),
		IMPLEMENTATION_PROXY_ENTRY(KMMFUidCodecPCM16toPCM16B,	CMMFAudio16PcmSwapEndianCodec::NewL),
		IMPLEMENTATION_PROXY_ENTRY(KMMFUidCodecPCM16BtoPCM16,	CMMFAudio16PcmSwapEndianCodec::NewL),
		IMPLEMENTATION_PROXY_ENTRY(KMMFUidCodecPCMU16toPCMU16B,	CMMFAudio16PcmSwapEndianCodec::NewL),
		IMPLEMENTATION_PROXY_ENTRY(KMMFUidCodecPCMU16BtoPCMU16,	CMMFAudio16PcmSwapEndianCodec::NewL),
		IMPLEMENTATION_PROXY_ENTRY(KMmfUidCodecAudioS16ToPCMS16,	CMMFAudioS16PcmS16Codec::NewL),
		IMPLEMENTATION_PROXY_ENTRY(KMmfUidCodecAudioS16ToPCMS8,	CMMFAudioS16PcmS8Codec::NewL),
		IMPLEMENTATION_PROXY_ENTRY(KMmfUidCodecAudioS16ToPCMU16BE,	CMMFAudioS16PcmU16BECodec::NewL),
		IMPLEMENTATION_PROXY_ENTRY(KMmfUidCodecAudioS16ToPCMU16,	CMMFAudioS16PcmU16Codec::NewL),
		IMPLEMENTATION_PROXY_ENTRY(KMmfUidCodecAudioS16ToPCMU8,	CMMFAudioS16PcmU8Codec::NewL),
		IMPLEMENTATION_PROXY_ENTRY(KMmfUidCodecAudioU16BEToPCMS16,	CMMFAudioU16BEPcmS16Codec::NewL),
		IMPLEMENTATION_PROXY_ENTRY(KMmfUidCodecAudioU16ToPCMS16,	CMMFAudioU16PcmS16Codec::NewL),
		IMPLEMENTATION_PROXY_ENTRY(KMmfUidCodecAudioU8ToPCMS16,	CMMFAudioU8PcmS16Codec::NewL),
		IMPLEMENTATION_PROXY_ENTRY(KMmfUidCodecIMAADPCMToPCM16,	CMMFImaAdPcmPcm16Codec::NewL),
		IMPLEMENTATION_PROXY_ENTRY(KMmfUidCodecMulawToPCM16,	CMMFMulawPcm16Codec::NewL),
		IMPLEMENTATION_PROXY_ENTRY(KMmfUidCodecPCM16ToIMAADPCM,	CMMFPcm16ImaAdPcmCodec::NewL),
		IMPLEMENTATION_PROXY_ENTRY(KMmfUidCodecPCM16ToMulaw,	CMMFPcm16MulawCodec::NewL)
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

	return ImplementationTable;
	}


