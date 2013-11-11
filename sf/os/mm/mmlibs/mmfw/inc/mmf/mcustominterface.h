//mcustominterface.h

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
 @publishedPartner
 @released
*/

#ifndef __MCUSTOMINTERFACE_H
#define __MCUSTOMINTERFACE_H

#include <e32std.h>

class MCustomInterface 
	{
public:

	/*
	Retrieves a pointer to a Custom Interface
	@param	aInterfaceUid. The UID of the custom interface.
	@return	Pointer to Custom Interface (NULL if the CI does not exist)
	*/
	virtual TAny* CustomInterface(TUid aInterfaceId) = 0;

	};

#endif
