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
// datamonitormessagesecom.h
// 
//

#if !defined(DATAMONMESSAGESECOM_H_INCLUDED)
#define DATAMONMESSAGESECOM_H_INCLUDED

#include <e32std.h>
#include <comms-infras/metadata.h>
#include <comms-infras/api_ext_msg.h>
#include <comms-infras/datamonitormessages.h>

namespace ESock
{

const TInt KDataMonitoringMessagesImplementationUid = 0x102744D3;

/** 
 	Types of the data monitoring message interface represented by CMessage.
	
	@internalTechnology
*/
enum TDataMonitorMsgImplementations
    {
    EDataMonitorMsgAny                 			= Meta::KNetMetaTypeAny, // Any data monitoring message type
    EDataMonitoringMsgDataTransferred			= 0x00000001,
	EDataMonitoringMsgDataSentNotification		= 0x00000002,
    EDataMonitoringMsgDataReceivedNotification	= 0x00000004,
    };

/**
	Data monitoring message factory for response messages.

	@internalComponent
	@released since v9.2
*/
class CDataMonitoringMsgFactory : public CBase
   {
public:
   static Meta::SMetaDataECom* NewMessageL(TAny* aParams);
   };

/*-------------------------------------------------------
	Server -> Client messages
  -------------------------------------------------------*/

/**
	Response message containing the total volume of data transferred in either
	direction.
	
	@internalTechnology
*/
class CDataTransferred : public CCommsDataMonitorApiExtResp
    {
public:
	void DispatchL(MDataMonitorProtocolResp& aProtocol);

	inline static CDataTransferred* NewL();

	// Accessors to member data
	inline TUint ReceivedBytes() const;
	inline void SetReceivedBytes(TUint aReceivedBytes);

	inline TUint SentBytes() const;
	inline void SetSentBytes(TUint aSentBytes);

	DATA_VTABLE

protected:
	TUint iReceivedBytes;
	TUint iSentBytes;
   };

/**
	Response message indicating that the volume of data received has reached or
	exceeded a specified threshold.
	
	@internalTechnology
*/
class CDataReceived : public CCommsDataMonitorApiExtResp
	{
public:
  	void DispatchL(MDataMonitorProtocolResp& aProtocol);
	
	inline static CDataReceived* NewL();

	inline TUint ReceivedBytes() const;
	inline void SetReceivedBytes(TUint aReceivedBytes);

	DATA_VTABLE

protected:
	TUint iReceivedBytes;
	};

/**
	Response message indicating that the volume of data sent has reached or
	exceeded a specified threshold.
	
	@internalTechnology
*/
class CDataSent : public CCommsDataMonitorApiExtResp
	{
public:
  	void DispatchL(MDataMonitorProtocolResp& aProtocol);
	
	inline static CDataSent* NewL();

	inline TUint SentBytes() const;
	inline void SetSentBytes(TUint aSentBytes);

	DATA_VTABLE

protected:
	TUint iSentBytes;
	};
 
#include <comms-infras/datamonmessagesecom.inl>

} //namespace ESock

#endif   // DATAMONMESSAGESECOM_H_INCLUDED

