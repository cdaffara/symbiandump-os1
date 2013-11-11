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

/**
 @file
 @publishedPartner
 @released
*/

#ifndef HARDRESETINITIATOR_H
#define HARDRESETINITIATOR_H


/** 
The UID of the Controller Hard Reset API. If the HC Hard Reset API ever has to change,
a new UID and associated M- class will be created. 

Old (non-updated) Controller Hard Reset plugins will still work as long as the old API 
is supported
*/

#include <e32base.h>

const TInt KHCHardResetUid = 0x1027407F;

/**
Mixin for the Controller Hard Reset API.
This interface is to be used via CHciCorePlugin.
@see CHciCorePlugin::Interface(TUid)
*/
class MHardResetInitiator
	{
public:
	/**
	This function should be used to 'reboot' the controller, for example by
	powering it down, and then up again.
	*/
	virtual void MhriStartHardReset() = 0;
	};

#endif // HARDRESETINITIATOR_H
