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
// RawIP MCPR
// 
//

/**
 @file
 @internalComponent
*/

#ifndef SYMBIAN_RAWIPMCPR_H
#define SYMBIAN_RAWIPMCPR_H

#include <comms-infras/ss_mcprnodemessages.h>
#include <comms-infras/agentmcpr.h>
#include "rawipmcprfactory.h"
#include <comms-infras/ss_log.h>

#define KRawIPMCprTag KESockMetaConnectionTag
_LIT8(KRawIPMCprSubTag, "rawipmcpr");

namespace RawIPMCprStates
    {
    class TCreateProvisionInfo;
    }

class CRawIpAgentHandler;

class CRawIpMetaConnectionProvider : public CAgentMetaConnectionProvider
/** PPP meta connection provider

@internalTechnology
@released Since 9.4 */
    {
    friend class RawIPMCprStates::TCreateProvisionInfo;
public:
    typedef CRawIpMetaConnectionProviderFactory FactoryType;

	static CRawIpMetaConnectionProvider* NewL(ESock::CMetaConnectionProviderFactoryBase& aFactory, const ESock::TProviderInfo& aProviderInfo);
	virtual ~CRawIpMetaConnectionProvider();

protected:
    CRawIpMetaConnectionProvider(ESock::CMetaConnectionProviderFactoryBase& aFactory, const ESock::TProviderInfo& aProviderInfo);
    void SetAccessPointConfigFromDbL();

protected:
	virtual void ReceivedL(const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aMessage);
	void ConstructL();

private:
	CRawIpAgentHandler* iAgentHandler;
    };

#endif //SYMBIAN_RAWIPMCPR_H
