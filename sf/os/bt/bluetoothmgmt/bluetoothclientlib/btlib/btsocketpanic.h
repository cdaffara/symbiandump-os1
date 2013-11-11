// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// BT socket interface types
// 
//

#ifndef BTSOCKETPANIC_H
#define BTSOCKETPANIC_H

/**
Panic values:
	EUnfinishedBusiness				Panics if the Bluetooth socket is still in a busy state
	EBadArgument					Panics if a bad argument is provided
	EBadRequest						Panics if a bad request is received
	EInvalidSession					Panics if a session is invalid
	EInvalidSubSession				Panics if a sub-session is invalid
	EBBInvalidAddress				Panics if an invalid address is provided to the baseband
	EBadSyncPacketTypes				Panics if bad packet types are received for a synchronous link
	EBasebandAlreadyConnected		Panics if the baseband is already connected
	EBasebandFailedDisconnect		Panics if the baseband failed to disconnect properly
	EBasebandFailedConnect			Panics if the baseband failed to connect properly
	EInvalidOpWhenAutoSniffOn		Panics if an invalid operation occurs in sniff mode
	ENullPointerInAsyncDelete		Panics if a pointer to be deleted is invalid

*/
enum TBTSocketPanics
	{
	EUnfinishedBusiness = 0,
	EBadArgument		= 1,
	EBadRequest			= 2,
	EInvalidSession		= 3,
	EInvalidSubSession	= 4,
	EBBInvalidAddress	= 5,
	EBadSyncPacketTypes	= 6,
	EBasebandAlreadyConnected = 7,
	EBasebandFailedDisconnect = 8,
	EBasebandFailedConnect = 9,
	EInvalidOpWhenAutoSniffOn = 10,
	ENullPointerInAsyncDelete = 11,
	};
void Panic(TBTSocketPanics aCode);

#endif
