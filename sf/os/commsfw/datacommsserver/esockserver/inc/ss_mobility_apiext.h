// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

/**
 @file
 @publishedPartner
 @released
*/

#ifndef SS_MOBILITY_APIEXT_H
#define SS_MOBILITY_APIEXT_H

#include <es_sock.h>
#include <comms-infras/ss_api_ext.h>
#include <comms-infras/mobilitymessages.h>

namespace Elements
{
	class CResponseMsg;
}

//CCommsMobilitySrvResp is not part of the ESock namespace for compatibility only
class CCommsMobilitySrvResp : public ESock::CCommsApiExtResponder
/**
This is a server side mobility response API extension.

CCommsMobilitySrvResp is a self deleting object.
Its lifetime originates with a call to SubscribeForMobilityResp(CCommsMobilitySrvResp*)
and terminates with a call to any of its exported public API (i.e. PreferredCarrierAvailable(),
etc).

All exported public APIs are static and take "CCommsMobilitySrvResp*& aThis" as the first
parameter which is deleted and assigned NULL during any call to this API.

@publishedPartner
@released
*/
	{
public:
	IMPORT_C static void PreferredCarrierAvailable(CCommsMobilitySrvResp*& aThis, TAccessPointInfo aOldAP, TAccessPointInfo aNewAP, TBool aIsUpgrade, TBool aIsSeamless);
	IMPORT_C static void NewCarrierActive(CCommsMobilitySrvResp*& aThis, TAccessPointInfo aNewAP, TBool aIsSeamless);
	IMPORT_C static void Error(CCommsMobilitySrvResp*& aThis, TInt aError);

protected:
	explicit CCommsMobilitySrvResp();
	virtual ~CCommsMobilitySrvResp();

private:
	virtual void DoPreferredCarrierAvailable(TAccessPointInfo aOldAP, TAccessPointInfo aNewAP, TBool aIsUpgrade, TBool aIsSeamless) = 0;
	virtual void DoNewCarrierActive(TAccessPointInfo aNewAP, TBool aIsSeamless) = 0;
	};

namespace ESock
{

class AMobilityProtocolReq : public ESock::AIPCClientInfo, public ESock::AExtensionInterfaceBase<ESock::KMobilityRequestMessagesImplementationUid>
/**
Interface to be implemented by the client to support mobility API extension on the server side.

An example of an empty implementation could look like this:

class AExampleMobilityImplementation : public MMobilityProtocolReq
	{
public:
	void MigrateToPreferredCarrier(TSubSessionUniqueId aClientId);
	void IgnorePreferredCarrier(TSubSessionUniqueId aClientId);
	void NewCarrierAccepted(TSubSessionUniqueId aClientId);
	void NewCarrierRejected(TSubSessionUniqueId aClientId);
	void SubscribeForMessage(TSubSessionUniqueId aClientId, CCommsMobilitySrvResp* aRespExt);
	void CancelSubscription(TSubSessionUniqueId aClientId);
	};

void AExampleMobilityImplementation::SubscribeForMessage(TSubSessionUniqueId aClientId, CCommsMobilitySrvResp* aRespExt)
	{
	TAccessPointInfo oldap;
	oldap.SetAccessPoint(1); //example access point
	TAccessPointInfo newap;
	newap.SetAccessPoint(2); //example access point
	TBool upgrade = ETrue;   //example upgeade
	CCommsMobilitySrvResp::PreferredCarrierAvailable(aRespExt,oldap,newap,upgrade);
	}

	...

@publishedPartner
@released
*/
	{
public:
    static const TSupportedCommsApiExt KInterfaceId = EMobilityApiExt;

public:
	virtual void SubscribeForMessage(TSubSessionUniqueId aClientId, CCommsMobilitySrvResp* aRespExt) = 0;
	virtual void CancelSubscription(TSubSessionUniqueId aClientId) = 0;
	virtual void MigrateToPreferredCarrier(TSubSessionUniqueId aClientId) = 0;
	virtual void IgnorePreferredCarrier(TSubSessionUniqueId aClientId) = 0;
	virtual void NewCarrierAccepted(TSubSessionUniqueId aClientId) = 0;
	virtual void NewCarrierRejected(TSubSessionUniqueId aClientId) = 0;

private:
	virtual void CancelExtensionInterface(TSubSessionUniqueId aClientId)
		{
		CancelSubscription(aClientId);
		}
	};

} //namespace ESock


#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <comms-infras/ss_mobility_apiext_internal.h>
#endif


#endif
// SS_MOBILITY_APIEXT_H


