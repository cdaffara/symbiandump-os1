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

#ifndef __BITDRAWSCALING_H__
#define __BITDRAWSCALING_H__

/**
MScalingSettings interface manages X-axis and Y-axis scaling factors of CFbsDrawDevice interface.

The interface could be retrieved calling CFbsDrawDevice::GetInterface() with 
KScalingSettingsInterfaceID as an argument.
If the draw device does not have support for scaling, MScalingSettings cannot be retrieved.

MScalingSettings interface lifetime is the same as the lifetime of CFbsDrawDevice
interface, which creates it. 
Do not try to delete MScalingSettings interface pointer!

MScalingSettings interface offers following methods: 
Get(), Set(), IsScalingOff().

@see CFbsDrawDevice
@internalTechnology
*/
class MScalingSettings
	{
public:
	/**
	Sets scaling factor by which the drawing device should scale the drawing images.
	If you want to un-scale the device, call Set() with factorX = 1,  
	factorY = 1, divisorX = 1, divisorY = 1.
	@param aFactorX Scaling factor for the X-axis of the screen device.
	@param aFactorY Scaling factor for the y-axis of the screen device.
	@param aDivisorX Not used. Should be set to 1.
	@param aDivisorY Not used. Should be set to 1.
	@return KErrNone success.
	@internalTechnology
	*/
	virtual TInt Set(TInt aFactorX, TInt aFactorY, TInt aDivisorX, TInt aDivisorY) = 0;
	/**
	Retrieves X-axis and Y-axis scaling factors.
	@param aFactorX Upon return contains X-axis scaling factor.
	@param aFactorY Upon return contains Y-axis scaling factor.
	@param aDivisorX Upon return contains the decimal fraction of X-axis scaling factor.
	@param aDivisorY Upon return contains the decimal fraction of Y-axis scaling factor.
	@internalTechnology
	*/
	virtual void Get(TInt& aFactorX, TInt& aFactorY, TInt& aDivisorX, TInt& aDivisorY) = 0;
	/**
	Notifies the caller whether the drawing device is scaled or not.
	@return ETrue Drawing device is not scaled, EFalse - it is scaled.
	@internalTechnology
	*/
	virtual TBool IsScalingOff() = 0;
	};

#endif//__BITDRAWSCALING_H__
