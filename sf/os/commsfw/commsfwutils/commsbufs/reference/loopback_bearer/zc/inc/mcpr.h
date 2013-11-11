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
#ifndef __ZEROCOPY_LOOPBACK_MCPR_H__
#define __ZEROCOPY_LOOPBACK_MCPR_H__

#ifdef SYMBIAN_OLD_EXPORT_LOCATION
#include <ss_mcprnodemessages.h>
#include "agentmcpr.h"
#else
#include <comms-infras/ss_mcprnodemessages.h>
#include <comms-infras/agentmcpr.h>
#endif


#include "mcpr_factory.h"

namespace ZeroCopyLoopbackMCprStates
    {
	class TSendProvision;
    }

class CZeroCopyLoopbackProvision;
class CZeroCopyLoopbackProtoAgentHandler;

class CZeroCopyLoopbackMetaConnectionProvider : public CAgentMetaConnectionProvider
/** PPP meta connection provider

@internalTechnology
@released Since 9.4 */
    {
    friend class ZeroCopyLoopbackMCprStates::TSendProvision;
public:
    typedef CZeroCopyLoopbackMetaConnectionProvider FactoryType;

	static CZeroCopyLoopbackMetaConnectionProvider* NewL(ESock::CMetaConnectionProviderFactoryBase& aFactory, const ESock::TProviderInfo& aProviderInfo);
	virtual ~CZeroCopyLoopbackMetaConnectionProvider();

protected:
    CZeroCopyLoopbackMetaConnectionProvider(ESock::CMetaConnectionProviderFactoryBase& aFactory, const ESock::TProviderInfo& aProviderInfo);
    void SetAccessPointConfigFromDbL();

protected:
	virtual void ReceivedL(const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aMessage);
	void ConstructL();

private:
	TIp6Addr PresetIP6Addr(ESock::CCommsDatIapView* aReader, CommsDat::TMDBElementId aElementId);	// Duplicated??

private:
	CZeroCopyLoopbackProtoAgentHandler* iAgentHandler;
    };

#endif // __ZEROCOPY_LOOPBACK_MCPR_H__
