/*
* Copyright (c) 1997-2002 Nokia Corporation and/or its subsidiary(-ies).
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

#include <ecom/ecom.h>
#include <ecom/implementationproxy.h> // For making it ECom plugin
#include <mmf/plugin/mmfhwdeviceimplementationuids.hrh>
#include "MMFpcm16ToPcm16HwDevice.h"
#include "MMFpcm16ToPcmU8HwDevice.h"
#include "MMFpcmU8ToPcm16HwDevice.h"
#include "MmfMuLawToPcm16hwDevice.h"
#include "MmfALawToPcm16HwDevice.h"
#include "MmfPcm16toAlawhwDevice.h"
#include "mmfpcm16toMulawhwdevice.h"
#include "mmfpcm16SwapEndianhwdevice.h"
#include "MMFpcm16ToPcmU8HwDevice.h"
#include "mmfpcmU16TopcmS16HwDevice.h"
#include "mmfpcmU16BeToPcmS16HwDevice.h"
#include "mmfpcmS8ToPcmS16HwDevice.h"
#include "mmfpcmS16topcmU16Codec.h"
#include "mmfpcmS16PcmS8HwDevice.h"
#include "mmfpcm16topcmU16BEHwDevice.h"
#include "MmfImaAdpcmtopcm16hwdevice.h"
#include "mmfpcmU16BeToPcmS16HwDevice.h"
#include "mmfpcm16ToImaAdpcm.h"



// _________________________________________________________________________
// Exported proxy for instantiation method resolution
// Define the interface UIDs
//
const TImplementationProxy ImplementationTable[] =
	{
        IMPLEMENTATION_PROXY_ENTRY(KMmfUidHwDevicePCM16ToPCM16,    CMMFPcm16ToPcm16HwDevice::NewL),
		IMPLEMENTATION_PROXY_ENTRY(KMmfUidHwDevicePCM16ToPCMU8,    CMMFPcm16ToPcmU8HwDevice::NewL),
		IMPLEMENTATION_PROXY_ENTRY(KMmfUidHwDevicePCMU8ToPCM16,    CMMFPcmU8ToPcm16HwDevice::NewL),
		IMPLEMENTATION_PROXY_ENTRY(KMmfUidHwDeviceMulawToPCM16,    CMMFMulawToPcm16CodecHwDevice::NewL),
		IMPLEMENTATION_PROXY_ENTRY(KMmfUidHwDeviceAlawToPCM16,     CMMFAlawToPcm16CodecHwDevice::NewL),
		IMPLEMENTATION_PROXY_ENTRY(KMmfUidHwDevicePCM16ToMuLaw,    CMMFPcm16ToMulawHwDevice::NewL),
		IMPLEMENTATION_PROXY_ENTRY(KMmfUidHwDevicePCM16ToALaw,     CMMFPcm16ToAlawHwDevice::NewL),
		IMPLEMENTATION_PROXY_ENTRY(KMmfUidHwDevicePCM16toPCM16B,   CMMFPcm16SwapEndianHwDevice::NewL ),		
		IMPLEMENTATION_PROXY_ENTRY(KMmfUidHwDeviceS16ToPCMU8,      CMMFPcm16ToPcmU8HwDevice::NewL ),
		IMPLEMENTATION_PROXY_ENTRY(KMmfUidHwDeviceS16ToPCMU16,     CMMFPcm16ToPcmU16HwDevice::NewL ),	
		IMPLEMENTATION_PROXY_ENTRY(KMMFUidHwDevicePCM16toPCMU16B,  CMMFPcm16ToPcmU16BEHwDevice::NewL),
		IMPLEMENTATION_PROXY_ENTRY(KMmfUidHwDevicePCM8ToPCM16,     CMMFPcm8ToPcm16HwDevice::NewL ),
		IMPLEMENTATION_PROXY_ENTRY(KMMFUidHwDevicePCM16toPCMU16B,  CMMFPcm16ToPcmU16BEHwDevice::NewL ),
		IMPLEMENTATION_PROXY_ENTRY(KMmfUidHwDeviceS16ToPCMU16,     CMMFPcm16ToPcmU16HwDevice::NewL ),
		IMPLEMENTATION_PROXY_ENTRY(KMmfUidHwDeviceU16ToPCMS16,     CMMFPcmU16ToPcm16HwDevice::NewL ),
		IMPLEMENTATION_PROXY_ENTRY(KMmfUidHwDeviceS16ToPCMS8,      CMMFPcmS16ToPcmS8HwDevice::NewL ),
		IMPLEMENTATION_PROXY_ENTRY(KMMFUidHwDevicePCM16BtoPCM16,   CMMFPcm16SwapEndianHwDevice::NewL),
		IMPLEMENTATION_PROXY_ENTRY(KMmfUidHwDeviceU16BEToPCMS16,   CMMFPcmU16BeToPcmS16HwDevice::NewL),
		IMPLEMENTATION_PROXY_ENTRY(KMmfUidHwDevicePCM16ToIMAADPCM, CMMFPcm16ToImaAdpcmHwDevice::NewL),
		IMPLEMENTATION_PROXY_ENTRY(KMmfUidHwDeviceIMAADPCMToPCM16, CMMFImaAdpcmToPcm16CodecHwDevice::NewL),
		IMPLEMENTATION_PROXY_ENTRY(KMMFUidHwDevicePCMU16toPCMU16B, CMMFPcm16SwapEndianHwDevice::NewL),
		IMPLEMENTATION_PROXY_ENTRY(KMMFUidHwDevicePCMU16BtoPCMU16, CMMFPcm16SwapEndianHwDevice::NewL),
    };

/**
Exported function, returns a pointer to implemenation table.

@param  aTableCount
        Reference to table count.

@return	Pointer to the implementation table.
*/
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
	return ImplementationTable;
    }
