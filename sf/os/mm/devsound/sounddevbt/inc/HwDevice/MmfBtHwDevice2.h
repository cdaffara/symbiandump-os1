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


#ifndef __MMF_BT_HW_DEVICE2_H__
#define __MMF_BT_HW_DEVICE2_H__

#include <mmfhwdevice.h>

/**
 Specialisation of CMMFHwDevice that provides asynchronous initialisation.
 @publishedAll
 */
class CMMFHwDevice2  : public CMMFHwDevice
	{
public:
	// From CMMFHwDevice
	static CMMFHwDevice2* NewL(const TFourCC& aSrcDatatype, const TFourCC& aDstDataType)
		{
		return reinterpret_cast<CMMFHwDevice2*>(CMMFHwDevice::NewL(aSrcDatatype, aDstDataType));
		};
	static CMMFHwDevice2* NewL(const TFourCC& aSrcDatatype, const TFourCC& aDstDataType, const TDesC& aPreferredSupplier)
		{
		return reinterpret_cast<CMMFHwDevice2*>(CMMFHwDevice::NewL(aSrcDatatype, aDstDataType, aPreferredSupplier));
		};
	inline static CMMFHwDevice2* NewL(TUid aUid)
		{
		return reinterpret_cast<CMMFHwDevice2*>(CMMFHwDevice::NewL(aUid));
		};
	static CMMFHwDevice2* NewL(TFileName aFileName)
		{
		return reinterpret_cast<CMMFHwDevice2*>(CMMFHwDevice::NewL(aFileName));
		};
	// CMMFHwDevice2 specific
	inline virtual ~CMMFHwDevice2(){};
	virtual void Init(THwDeviceInitParams& aDevInfo, TRequestStatus& aStatus) = 0;	
	};
	
#endif

