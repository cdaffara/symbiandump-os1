// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <ecom/implementationproxy.h>
#include "ciudecoder.h"
#include "ciupostproc.h"
#include "ciuencoder.h"
#include "ciupreproc.h"

// __________________________________________________________________________
// Exported proxy for instantiation method resolution
const TImplementationProxy ImplementationTable[] =
	{
	IMPLEMENTATION_PROXY_ENTRY(KUidDevVideoCiuTestDecodeHwDeviceDefine1, CMMFTestVideoCiuDecodeHwDevice::NewL),
	IMPLEMENTATION_PROXY_ENTRY(KUidDevVideoCiuTestPostProcHwDeviceDefine1, CMMFTestCiuVideoPostProcHwDevice::NewL),
	IMPLEMENTATION_PROXY_ENTRY(KUidDevVideoCiuTestEncodeHwDeviceDefine1, CMMFTestCiuVideoEncodeHwDevice::NewL),
	IMPLEMENTATION_PROXY_ENTRY(KUidDevVideoCiuTestPreProcHwDeviceDefine1, CMMFTestCiuVideoPreProcHwDevice::NewL)
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

	return ImplementationTable;
	}

