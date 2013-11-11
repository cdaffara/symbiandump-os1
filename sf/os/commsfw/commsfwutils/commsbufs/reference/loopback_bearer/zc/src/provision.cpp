/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/
#include <comms-infras/ss_log.h>
#include <in_sock.h>
#include <comms-infras/metadata.h>
#include <comms-infras/coremcpractivities.h>
#include <comms-infras/ss_tiermanagerutils.h>
#include "mcpr.h"
#include "provision.h"

using namespace ESock;

void CZeroCopyLoopbackProvision::InitialiseConfigL(CCommsDatIapView* aIapView)
	{
	// We can ignore traps because valid values will be always be returned by callee. 
	TRAP_IGNORE(iIp6Provision.InitialiseConfigL(aIapView));
	}


void TZeroCopyLoopbackIp6Provision::GetIp6AddrL(CCommsDatIapView* aIapView, CommsDat::TMDBElementId aElementId, TIp6Addr& aAddr)
	{
	CAgentMetaConnectionProvider::GetIp6AddrL(aIapView, aElementId, aAddr);
	}

void TZeroCopyLoopbackIp6Provision::InitialiseConfigL(CCommsDatIapView* aIapView)
	{
	//
	// Setup default values - these values will be used in the event that we leave
	// due to an error when reading CommsDat.
	//
	
	SetIp6DNSAddrFromServer(ETrue);
	SetIp6NameServer1(KInet6AddrNone);
	SetIp6NameServer2(KInet6AddrNone);	
	
	//
	// Read CommsDat fields
	//
	
	TBool boolVal;

	aIapView->GetBoolL(KCDTIdIp6DNSAddrFromServer, boolVal);

	SetIp6DNSAddrFromServer(boolVal);

	if (!Ip6DNSAddrFromServer())
		{
		TIp6Addr addr;
		GetIp6AddrL(aIapView, KCDTIdIp6NameServer1, addr);
		SetIp6NameServer1(addr);
		GetIp6AddrL(aIapView, KCDTIdIp6NameServer2, addr);
		SetIp6NameServer2(addr);
		}
	else
		{
		SetIp6NameServer1(KInet6AddrNone);
		SetIp6NameServer2(KInet6AddrNone);	
		}
	}

//
// Attribute table for provisioning structures passed to CFProtocol
//

START_ATTRIBUTE_TABLE(CZeroCopyLoopbackProvision, CZeroCopyLoopbackProvision::EUid, CZeroCopyLoopbackProvision::ETypeId)
// no attributes defined as no serialisation takes place.
END_ATTRIBUTE_TABLE()

START_ATTRIBUTE_TABLE(TZeroCopyLoopBackAgentProvision, TZeroCopyLoopBackAgentProvision::EUid, TZeroCopyLoopBackAgentProvision::ETypeId)
// no attributes defined as no serialisation takes place.
END_ATTRIBUTE_TABLE()

