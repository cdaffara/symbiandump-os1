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
 @internalComponent
*/

#ifndef CONNECTIONSTATE_H
#define CONNECTIONSTATE_H

/**
Enums describing connection's state.
*/
enum TConnectionState
	{
	EConnecting = 1,
	EConnected,
	EDisconnecting,
	EDisconnected	
	};
	
#endif // CONNECTIONSTATE_H
