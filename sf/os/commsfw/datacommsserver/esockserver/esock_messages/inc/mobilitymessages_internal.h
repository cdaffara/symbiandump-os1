// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @InternalTechnology
 @released
*/

#ifndef __MOBILITYMESSAGES_INTERNAL_H__
#define __MOBILITYMESSAGES_INTERNAL_H__

#include <comms-infras/ss_api_ext.h>

namespace ESock
{


/**
	Information container for this plugin with respect to mobility
	request messages.

	@internalTechnology
*/
class CMobilityMsgPluginInfo : public CExtItfMsgPluginInfo
	{
public:
	IMPORT_C static CMobilityMsgPluginInfo* NewL(TAny* aParams);
	IMPORT_C const TImplementationProxy* ImplementationProxy() const;
	IMPORT_C TInt ImplementationProxyTableSize() const;
	};

class TCommsMobilityApiExtReqMsg : public TCommsApiExtReqMsg
/**
Mobility request message.

@internalTechnology
@released
*/
	{
	};

class TMigrateToPreferredCarrier : public TCommsMobilityApiExtReqMsg
/**
Migration to preferred carrier is allowed message.

@internalTechnology
@released
*/
   {
public:
	IMPORT_C void DispatchL(NetInterfaces::TInterfaceControl& aIntfCtl, const TSubSessionUniqueId& aSubSessionUniqueId, Elements::RResponseMsg& aResponseMsg);

	DECLARE_MVIP_CTR(TMigrateToPreferredCarrier)
	
	EXPORT_DATA_VTABLE_AND_FN
	};

class TIgnorePreferredCarrier : public TCommsMobilityApiExtReqMsg
/**
Migration to preferred carrier is not allowed message.

@internalTechnology
@released
*/
	{
public:
	IMPORT_C void DispatchL(NetInterfaces::TInterfaceControl& aIntfCtl, const TSubSessionUniqueId& aSubSessionUniqueId, Elements::RResponseMsg& aResponseMsg);

	inline TInt Reason() const;
	inline void SetReason(TInt aReason);

	DECLARE_MVIP_CTR(TIgnorePreferredCarrier)
	
	EXPORT_DATA_VTABLE_AND_FN

protected:
	TInt iReason;
	};

class TNewCarrierAccepted : public TCommsMobilityApiExtReqMsg
/**
New carrier accepted message.

@internalTechnology
@released
*/
   {
public:
	IMPORT_C void DispatchL(NetInterfaces::TInterfaceControl& aIntfCtl, const TSubSessionUniqueId& aSubSessionUniqueId, Elements::RResponseMsg& aResponseMsg);

	DECLARE_MVIP_CTR(TNewCarrierAccepted)

	EXPORT_DATA_VTABLE_AND_FN
	};

class TNewCarrierRejected : public TCommsMobilityApiExtReqMsg
/**
New carrier rejected message.

@internalTechnology
@released
*/
	{	   	
public:
	IMPORT_C void DispatchL(NetInterfaces::TInterfaceControl& aIntfCtl, const TSubSessionUniqueId& aSubSessionUniqueId, Elements::RResponseMsg& aResponseMsg);

	DECLARE_MVIP_CTR(TNewCarrierRejected)
	
	EXPORT_DATA_VTABLE_AND_FN
	};

class TMobilitySubscriptionMsg : public TCommsMobilityApiExtReqMsg
/**
Mobility subscription message.

@internalTechnology
@released
*/
   {
public:
	IMPORT_C void DispatchL(NetInterfaces::TInterfaceControl& aIntfCtl, const TSubSessionUniqueId& aSubSessionUniqueId, Elements::RResponseMsg& aResponseMsg);

	DECLARE_MVIP_CTR(TMobilitySubscriptionMsg)

	EXPORT_DATA_VTABLE_AND_FN
	};

class TMobilitySubscriptionCancelMsg : public TCommsMobilityApiExtReqMsg
/**
Mobility subscription cancellation message.

@internalTechnology
@released
*/
   {
public:
	IMPORT_C void DispatchL(NetInterfaces::TInterfaceControl& aIntfCtl, const TSubSessionUniqueId& aSubSessionUniqueId, Elements::RResponseMsg& aResponseMsg);

	DECLARE_MVIP_CTR(TMobilitySubscriptionCancelMsg)

	EXPORT_DATA_VTABLE_AND_FN
	};

#include <comms-infras/mobilitymessages.inl>
} // namespace ESock

#endif // __MOBILITYMESSAGES_INTERNAL_H__

