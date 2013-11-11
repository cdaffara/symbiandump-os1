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
// Mobility Connection Provider class definition.
// 
//

/**
 @file
 @internalComponent
*/

#ifndef SYMBIAN_MOBILITYCPR_H
#define SYMBIAN_MOBILITYCPR_H

#include <comms-infras/corecpr.h>
#include <comms-infras/ss_mobility_apiext.h>
#include <comms-infras/mobilitycpractivities.h>
#include <comms-infras/mobilitycprstates.h>

_LIT(KMobilityCprPanic, "MobilityCprPanic");


namespace MobilityCprStates
{
	class TStateBase;
}

class CMobilityConnectionProvider : public CCoreConnectionProvider, public ESock::AMobilityProtocolReq,
	ITFHIERARCHY_LINK_1(CMobilityConnectionProvider, CCoreConnectionProvider, ESock::AMobilityProtocolReq)
/** Mobility connection provider

@internalTechnology
*/
    {
	friend class MobilityCprActivities::CMobilityActivity;
	friend class MobilityCprStates::TStateBase;

public:
    typedef ITFHIERARCHY_LINK_1(CMobilityConnectionProvider, CCoreConnectionProvider, ESock::AMobilityProtocolReq) TIfStaticFetcherNearestInHierarchy;

public:
	IMPORT_C static CMobilityConnectionProvider* NewL(ESock::CConnectionProviderFactoryBase& aFactory, const MeshMachine::TNodeActivityMap& aActivityMap);
	IMPORT_C void ReturnInterfacePtrL(AMobilityProtocolReq*& aInterface);

private:
	// AMobilityProtocolReq virtuals
	IMPORT_C virtual void OpenExtensionInterface(TSubSessionUniqueId aClientId, const Messages::TRuntimeCtxId& aControlClient, ESock::CCommsApiExtResponder* aResponder);
	IMPORT_C virtual void CloseExtensionInterface(TSubSessionUniqueId aClientId);

	IMPORT_C virtual void SubscribeForMessage(TSubSessionUniqueId aClientId, CCommsMobilitySrvResp* aRespExt);
	IMPORT_C virtual void CancelSubscription(TSubSessionUniqueId aClientId);
	IMPORT_C virtual void MigrateToPreferredCarrier(TSubSessionUniqueId aClientId);
	IMPORT_C virtual void IgnorePreferredCarrier(TSubSessionUniqueId aClientId);
	IMPORT_C virtual void NewCarrierAccepted(TSubSessionUniqueId aClientId);
	IMPORT_C virtual void NewCarrierRejected(TSubSessionUniqueId aClientId);

protected:
    IMPORT_C CMobilityConnectionProvider(ESock::CConnectionProviderFactoryBase& aFactory, const MeshMachine::TNodeActivityMap& aActivityMap);
    IMPORT_C virtual ~CMobilityConnectionProvider();
    IMPORT_C virtual void ReceivedL(const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aMessage);

private:
	MobilityCprActivities::CMobilityActivity* iMobilityActivity;
    };

#endif //SYMBIAN_MOBILITYCPR_H

