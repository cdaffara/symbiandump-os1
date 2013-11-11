/**
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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



/**
 @file
 @internalComponent
*/

#if !defined(SS_DATAMON_APIEXT_H_INCLUDED)
#define SS_DATAMON_APIEXT_H_INCLUDED

#include <es_sock.h>
#include <comms-infras/ss_api_ext.h>
#include <comms-infras/api_ext_list.h>
#include <comms-infras/datamonitormessages.h>

namespace Elements
{
	class CResponseMsg;
}

namespace ESock
{
class TRequestDataTransferred;
class TRequestDataReceivedNotification;
class TRequestDataSentNotification;

/**
	Server-side data monitoring API extension.

	@internalTechnology
*/
class CDataMonitoringResponder : public CCommsApiExtIpcResponder
	{
	friend class TRequestDataTransferred;
	friend class TRequestDataReceivedNotification;
	friend class TRequestDataSentNotification;
	friend class TLegacyDataMonitoringNotificationRequest;

public:
	IMPORT_C static void DataTransferred(CDataMonitoringResponder*& aThis, TUint32 aReceivedBytes, TUint32 aSentBytes);
	IMPORT_C static void DataSentNotification(CDataMonitoringResponder*& aThis, TUint32 aSentBytes);
	IMPORT_C static void DataReceivedNotification(CDataMonitoringResponder*& aThis, TUint32 aReceivedBytes);
	IMPORT_C static void CancelRequest(CDataMonitoringResponder*& aThis);
	IMPORT_C static void Error(CDataMonitoringResponder*& aThis, TInt aError);

protected:
	CDataMonitoringResponder(Elements::RResponseMsg& aResponseMsg);
	~CDataMonitoringResponder();

	virtual void DoDataTransferred(TUint32 aReceivedBytes, TUint32 aSentBytes);
	virtual void DoDataSentNotification(TUint32 aSentBytes);
	virtual void DoDataReceivedNotification(TUint32 aReceivedBytes);
	virtual void DoCancelRequest();

private:
	IMPORT_C static CDataMonitoringResponder* NewL(Elements::RResponseMsg& aResponseMsg);

private:
	CCommsDataMonitorApiExtResp* iMsg;
	};


/**
	Interface to be implemented by entities wishing to support the retrieval
	of data monitoring information.

	@internalTechnology
*/
class ADataMonitoringProtocolReq : public AExtensionInterfaceBase<KDataMonitoringRequestMessagesImplementationUid>
	{
public:
	static const TSupportedCommsApiExt KInterfaceId = EDataMonitoringApiExt;

	virtual void RequestDataTransferred(CDataMonitoringResponder*& aResponder, TSubSessionUniqueId aClientId) = 0;
	virtual void CancelDataTransferredRequest(TSubSessionUniqueId aClientId) = 0;

	virtual void RequestDataReceivedNotification(CDataMonitoringResponder*& aResponder, TUint32 aDelta, TUint32 aReceivedBytes, TSubSessionUniqueId aClientId) = 0;
	virtual void CancelDataReceivedNotificationRequest(TSubSessionUniqueId aClientId) = 0;

	virtual void RequestDataSentNotification(CDataMonitoringResponder*& aResponder, TUint32 aDelta, TUint32 aSentBytes, TSubSessionUniqueId aClientId) = 0;
	virtual void CancelDataSentNotificationRequest(TSubSessionUniqueId aClientId) = 0;

protected:
	virtual void CancelSubscription(TSubSessionUniqueId aClientId)
		{
		CancelDataTransferredRequest(aClientId);
		CancelDataReceivedNotificationRequest(aClientId);
		CancelDataSentNotificationRequest(aClientId);
		}
	};

} //namespace ESock

#endif // SS_DATAMON_APIEXT_H_INCLUDED


