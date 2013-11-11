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

#ifndef PANUIINTERFACES_H
#define PANUIINTERFACES_H

#include <btextnotifiers.h>

/**
UID of the PAN profile device selection RNotifier plugin
@publishedPartner
@released
*/
const TUid KBTPanDeviceSelectionNotifierUid={0x10204500};

/**
UID of the PAN profile incoming PAN role selection RNotifier plugin
@publishedPartner
@released
*/
const TUid KBTPanNapUplinkAuthorisationNotifierUid={0x102831E9};

/**
UID of the PAN profile incoming socket, for authorisation purposes
@publishedPartner
@released
*/
const TUid KBTPanAuthorisationUid={0x10205DB1};


#endif // PANUIINTERFACES_H
