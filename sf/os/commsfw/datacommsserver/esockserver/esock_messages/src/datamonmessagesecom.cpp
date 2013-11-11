// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "datamonmessagesecom.h"

#include <comms-infras/es_datamon_apiext.h>


using namespace ESock;

Meta::SMetaDataECom* CDataMonitoringMsgFactory::NewMessageL(TAny* aParams)
   {
   TInt32 type = reinterpret_cast<TInt32>(aParams);

   switch(type)
      {      
	case EDataMonitoringMsgDataTransferred:
		return new(ELeave) CDataTransferred;
	case EDataMonitoringMsgDataSentNotification:
		return new(ELeave) CDataSent;
	case EDataMonitoringMsgDataReceivedNotification:
		return new(ELeave) CDataReceived;
	default:
      User::Leave(KErrNotFound);
      }
      
   return NULL;
   }   

/*-------------------------------------------------------
	Server -> Client messages
  -------------------------------------------------------*/

START_ATTRIBUTE_TABLE(CDataTransferred, KDataMonitoringMessagesImplementationUid, EDataMonitoringMsgDataTransferred)
	REGISTER_ATTRIBUTE(CDataTransferred, iReceivedBytes, TMeta<TUint>)
	REGISTER_ATTRIBUTE(CDataTransferred, iSentBytes, TMeta<TUint>)
END_ATTRIBUTE_TABLE()

START_ATTRIBUTE_TABLE(CDataSent, KDataMonitoringMessagesImplementationUid, EDataMonitoringMsgDataSentNotification)
	REGISTER_ATTRIBUTE(CDataSent, iSentBytes, TMeta<TUint>)
END_ATTRIBUTE_TABLE()

START_ATTRIBUTE_TABLE(CDataReceived, KDataMonitoringMessagesImplementationUid, EDataMonitoringMsgDataReceivedNotification)
	REGISTER_ATTRIBUTE(CDataReceived, iReceivedBytes, TMeta<TUint>)
END_ATTRIBUTE_TABLE()

void CDataTransferred::DispatchL(MDataMonitorProtocolResp& aProtocol)
	{
	aProtocol.DataTransferred(iReceivedBytes, iSentBytes);
	}

void CDataSent::DispatchL(MDataMonitorProtocolResp& aProtocol)
	{
	aProtocol.DataSentNotification(iSentBytes);
	}
	
void CDataReceived::DispatchL(MDataMonitorProtocolResp& aProtocol)
	{
	aProtocol.DataReceivedNotification(iReceivedBytes);
	}
	

