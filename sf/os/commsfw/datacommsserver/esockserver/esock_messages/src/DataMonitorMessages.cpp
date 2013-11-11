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
//

/**
 @file
 @internalTechnology
*/

#include "datamonitormessages.h"
#include <comms-infras/ss_datamon_apiext.h> // MDataMonitorProtocolReq
#include <elements/responsemsg.h> 		// CResponseMsg

using namespace ESock;
using namespace Elements;
using namespace NetInterfaces;

enum TMobilityRequestMessages
	{
	EDataMonitoringMsgRequestDataTransferred = 0,
	EDataMonitoringMsgCancelDataTransferredRequest,
	EDataMonitoringMsgRequestDataSentNotification,
	EDataMonitoringMsgCancelDataSentNotificationRequest,
	EDataMonitoringMsgRequestDataReceivedNotification,
	EDataMonitoringMsgCancelDataReceivedNotificationRequest,
	};

/*-------------------------------------------------------
	Client -> Server messages
  -------------------------------------------------------*/

EXPORT_START_ATTRIBUTE_TABLE_AND_FN(TRequestDataTransferred, KDataMonitoringRequestMessagesImplementationUid, EDataMonitoringMsgRequestDataTransferred)
END_ATTRIBUTE_TABLE()

EXPORT_START_ATTRIBUTE_TABLE_AND_FN(TCancelDataTransferredRequest, KDataMonitoringRequestMessagesImplementationUid, EDataMonitoringMsgCancelDataTransferredRequest)
END_ATTRIBUTE_TABLE()

EXPORT_START_ATTRIBUTE_TABLE_AND_FN(TRequestDataSentNotification, KDataMonitoringRequestMessagesImplementationUid, EDataMonitoringMsgRequestDataSentNotification)
	REGISTER_ATTRIBUTE(TRequestDataSentNotification, iDelta, TMeta<TUint>)
	REGISTER_ATTRIBUTE(TRequestDataSentNotification, iSentBytes, TMeta<TUint>)
END_ATTRIBUTE_TABLE()

EXPORT_START_ATTRIBUTE_TABLE_AND_FN(TCancelDataSentNotificationRequest, KDataMonitoringRequestMessagesImplementationUid, EDataMonitoringMsgCancelDataSentNotificationRequest)
END_ATTRIBUTE_TABLE()

EXPORT_START_ATTRIBUTE_TABLE_AND_FN(TRequestDataReceivedNotification, KDataMonitoringRequestMessagesImplementationUid, EDataMonitoringMsgRequestDataReceivedNotification)
	REGISTER_ATTRIBUTE(TRequestDataReceivedNotification, iDelta, TMeta<TUint>)
	REGISTER_ATTRIBUTE(TRequestDataReceivedNotification, iReceivedBytes, TMeta<TUint>)
END_ATTRIBUTE_TABLE()

EXPORT_START_ATTRIBUTE_TABLE_AND_FN(TCancelDataReceivedNotificationRequest, KDataMonitoringRequestMessagesImplementationUid, EDataMonitoringMsgCancelDataReceivedNotificationRequest)
END_ATTRIBUTE_TABLE()

EXPORT_C void TRequestDataTransferred::DispatchL(NetInterfaces::TInterfaceControl& aIntfCtrl, const TSubSessionUniqueId& aSubSessionUniqueId, Elements::RResponseMsg& aResponseMsg)
	{
	ADataMonitoringProtocolReq* i = reinterpret_cast<ADataMonitoringProtocolReq*>(aIntfCtrl.FetchInterfaceL(EDataMonitoringApiExt));
	CDataMonitoringResponder* responder = CDataMonitoringResponder::NewL(aResponseMsg);
	i->RequestDataTransferred(responder, aSubSessionUniqueId);
	}

EXPORT_C void TCancelDataTransferredRequest::DispatchL(NetInterfaces::TInterfaceControl& aIntfCtrl, const TSubSessionUniqueId& aSubSessionUniqueId, Elements::RResponseMsg& aResponseMsg)
	{
	ADataMonitoringProtocolReq* i = reinterpret_cast<ADataMonitoringProtocolReq*>(aIntfCtrl.FetchInterfaceL(EDataMonitoringApiExt));
	i->CancelDataTransferredRequest(aSubSessionUniqueId);
	aResponseMsg.Complete(KErrNone);
	}

EXPORT_C void TRequestDataReceivedNotification::DispatchL(NetInterfaces::TInterfaceControl& aIntfCtrl, const TSubSessionUniqueId& aSubSessionUniqueId, Elements::RResponseMsg& aResponseMsg)
	{
	ADataMonitoringProtocolReq* i = reinterpret_cast<ADataMonitoringProtocolReq*>(aIntfCtrl.FetchInterfaceL(EDataMonitoringApiExt));
	CDataMonitoringResponder* responder = CDataMonitoringResponder::NewL(aResponseMsg);
	i->RequestDataReceivedNotification(responder, iDelta, iReceivedBytes, aSubSessionUniqueId);
	}

EXPORT_C void TCancelDataReceivedNotificationRequest::DispatchL(NetInterfaces::TInterfaceControl& aIntfCtrl, const TSubSessionUniqueId& aSubSessionUniqueId, Elements::RResponseMsg& aResponseMsg)
	{
	ADataMonitoringProtocolReq* i = reinterpret_cast<ADataMonitoringProtocolReq*>(aIntfCtrl.FetchInterfaceL(EDataMonitoringApiExt));
	i->CancelDataReceivedNotificationRequest(aSubSessionUniqueId);
	aResponseMsg.Complete(KErrNone);
	}

EXPORT_C void TRequestDataSentNotification::DispatchL(NetInterfaces::TInterfaceControl& aIntfCtrl, const TSubSessionUniqueId& aSubSessionUniqueId, Elements::RResponseMsg& aResponseMsg)
	{
	ADataMonitoringProtocolReq* i = reinterpret_cast<ADataMonitoringProtocolReq*>(aIntfCtrl.FetchInterfaceL(EDataMonitoringApiExt));
	CDataMonitoringResponder* responder = CDataMonitoringResponder::NewL(aResponseMsg);
	i->RequestDataSentNotification(responder, iDelta, iSentBytes, aSubSessionUniqueId);
	}

EXPORT_C void TCancelDataSentNotificationRequest::DispatchL(NetInterfaces::TInterfaceControl& aIntfCtrl, const TSubSessionUniqueId& aSubSessionUniqueId, Elements::RResponseMsg& aResponseMsg)
	{
	ADataMonitoringProtocolReq* i = reinterpret_cast<ADataMonitoringProtocolReq*>(aIntfCtrl.FetchInterfaceL(EDataMonitoringApiExt));
	i->CancelDataSentNotificationRequest(aSubSessionUniqueId);
	aResponseMsg.Complete(KErrNone);
	}

DEFINE_MVIP_CTR(TRequestDataTransferred);
DEFINE_MVIP_CTR(TCancelDataTransferredRequest);
DEFINE_MVIP_CTR(TRequestDataSentNotification);
DEFINE_MVIP_CTR(TCancelDataSentNotificationRequest);
DEFINE_MVIP_CTR(TRequestDataReceivedNotification);
DEFINE_MVIP_CTR(TCancelDataReceivedNotificationRequest);

const TImplementationProxy DataMonitoringReqMsgImplementationTable[] =
	{
	MVIP_CTR_ENTRY(EDataMonitoringMsgRequestDataTransferred, TRequestDataTransferred),
	MVIP_CTR_ENTRY(EDataMonitoringMsgCancelDataTransferredRequest, TCancelDataTransferredRequest),
	MVIP_CTR_ENTRY(EDataMonitoringMsgRequestDataSentNotification, TRequestDataSentNotification),
	MVIP_CTR_ENTRY(EDataMonitoringMsgCancelDataSentNotificationRequest, TCancelDataSentNotificationRequest),
	MVIP_CTR_ENTRY(EDataMonitoringMsgRequestDataReceivedNotification, TRequestDataReceivedNotification),
	MVIP_CTR_ENTRY(EDataMonitoringMsgCancelDataReceivedNotificationRequest, TCancelDataReceivedNotificationRequest),
	};

EXPORT_C CDataMonitoringMsgPluginInfo* CDataMonitoringMsgPluginInfo::NewL(TAny* /* aParams */)
	{
	return new(ELeave) CDataMonitoringMsgPluginInfo();
	}

EXPORT_C const TImplementationProxy* CDataMonitoringMsgPluginInfo::ImplementationProxy() const
	{
	return DataMonitoringReqMsgImplementationTable;
	}

EXPORT_C TInt CDataMonitoringMsgPluginInfo::ImplementationProxyTableSize() const
	{
	return sizeof(DataMonitoringReqMsgImplementationTable) / sizeof(DataMonitoringReqMsgImplementationTable[0]);
	}




