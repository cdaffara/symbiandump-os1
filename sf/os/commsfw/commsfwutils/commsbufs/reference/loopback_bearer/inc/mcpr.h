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
#ifndef __LEGACY_LOOPBACK_MCPR_H__
#define __LEGACY_LOOPBACK_MCPR_H__

#ifdef SYMBIAN_OLD_EXPORT_LOCATION
#include <ss_mcprnodemessages.h>
#include "agentmcpr.h"
#else
#include <comms-infras/ss_mcprnodemessages.h>
#include <comms-infras/agentmcpr.h>
#endif


#include "mcpr_factory.h"

namespace LegacyLoopbackMCprStates
    {
	class TSendProvision;
    }

class CLegacyLoopbackProvision;
class CLegacyLoopbackProtoAgentHandler;

class CLegacyLoopbackMetaConnectionProvider : public CAgentMetaConnectionProvider
/** PPP meta connection provider

@internalTechnology
@released Since 9.4 */
    {
    friend class LegacyLoopbackMCprStates::TSendProvision;
public:
    typedef CLegacyLoopbackMetaConnectionProvider FactoryType;

	static CLegacyLoopbackMetaConnectionProvider* NewL(ESock::CMetaConnectionProviderFactoryBase& aFactory, const ESock::TProviderInfo& aProviderInfo);
	virtual ~CLegacyLoopbackMetaConnectionProvider();

protected:
    CLegacyLoopbackMetaConnectionProvider(ESock::CMetaConnectionProviderFactoryBase& aFactory, const ESock::TProviderInfo& aProviderInfo);
    void SetAccessPointConfigFromDbL();

protected:
	virtual void ReceivedL(const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aMessage);
	void ConstructL();

private:
	TIp6Addr PresetIP6Addr(ESock::CCommsDatIapView* aReader, CommsDat::TMDBElementId aElementId);	// Duplicated??

private:
	CLegacyLoopbackProtoAgentHandler* iAgentHandler;
    };

#endif // __LEGACY_LOOPBACK_MCPR_H__
