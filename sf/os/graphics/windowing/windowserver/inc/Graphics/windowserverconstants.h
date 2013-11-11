// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Window server internal constants
// 
//

#ifndef WINDOWSERVERCONSTANTS_H
#define WINDOWSERVERCONSTANTS_H

#include <textcursor.h>

/**
@internalAll
*/
_LIT(KWSERVServerName,"!Windowserver");

/**
Declaration of constant TUids for APIExtension to use as identifiers.
@internalComponent
@prototype
*/
const TUid KApiExtensionInterfaceUid = {0x102864E8};

/** Flags and masks for TTextCursor internal use only
	@internalComponent */
enum TTextCursorPrivateFlags
	{
	ETextCursorUserFlags		= 0x0000FFFF,
	ETextCursorFlagClipped		= 0x00010000,
	ETextCursorPrivateFlags		= 0xFFFF0000
	};

#endif // WINDOWSERVERCONSTANTS_H
