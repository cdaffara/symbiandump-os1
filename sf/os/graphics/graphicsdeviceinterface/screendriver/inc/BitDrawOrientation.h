// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __BITDRAWORIENTATION_H__
#define __BITDRAWORIENTATION_H__

/**
MDrawDeviceOrientation interface manages the orientation degrees of CFbsDrawDevice interface.

The interface could be retrieved calling CFbsDrawDevice::GetInterface() with 
KOrientationInterfaceID as an argument.
If the drawing device does not have support for moving origin, MDrawDeviceOrientation cannot be retrieved.
   
MDrawDeviceOrientation interface lifetime is the same as the lifetime of CFbsDrawDevice
interface, which creates it. 
Do not try to delete MDrawDeviceOrientation interface pointer!
   
MDrawDeviceOrientation interface offers following method: 
Orientation()
   
@see CFbsDrawDevice
@internalTechnology
*/
class MDrawDeviceOrientation
	{
public:
	/**
	Retrieves orientation.
	@return returns orientation degrees (0, 90, 180, 270).
	@internalTechnology
	*/
	virtual CFbsDrawDevice::TOrientation Orientation() = 0;
	};

#endif//__BITDRAWORIENTATION_H__
