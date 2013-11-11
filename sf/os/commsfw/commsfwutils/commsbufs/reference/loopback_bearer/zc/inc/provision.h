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
#ifndef __ZEROCOPY_LOOPBACK_PROVISION_H__
#define __ZEROCOPY_LOOPBACK_PROVISION_H__

#include <comms-infras/metadata.h>
#include <in_sock.h>		// TIp6Addr
#include <metadatabase.h>		// for TMDBElementId

namespace ESock
	{
	class CCommsDatIapView;
	}

class TZeroCopyLoopbackIp6Provision
/**
Provisioning information for CIPv4Binder
*/
	{
	friend class CZeroCopyLoopbackProvision;
	
public:
    inline const TIp6Addr& Ip6NameServer1() const;
    inline const TIp6Addr& Ip6NameServer2() const;
    inline TBool Ip6DNSAddrFromServer() const;

private:
	void InitialiseConfigL(ESock::CCommsDatIapView* aIapView);
    inline void SetIp6NameServer1(const TIp6Addr& aIp6NameServer1);
    inline void SetIp6NameServer2(const TIp6Addr& aIp6NameServer2);	
    inline void SetIp6DNSAddrFromServer(TBool aValue);
	void GetIp6AddrL(ESock::CCommsDatIapView* aIapView, CommsDat::TMDBElementId aElementId, TIp6Addr& aAddr);

private:
	TIp6Addr iIp6NameServer1;
	TIp6Addr iIp6NameServer2;
	TUint iIp6DNSAddrFromServer : 1;
	};


class TZeroCopyLoopBackAgentProvision : public Meta::SMetaData
/**
Provisioning information that is retrieved dynamically from the Agent on ConnectComplete

Historically used to be a member of CZeroCopyLoopbackProvision, but split off into an SMetaData
provisioning structure of its own right.
*/
	{
public:
	enum 
		{
		EUid = 0x10286A08,
		ETypeId = 2
		};

public:
	inline TInt IsDialIn() const;
	inline void SetIsDialIn(TInt aValue);

	DATA_VTABLE
	
private:
	TInt iDialIn;
	};


class CZeroCopyLoopbackProvision : public CBase, public Meta::SMetaData
/**
Main provisioning info class that aggregates specific provisioning classes.
*/
	{
public:
	enum 
		{
		EUid = 0x10286A08,
		ETypeId = 1
		};

public:
	void InitialiseConfigL(ESock::CCommsDatIapView* aIapView);

public:
	TZeroCopyLoopbackIp6Provision iIp6Provision;
	
	DATA_VTABLE
	};

	
//
// TZeroCopyLoopBackAgentProvision inlines
//

TInt TZeroCopyLoopBackAgentProvision::IsDialIn() const { return iDialIn; }

void TZeroCopyLoopBackAgentProvision::SetIsDialIn(TInt aValue) { iDialIn = aValue; }

//
// TZeroCopyLoopbackIp6Provision inline functions
//

const TIp6Addr& TZeroCopyLoopbackIp6Provision::Ip6NameServer1() const { return iIp6NameServer1; }

const TIp6Addr& TZeroCopyLoopbackIp6Provision::Ip6NameServer2() const { return iIp6NameServer2; }

TBool TZeroCopyLoopbackIp6Provision::Ip6DNSAddrFromServer() const { return iIp6DNSAddrFromServer; }

void TZeroCopyLoopbackIp6Provision::SetIp6NameServer1(const TIp6Addr& aIp6NameServer1)
    {
    iIp6NameServer1 = aIp6NameServer1;
    }

void TZeroCopyLoopbackIp6Provision::SetIp6NameServer2(const TIp6Addr& aIp6NameServer2)
    {
    iIp6NameServer2 = aIp6NameServer2;
    }

void TZeroCopyLoopbackIp6Provision::SetIp6DNSAddrFromServer(TBool aIp6DNSAddrFromServer)
    {
    iIp6DNSAddrFromServer = aIp6DNSAddrFromServer;
    }

#endif // __ZEROCOPY_LOOPBACK_PROVISION_H__
