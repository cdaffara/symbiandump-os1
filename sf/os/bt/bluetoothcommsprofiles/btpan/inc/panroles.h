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

#ifndef PANROLES_H
#define PANROLES_H

#include <btsdp.h> // for PAN UUIDs

enum TBluetoothPanRole
/**
@internalTechnology
@released
*/
	{
	EPanRoleUnknown = 0x0000, //< The role is not known (normally because we haven't selected it yet)
	EPanRoleU = KPanUUUID,
	EPanRoleNap = KPanNapUUID,
	EPanRoleGn = KPanGnUUID
	};
	
/**
The number of different PAN roles for which we can register in SDP
@released
@internalTechnology
*/
const TUint KMaxPanRoles = 3;
	
#endif // PANROLES_H
