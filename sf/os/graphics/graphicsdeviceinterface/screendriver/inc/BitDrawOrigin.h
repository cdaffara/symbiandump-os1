// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __BITDRAWORIGIN_H__
#define __BITDRAWORIGIN_H__

/**
MDrawDeviceOrigin interface manages the origin point of CFbsDrawDevice interface.

The interface could be retrieved calling CFbsDrawDevice::GetInterface() with 
KDrawDeviceOriginInterfaceID as an argument.
If the drawing device does not have support for moving origin, MDrawDeviceOrigin cannot be retrieved.
   
MDrawDeviceOrigin interface lifetime is the same as the lifetime of CFbsDrawDevice
interface, which creates it. 
Do not try to delete MDrawDeviceOrigin interface pointer!
   
MDrawDeviceOrigin interface offers following methods: 
Get(), Set().
   
@see CFbsDrawDevice
@internalTechnology
*/
class MDrawDeviceOrigin
	{
public:
	/**
	Sets the origin of the drawing device. By default drawing device origin is (0,0).
	If you want to set the default origin, call Set() with (0,0) point coordinates.
	@param aOrigin Specifies physical coordinates of the new origin
	of the drawing device. The drawing device maps the logical point (0,0) to
	the "aOrigin" physical point .
	@return KErrNone success.
	@internalTechnology
	*/
	virtual TInt Set(const TPoint& aOrigin) = 0;
	/**
	Retrieves origin point.
	@param aOrigin Upon return contains origin point.
	@internalTechnology
	*/
	virtual void Get(TPoint& aOrigin) = 0;
	};

#endif//__BITDRAWORIGIN_H__
