// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

// Standard includes
#include <e32std.h>
#include <mmf/server/mmfswcodecwrapper.h>

#include <ecom/implementationproxy.h>
#include "GSM610.H"
#include <ecom/ecom.h>
#include <e32def.h>
#include <mmf/plugin/mmfhwdeviceimplementationuids.hrh>


//
// Exported proxy for instantiation method resolution
// Define the interface UIDs
//

const TImplementationProxy ImplementationTable[] = 
	{
		IMPLEMENTATION_PROXY_ENTRY(KMmfUidHwDeviceGSM610ToPCM16,	CMmfGsm610ToPcm16HwDevice::NewL),
		IMPLEMENTATION_PROXY_ENTRY(KMmfUidHwDevicePCM16ToGSM610,	CMmfPcm16ToGsm610HwDevice::NewL), 
	};


//
// ImplementationGroupProxy
//
//

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

	return ImplementationTable;
	}


