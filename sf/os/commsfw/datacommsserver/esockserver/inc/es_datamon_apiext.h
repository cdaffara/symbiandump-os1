/*
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
 @internalTechnology
*/

#ifndef ES_DATAMON_APIEXT_H
#define ES_DATAMON_APIEXT_H

#include <es_sock.h>
#include <comms-infras/es_api_ext.h>

/**
    Data monitoring API extension for client.

    This class can actually be used by any client, one running an Active Scheduler
    and one that is not. If an Active Scheduler is not present, clients must wait
    for the TRequestStatus to change.

	@publishedPartner
*/
class RCommsDataMonitoringApiExt : public RCommsApiExtension<EDataMonitoringApiExt>
	{
public:
	IMPORT_C explicit RCommsDataMonitoringApiExt();
	IMPORT_C void Close();

	IMPORT_C void RequestDataTransferred(TDes8& aResponseEventBuf, TRequestStatus& aStatus);
	IMPORT_C void CancelDataTransferredRequest();

	IMPORT_C void RequestDataReceivedNotification(TUint32 aDelta, TUint32 aReceivedBytes, TDes8& aResponseEventBuf, TRequestStatus& aStatus);
	IMPORT_C void CancelDataReceivedNotificationRequest();

	IMPORT_C void RequestDataSentNotification(TUint32 aDelta, TUint32 aSentBytes, TDes8& aResponseEventBuf, TRequestStatus& aStatus);
	IMPORT_C void CancelDataSentNotificationRequest();
	};

/**
	Data monitoring API extension protocol.

	Implemented by classes who wish to register for data monitoring response messages.

	@internalTechnology
*/
class MDataMonitorProtocolResp
	{
public:
	virtual void DataTransferred(TUint32 aReceivedBytes, TUint32 aSentBytes) = 0;
	virtual void DataReceivedNotification(TUint32 aReceivedBytes) = 0;
	virtual void DataSentNotification(TUint32 aSentBytes) = 0;
	};

#endif   // ES_DATAMON_APIEXT_H

