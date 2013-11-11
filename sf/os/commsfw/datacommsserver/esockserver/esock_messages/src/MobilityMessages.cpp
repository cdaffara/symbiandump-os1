// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalTechnology
*/

#include "mobilitymessages.h"
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <comms-infras/mobilitymessages_internal.h>
#include <comms-infras/ss_mobility_apiext_internal.h>
#endif

#include <comms-infras/es_mobility_apiext.h> 	// MMobilityProtocolResp
#include <comms-infras/ss_mobility_apiext.h> 	// MMobilityProtocolReq
#include <elements/responsemsg.h> 			// CResponseMsg

using namespace ESock;
using namespace Elements;
using namespace NetInterfaces;

enum TMobilityRequestMessages
	{
	EMigrateToPreferredCarrier = 0,
	EIgnorePreferredCarrier,
	ENewCarrierAccepted,
	ENewCarrierRejected,
	EMobilityMessageSubscription,
	EMobilityMessageSubscriptionCancel,
	};

/*-------------------------------------------------------
	Client -> Server messages
  -------------------------------------------------------*/

EXPORT_START_ATTRIBUTE_TABLE_AND_FN( TMigrateToPreferredCarrier, KMobilityRequestMessagesImplementationUid, EMigrateToPreferredCarrier )
END_ATTRIBUTE_TABLE()

EXPORT_START_ATTRIBUTE_TABLE_AND_FN( TIgnorePreferredCarrier, KMobilityRequestMessagesImplementationUid, EIgnorePreferredCarrier )
   REGISTER_ATTRIBUTE( TIgnorePreferredCarrier, iReason, TMetaNumber)
END_ATTRIBUTE_TABLE()

EXPORT_START_ATTRIBUTE_TABLE_AND_FN( TNewCarrierAccepted, KMobilityRequestMessagesImplementationUid, ENewCarrierAccepted )
END_ATTRIBUTE_TABLE()

EXPORT_START_ATTRIBUTE_TABLE_AND_FN( TNewCarrierRejected, KMobilityRequestMessagesImplementationUid, ENewCarrierRejected )
END_ATTRIBUTE_TABLE()

EXPORT_START_ATTRIBUTE_TABLE_AND_FN( TMobilitySubscriptionMsg, KMobilityRequestMessagesImplementationUid, EMobilityMessageSubscription )
END_ATTRIBUTE_TABLE()

EXPORT_START_ATTRIBUTE_TABLE_AND_FN( TMobilitySubscriptionCancelMsg, KMobilityRequestMessagesImplementationUid, EMobilityMessageSubscriptionCancel )
END_ATTRIBUTE_TABLE()

EXPORT_C void TMigrateToPreferredCarrier::DispatchL(TInterfaceControl& aIntfCtl, const TSubSessionUniqueId& aSubSessionUniqueId, RResponseMsg& aResponseMsg)
	{
	AMobilityProtocolReq* i = reinterpret_cast<AMobilityProtocolReq*>(aIntfCtl.FetchInterfaceL(EMobilityApiExt));
	i->MigrateToPreferredCarrier(aSubSessionUniqueId);
	aResponseMsg.Complete(KErrNone);
	}

EXPORT_C void TIgnorePreferredCarrier::DispatchL(TInterfaceControl& aIntfCtl, const TSubSessionUniqueId& aSubSessionUniqueId, RResponseMsg& aResponseMsg)
	{
	AMobilityProtocolReq* i = reinterpret_cast<AMobilityProtocolReq*>(aIntfCtl.FetchInterfaceL(EMobilityApiExt));
	i->IgnorePreferredCarrier(aSubSessionUniqueId);
	aResponseMsg.Complete(KErrNone);
	}

EXPORT_C void TNewCarrierAccepted::DispatchL(TInterfaceControl& aIntfCtl, const TSubSessionUniqueId& aSubSessionUniqueId, RResponseMsg& aResponseMsg)
	{
	AMobilityProtocolReq* i = reinterpret_cast<AMobilityProtocolReq*>(aIntfCtl.FetchInterfaceL(EMobilityApiExt));
	i->NewCarrierAccepted(aSubSessionUniqueId);
	aResponseMsg.Complete(KErrNone);
	}

EXPORT_C void TNewCarrierRejected::DispatchL(TInterfaceControl& aIntfCtl, const TSubSessionUniqueId& aSubSessionUniqueId, RResponseMsg& aResponseMsg)
	{
	AMobilityProtocolReq* i = reinterpret_cast<AMobilityProtocolReq*>(aIntfCtl.FetchInterfaceL(EMobilityApiExt));
	i->NewCarrierRejected(aSubSessionUniqueId);
	aResponseMsg.Complete(KErrNone);
	}

EXPORT_C void TMobilitySubscriptionMsg::DispatchL(TInterfaceControl& aIntfCtl, const TSubSessionUniqueId& aSubSessionUniqueId, RResponseMsg& aResponseMsg)
	{
	AMobilityProtocolReq* i = reinterpret_cast<AMobilityProtocolReq*>(aIntfCtl.FetchInterfaceL(EMobilityApiExt));
	i->SubscribeForMessage(aSubSessionUniqueId, CCommsMobilitySrvIpcResponder::NewL(aResponseMsg));
	};

EXPORT_C void TMobilitySubscriptionCancelMsg::DispatchL(TInterfaceControl& aIntfCtl, const TSubSessionUniqueId& aSubSessionUniqueId, RResponseMsg& aResponseMsg)
	{
	AMobilityProtocolReq* i = reinterpret_cast<AMobilityProtocolReq*>(aIntfCtl.FetchInterfaceL(EMobilityApiExt));
	i->CancelSubscription(aSubSessionUniqueId);
	aResponseMsg.Complete(KErrNone);
	};

DEFINE_MVIP_CTR(TMigrateToPreferredCarrier);
DEFINE_MVIP_CTR(TIgnorePreferredCarrier);
DEFINE_MVIP_CTR(TNewCarrierAccepted);
DEFINE_MVIP_CTR(TNewCarrierRejected);
DEFINE_MVIP_CTR(TMobilitySubscriptionMsg);
DEFINE_MVIP_CTR(TMobilitySubscriptionCancelMsg);

const TImplementationProxy MobilityReqMsgImplementationTable[] =
	{
	//NOTE: Entries must be sorted for the binary search to work efficiently!
	MVIP_CTR_ENTRY(EMigrateToPreferredCarrier, TMigrateToPreferredCarrier),
	MVIP_CTR_ENTRY(EIgnorePreferredCarrier, TIgnorePreferredCarrier),
	MVIP_CTR_ENTRY(ENewCarrierAccepted, TNewCarrierAccepted),
	MVIP_CTR_ENTRY(ENewCarrierRejected, TNewCarrierRejected),
	MVIP_CTR_ENTRY(EMobilityMessageSubscription, TMobilitySubscriptionMsg),
	MVIP_CTR_ENTRY(EMobilityMessageSubscriptionCancel, TMobilitySubscriptionCancelMsg),
	};

EXPORT_C CMobilityMsgPluginInfo* CMobilityMsgPluginInfo::NewL(TAny* /* aParams */)
	{
	return new(ELeave) CMobilityMsgPluginInfo();
	}

EXPORT_C const TImplementationProxy* CMobilityMsgPluginInfo::ImplementationProxy() const
	{
	return MobilityReqMsgImplementationTable;
	}

EXPORT_C TInt CMobilityMsgPluginInfo::ImplementationProxyTableSize() const
	{
	return sizeof(MobilityReqMsgImplementationTable) / sizeof(MobilityReqMsgImplementationTable[0]);
	}





