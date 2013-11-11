/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef DATAMONITORMESSAGES_H
#define DATAMONITORMESSAGES_H

#include <es_sock.h>
//#include <comms-infras/api_ext_msg.h>
#include <comms-infras/ss_api_ext.h>
#include <comms-infras/metavirtctor.h>

class MDataMonitorProtocolResp;

namespace ESock
{

const TInt KDataMonitoringRequestMessagesImplementationUid = 0x102822F5;

/**
	Information container for this plugin with respect to data monitoring 
	request messages.

	@internalTechnology
*/
class CDataMonitoringMsgPluginInfo : public CExtItfMsgPluginInfo
	{
public:
	IMPORT_C static CDataMonitoringMsgPluginInfo* NewL(TAny* aParams);
	IMPORT_C const TImplementationProxy* ImplementationProxy() const;
	IMPORT_C TInt ImplementationProxyTableSize() const;
	};

/**
	Data monitoring response message.
	
	@internalTechnology
*/
class CCommsDataMonitorApiExtResp : public CCommsApiExtRespMsg
	{
public:
	virtual void DispatchL(MDataMonitorProtocolResp& aProtocol) = 0;
	};

/**
	Data monitoring request message.

	@internalTechnology
*/
class TCommsDataMonitorApiExtReq : public TCommsApiExtReqMsg
	{
	};
	
/*-------------------------------------------------------
	Client -> Server messages
  -------------------------------------------------------*/

/**
	Request message requesting the notification of the total volume of data 
	transferred in either direction.
	
	@internalTechnology
*/
class TRequestDataTransferred : public TCommsDataMonitorApiExtReq
	{
public:	
	IMPORT_C void DispatchL(NetInterfaces::TInterfaceControl& aIntfCtrl, const TSubSessionUniqueId& aSubSessionUniqueId, Elements::RResponseMsg& aResponseMsg);

	DECLARE_MVIP_CTR(TRequestDataTransferred)
   
	EXPORT_DATA_VTABLE_AND_FN
	};

/**
	Request message requesting the cancellation of notification of the total 
	volume of data transferred in either direction.
	
	@internalTechnology
*/
class TCancelDataTransferredRequest : public TCommsDataMonitorApiExtReq
	{
public:	
	IMPORT_C void DispatchL(NetInterfaces::TInterfaceControl& aIntfCtrl, const TSubSessionUniqueId& aSubSessionUniqueId, Elements::RResponseMsg& aResponseMsg);
	
	DECLARE_MVIP_CTR(TCancelDataTransferredRequest)
   
	EXPORT_DATA_VTABLE_AND_FN
	};

/**
	Request message requesting notification of the volume of data sent having reached 
	or exceeded a specified threshold.
	
	@internalTechnology
*/
class TRequestDataSentNotification : public TCommsDataMonitorApiExtReq
   {
public:
  	IMPORT_C void DispatchL(NetInterfaces::TInterfaceControl& aIntfCtrl, const TSubSessionUniqueId& aSubSessionUniqueId, Elements::RResponseMsg& aResponseMsg);
	
	DECLARE_MVIP_CTR(TRequestDataSentNotification)
	
	inline TUint32 Delta() const;
	inline void SetDelta(TUint32 aDelta);
   
	inline TUint32 SentBytes() const;
	inline void SetSentBytes(TUint32 aSentBytes);

	EXPORT_DATA_VTABLE_AND_FN

protected:	
	TUint32 iDelta;
	TUint32 iSentBytes;
   };
   
/**
	Request message requesting the cancellation of notification of the volume of 
	data sent having reached or exceeded a specified threshold.
	
	@internalTechnology
*/
class TCancelDataSentNotificationRequest : public TCommsDataMonitorApiExtReq
	{
public:
	IMPORT_C void DispatchL(NetInterfaces::TInterfaceControl& aIntfCtrl, const TSubSessionUniqueId& aSubSessionUniqueId, Elements::RResponseMsg& aResponseMsg);
	
	DECLARE_MVIP_CTR(TCancelDataSentNotificationRequest)
	
	EXPORT_DATA_VTABLE_AND_FN
	};
	
/**
	Request message requesting notification of volume of data received having reached 
	or exceeded a specified threshold.
	
	@internalTechnology
*/
class TRequestDataReceivedNotification : public TCommsDataMonitorApiExtReq
	{
public:
  	IMPORT_C void DispatchL(NetInterfaces::TInterfaceControl& aIntfCtrl, const TSubSessionUniqueId& aSubSessionUniqueId, Elements::RResponseMsg& aResponseMsg);
	
	DECLARE_MVIP_CTR(TRequestDataReceivedNotification)
	
	inline TUint32 Delta() const;
	inline void SetDelta(TUint32 aDelta);
   
	inline TUint32 ReceivedBytes() const;
	inline void SetReceivedBytes(TUint32 aReceivedBytes);

	EXPORT_DATA_VTABLE_AND_FN

protected:	
	TUint32 iDelta;
	TUint32 iReceivedBytes;
	};

/**
	Request message requesting the cancellation of notification of the volume of 
	data received having reached or exceeded a specified threshold.
	
	@internalTechnology
*/
class TCancelDataReceivedNotificationRequest : public TCommsDataMonitorApiExtReq
	{
public:
	IMPORT_C void DispatchL(NetInterfaces::TInterfaceControl& aIntfCtrl, const TSubSessionUniqueId& aSubSessionUniqueId, Elements::RResponseMsg& aResponseMsg);
	
	DECLARE_MVIP_CTR(TCancelDataReceivedNotificationRequest)

	EXPORT_DATA_VTABLE_AND_FN
	};
 
#include <comms-infras/datamonitormessages.inl>

} //namespace ESock

#endif   // DATAMONITORMESSAGES_H

