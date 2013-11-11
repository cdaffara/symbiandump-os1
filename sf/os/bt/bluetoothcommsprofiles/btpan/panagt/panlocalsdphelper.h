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
// panhelperlocalsdp.h
// 
//

#ifndef PANHELPERLOCALSDP_H
#define PANHELPERLOCALSDP_H

#include <e32base.h>
#include <e32std.h>
#include <btsdp.h>
#include <es_sock.h>
#include <bt_sock.h>
#include "panroles.h"

namespace PanAgent
{

enum TPanSecurityMode
/**
Different security modes on PAN services
@internalTechnology
@released
*/
	{
	ENone = 0x0000,
	EServiceLevelSecurity = 0x0001,
	E802dot1xSecurity = 0x0002
	};

/**
The PAN NAP SDP default uplink throughput.  5b8d8 octets/s == 3Mbits/s
This value reflects the current maximum BT throughput using EDR hardware
*/
static const TUint32 KPanNapDefaultUplinkThroughput = 0x0005b8d8;

/**
The version of the BNEP protocol that we implement
*/
const TUint KBnepVersion = 0x0100;

/**
The version of the PAN role that we implement
@note Currently this is the same for all roles
*/
const TUint KPanRoleVersion = 0x0100;
	
	
NONSHARABLE_CLASS(CPanHelperLocalSdpRegistrar) 
/**
Register PAN roles in the local SDP database
@internalTechnology
*/
	{
public:
	static CPanHelperLocalSdpRegistrar* NewL();
	virtual ~CPanHelperLocalSdpRegistrar();
	
	
	void RegisterLocalSdpRecordL(TBluetoothPanRole aRole, TInt aIapId);
	void UnregisterLocalSdpRecord(TBluetoothPanRole aRole);
private:
	CPanHelperLocalSdpRegistrar();
	void ConstructL();
	
public:

private:			
	RSdp iSdpServer;
	RSdpDatabase iSdpDatabase;
	CPanHelperLocalSdpRegistrar* iLocalSdpRegistrar;
	TFixedArray<TSdpServRecordHandle, KMaxPanRoles> iLocalSdpRecordHandles; // position in array represents role; role is used to index array after rebasing role from 0x111x to zero
	};
}
#endif //PANHELPERLOCALSDP_H
