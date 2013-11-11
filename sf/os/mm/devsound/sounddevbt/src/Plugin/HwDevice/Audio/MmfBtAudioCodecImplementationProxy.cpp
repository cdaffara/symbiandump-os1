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

#include <ecom/ecom.h>
#include <ecom/implementationproxy.h> // For making it ECom plugin
#include <mmf/plugin/mmfbthwdeviceimplementationuids.hrh>
#include "MmfBtPcm16ToPcm16HwDevice.h"
#include "MmfBtPcm16ToPcmU8HwDevice.h"
#include "MmfBtPcmU8ToPcm16HwDevice.h"
#include "MmfBtMuLawToPcm16HwDevice.h"
#include "MmfBtALawToPcm16HwDevice.h"
#include "MmfBtPcm16ToAlawHwDevice.h"
#include "MmfBtPcm16ToMulawHwDevice.h"
#include "MmfBtPcm16SwapEndianHwDevice.h"
#include "MmfBtPcm16ToPcmU8HwDevice.h"
#include "MmfBtPcmU16ToPcmS16HwDevice.h"
#include "MmfBtPcmU16BeToPcmS16HwDevice.h"
#include "MmfBtPcmS8ToPcmS16HwDevice.h"
#include "MmfBtPcmS16ToPcmU16Codec.h"
#include "MmfBtPcmS16ToPcmS8HwDevice.h"
#include "MmfBtPcm16ToPcmU16BEHwDevice.h"
#include "MmfBtImaAdpcmToPcm16HwDevice.h"
#include "MmfBtPcmU16BeToPcmS16HwDevice.h"
#include "MmfBtPcm16ToImaAdpcm.h"



// _________________________________________________________________________
// Exported proxy for instantiation method resolution
// Define the interface UIDs
//
const TImplementationProxy ImplementationTable[] =
	{
        IMPLEMENTATION_PROXY_ENTRY(KMmfUidBtHwDevicePCM16ToPCM16,    CMMFPcm16ToPcm16HwDevice::NewL),
		IMPLEMENTATION_PROXY_ENTRY(KMmfUidBtHwDevicePCM16ToPCMU8,    CMMFPcm16ToPcmU8HwDevice::NewL),
		IMPLEMENTATION_PROXY_ENTRY(KMmfUidBtHwDevicePCMU8ToPCM16,    CMMFPcmU8ToPcm16HwDevice::NewL),
		IMPLEMENTATION_PROXY_ENTRY(KMmfUidBtHwDeviceMulawToPCM16,    CMMFMulawToPcm16CodecHwDevice::NewL),
		IMPLEMENTATION_PROXY_ENTRY(KMmfUidBtHwDeviceAlawToPCM16,     CMMFAlawToPcm16CodecHwDevice::NewL),
		IMPLEMENTATION_PROXY_ENTRY(KMmfUidBtHwDevicePCM16ToMuLaw,    CMMFPcm16ToMulawHwDevice::NewL),
		IMPLEMENTATION_PROXY_ENTRY(KMmfUidBtHwDevicePCM16ToALaw,     CMMFPcm16ToAlawHwDevice::NewL),
		IMPLEMENTATION_PROXY_ENTRY(KMmfUidBtHwDevicePCM16toPCM16B,   CMMFPcm16SwapEndianHwDevice::NewL ),		
		IMPLEMENTATION_PROXY_ENTRY(KMmfUidBtHwDeviceS16ToPCMU8,      CMMFPcm16ToPcmU8HwDevice::NewL ),
		IMPLEMENTATION_PROXY_ENTRY(KMmfUidBtHwDeviceS16ToPCMU16,     CMMFPcm16ToPcmU16HwDevice::NewL ),	
		IMPLEMENTATION_PROXY_ENTRY(KMMFUidBtHwDevicePCM16toPCMU16B,  CMMFPcm16ToPcmU16BEHwDevice::NewL),
		IMPLEMENTATION_PROXY_ENTRY(KMmfUidBtHwDevicePCM8ToPCM16,     CMMFPcm8ToPcm16HwDevice::NewL ),
		IMPLEMENTATION_PROXY_ENTRY(KMMFUidBtHwDevicePCM16toPCMU16B,  CMMFPcm16ToPcmU16BEHwDevice::NewL ),
		IMPLEMENTATION_PROXY_ENTRY(KMmfUidBtHwDeviceS16ToPCMU16,     CMMFPcm16ToPcmU16HwDevice::NewL ),
		IMPLEMENTATION_PROXY_ENTRY(KMmfUidBtHwDeviceU16ToPCMS16,     CMMFPcmU16ToPcm16HwDevice::NewL ),
		IMPLEMENTATION_PROXY_ENTRY(KMmfUidBtHwDeviceS16ToPCMS8,      CMMFPcmS16ToPcmS8HwDevice::NewL ),
		IMPLEMENTATION_PROXY_ENTRY(KMMFUidBtHwDevicePCM16BtoPCM16,   CMMFPcm16SwapEndianHwDevice::NewL),
		IMPLEMENTATION_PROXY_ENTRY(KMmfUidBtHwDeviceU16BEToPCMS16,   CMMFPcmU16BeToPcmS16HwDevice::NewL),
		IMPLEMENTATION_PROXY_ENTRY(KMmfUidBtHwDevicePCM16ToIMAADPCM, CMMFPcm16ToImaAdpcmHwDevice::NewL),
		IMPLEMENTATION_PROXY_ENTRY(KMmfUidBtHwDeviceIMAADPCMToPCM16, CMMFImaAdpcmToPcm16CodecHwDevice::NewL),
		IMPLEMENTATION_PROXY_ENTRY(KMMFUidBtHwDevicePCMU16toPCMU16B, CMMFPcm16SwapEndianHwDevice::NewL),
		IMPLEMENTATION_PROXY_ENTRY(KMMFUidBtHwDevicePCMU16BtoPCMU16, CMMFPcm16SwapEndianHwDevice::NewL),
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
