// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __BITDRAWSURFACE_H__
#define __BITDRAWSURFACE_H__

#include <bitdev.h>

class TSurfaceId;

/**
MSurfaceId interface manages the screen surface access

The interface could be retrieved calling CFbsDrawDevice::GetInterface() with 
KSurfaceInterfaceID as an argument.
   
MSurfaceId interface lifetime is the same as the lifetime of CFbsDrawDevice
interface, which creates it. 
Do not try to delete MSurfaceId interface pointer!
   
   
@see CFbsDrawDevice
@internalTechnology
*/
class MSurfaceId
	{
public:
	/**
	@internalTechnology
	*/
	virtual void GetSurface(TSurfaceId& aSid) const = 0;

	/**
	@internalTechnology
	*/
	virtual TUint DeviceOrientationsAvailable() const = 0;

	/**
	@internalTechnology
	*/
	virtual TBool SetDeviceOrientation(TDeviceOrientation aOrientation) = 0;

	/**
	@internalTechnology
	*/
	virtual TDeviceOrientation DeviceOrientation() const = 0;
	};

#endif//__BITDRAWSURFACE_H__


