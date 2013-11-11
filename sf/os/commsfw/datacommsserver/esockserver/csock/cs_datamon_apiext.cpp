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
*/

#include <comms-infras/es_datamon_apiext.h>
#include <comms-infras/datamonitormessages.h>

using namespace ESock;

EXPORT_C RCommsDataMonitoringApiExt::RCommsDataMonitoringApiExt()
	{
	}

EXPORT_C void RCommsDataMonitoringApiExt::Close()
	{
	RCommsApiExtensionBase::Close();
	}

EXPORT_C void RCommsDataMonitoringApiExt::RequestDataTransferred(TDes8& aResponseEventBuf, TRequestStatus& aStatus)
	{
	TRequestDataTransferred msg;

	SendRequest(msg, aResponseEventBuf, aStatus);
	}
	
EXPORT_C void RCommsDataMonitoringApiExt::CancelDataTransferredRequest()
	{
	TCancelDataTransferredRequest msg;

	SendMessage(msg);
	}

EXPORT_C void RCommsDataMonitoringApiExt::RequestDataReceivedNotification(TUint32 aDelta, TUint32 aReceivedBytes, TDes8& aResponseEventBuf, TRequestStatus& aStatus)
	{
	TRequestDataReceivedNotification msg;

	msg.SetDelta(aDelta);
	msg.SetReceivedBytes(aReceivedBytes);

	SendRequest(msg, aResponseEventBuf, aStatus);
	}
	
EXPORT_C void RCommsDataMonitoringApiExt::CancelDataReceivedNotificationRequest()
	{
	TCancelDataReceivedNotificationRequest msg;

	SendMessage(msg);
	}

EXPORT_C void RCommsDataMonitoringApiExt::RequestDataSentNotification(TUint32 aDelta, TUint32 aSentBytes, TDes8& aResponseEventBuf, TRequestStatus& aStatus)
	{
	TRequestDataSentNotification msg;

	msg.SetDelta(aDelta);
	msg.SetSentBytes(aSentBytes);

	SendRequest(msg, aResponseEventBuf, aStatus);
	}

EXPORT_C void RCommsDataMonitoringApiExt::CancelDataSentNotificationRequest()
	{
	TCancelDataSentNotificationRequest msg;

	SendMessage(msg);
	}

