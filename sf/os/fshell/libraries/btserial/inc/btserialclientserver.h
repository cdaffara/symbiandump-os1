// btserialclientserver.h
// 
// Copyright (c) 2008 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#ifndef __BTSERIALCLIENTSERVER_H
#define __BTSERIALCLIENTSERVER_H

#include <e32std.h>

_LIT(KBtSerialServerThreadName,"btserial");

const TInt KMaxBtSerialMessage=100;

enum TBtSerialMessages
	{
	EBtSerialWaitForConnection,
	EBtSerialCancelWaitForConnection,
	EBtSerialIsConnected,
	EBtSerialWrite,
	EBtSerialCancelWrite,
	EBtSerialRead,
	EBtSerialCancelRead,
	EBtSerialSetInputMode,
	EBtSerialGetConnectedDeviceAddr,
	EBtSerialGetConnectedDeviceName,
	};
	
TInt ServerThread(TAny* aParam);

#endif //__BTSERIALCLIENTSERVER_H
