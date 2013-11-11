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
* CDataTransferredRespMsg
* 
*
*/



/**
 @file
 @internalTechnology
 @prototype
*/
TRIVIAL_ACCESSOR(CDataTransferred, TUint, ReceivedBytes)
TRIVIAL_ACCESSOR(CDataTransferred, TUint, SentBytes)

/*
	CDataSentNotificationRespMsg
*/
TRIVIAL_ACCESSOR(CDataSent, TUint, SentBytes)

/*
	CDataReceivedNotificationRespMsg
*/
TRIVIAL_ACCESSOR(CDataReceived, TUint, ReceivedBytes)


CDataTransferred* CDataTransferred::NewL()
   {
   STypeId typeId = STypeId::CreateSTypeId(KDataMonitoringMessagesImplementationUid, EDataMonitoringMsgDataTransferred);
   return static_cast<CDataTransferred*>(CCommsApiExtRespMsg::NewL(typeId));
   }

CDataSent* CDataSent::NewL()
   {
   STypeId typeId = STypeId::CreateSTypeId(KDataMonitoringMessagesImplementationUid, EDataMonitoringMsgDataSentNotification);
   return static_cast<CDataSent*>(CCommsApiExtRespMsg::NewL(typeId));
   }

CDataReceived* CDataReceived::NewL()
   {
   STypeId typeId = STypeId::CreateSTypeId(KDataMonitoringMessagesImplementationUid, EDataMonitoringMsgDataReceivedNotification);
   return static_cast<CDataReceived*>(CCommsApiExtRespMsg::NewL(typeId));
   }

