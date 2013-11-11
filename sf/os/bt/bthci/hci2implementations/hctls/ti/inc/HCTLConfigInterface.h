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

/**
 @file
 @internalTechnology
*/

#ifndef HCTLCONFIGINTERFACE_H
#define HCTLCONFIGINTERFACE_H

#include <e32def.h>

/**
The UID of the HCTL configuration interface.
*/
const TInt KHctlConfigInterfaceUid = 0x10274085;

/**
Mixin for the HCTL configuration interface.
*/ 

class MHctlConfigInterface
	{
public:
	/**
	Update the BAUD rate.
	
	@param aBaudRate New value for BAUD rate
	@return Standard error value.
	*/
	virtual TInt MhciUpdateBaudRate(TUint32 aBaudRate) = 0;
	};

#endif // HCTLCONFIGINTERFACE_H
