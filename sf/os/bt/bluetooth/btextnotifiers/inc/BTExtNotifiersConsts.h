// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef BTEXTNOTIFIERSCONSTS_H
#define BTEXTNOTIFIERSCONSTS_H

#include <e32cmn.h> 

/**
@file
@publishedAll
@released

UID to invoke the device selection dialog via the notifier framework.
**/
const TUid KDeviceSelectionNotifierUid={0x100069D1};

/**
@deprecated
Use TBTDeviceList::MaxNumberOfDevices() instead. Note however that 
you should call this function each time you wish to use this value
rather than storing a copy because the actual value is subject to 
change.
*/
const TUint KMaxPanDevicesForSimultaneousSelection = 8;

/** Maximum length of the Realm text buffer. */
const TUint KPbapAuthRealmLength = 256;

/** Maximum length of the password text buffer. */
const TUint KPbapAuthPasswordLength = 64;

#endif // BTEXTNOTIFIERSCONSTS_H