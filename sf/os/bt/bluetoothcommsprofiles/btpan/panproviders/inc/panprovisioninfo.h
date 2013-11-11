// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// PAN Provisioning information
// 
//

/**
 @file
 @internalComponent
*/

#ifndef SYMBIAN_PANPROVISIONINFO_H
#define SYMBIAN_PANPROVISIONINFO_H

#include <e32std.h>
#include <bluetooth/logger.h>
#include <comms-infras/metadata.h>

#include <elements/nm_node.h>
#include <comms-infras/ss_common.h>
#include <networking/pktdrv.h>
#include <networking/pktdrv_internal.h>

NONSHARABLE_CLASS(CPanProvisionInfo) : public CBase, public Meta::SMetaData
    {
    enum
        {
        EUid = 0x1028376E,
        ETypeId = 1,
        };
    
    
public:
    inline CPanProvisionInfo();
        
    ~CPanProvisionInfo() {};
    
    inline const Messages::TNodeId& BnepConnectionMgr();
    inline void SetBnepConnectionMgr(const Messages::TNodeId& aConnMgr);
    
    inline CPacketDriverOwner& PacketDriverOwner() const;
    inline void SetPacketDriverOwner(CPacketDriverOwner& aPktDrvOwner);
    
    inline void Reset();

    DATA_VTABLE

private:
    Messages::TNodeId iBnepConnectionMgr;
    CPacketDriverOwner* iPktDrvOwner;
    };



inline CPanProvisionInfo::CPanProvisionInfo()
    {
    }

inline CPacketDriverOwner& CPanProvisionInfo::PacketDriverOwner() const
    {
    #ifdef __FLOG_ACTIVE
    _LIT8(KLogComponent, LOG_COMPONENT_PAN_PROVIDERS);
	#endif

	#ifdef _DEBUG
    PANICCATEGORY("btprovider");
	#endif

    ASSERT_DEBUG(iPktDrvOwner);
    return *iPktDrvOwner;
    }
    
inline void CPanProvisionInfo::SetPacketDriverOwner(CPacketDriverOwner& aPktDrvOwner)
    {
	#ifdef __FLOG_ACTIVE
	_LIT8(KLogComponent, LOG_COMPONENT_PAN_PROVIDERS);
	#endif
	
	#ifdef _DEBUG
	PANICCATEGORY("btprovider");
	#endif

    ASSERT_DEBUG(!iPktDrvOwner); // should only be set once
    iPktDrvOwner = &aPktDrvOwner;
    }


inline const Messages::TNodeId& CPanProvisionInfo::BnepConnectionMgr()
    {
    return iBnepConnectionMgr;
    }
    
inline void CPanProvisionInfo::SetBnepConnectionMgr(const Messages::TNodeId& aConnMgr)
    {
    iBnepConnectionMgr = aConnMgr;
    }

inline void CPanProvisionInfo::Reset()
	{
	iBnepConnectionMgr = Messages::TNodeId::NullId();
	iPktDrvOwner = NULL;
	}

#endif
// SYMBIAN_PANPROVISIONINFO_H
