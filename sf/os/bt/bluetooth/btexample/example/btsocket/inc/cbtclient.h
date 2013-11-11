/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name     : CBtClient.h
* Part of  : ex_btsocket
* Created  : 17/11/2004 by Shane Kearns
* Client "smart connector" class
* Version  :
* 
*
*/



#ifndef CBTCLIENT_H
#define CBTCLIENT_H

#include <e32std.h>
#include <bt_sock.h>
#include <btsdp.h>
#include "exbtutil.h"
#include "mconnectionobserver.h"

/**
A helper class for parsing the protocol descriptor list SDP attribute,
in order to retrieve the protocol and port required for connecting
*/
class TBtClientSdpProtocolListParser : public MSdpAttributeValueVisitor
	{
	public:
		TBtClientSdpProtocolListParser();
	private:
		//virtual functions from MSdpAttributeValueVisitor
		virtual void VisitAttributeValueL(CSdpAttrValue &aValue, TSdpElementType aType);
		virtual void StartListL(CSdpAttrValueList &aList);
		virtual void EndListL();

	public:
		//data members
		TUint iPort;
		TUUID iProtocol;
		enum
			{
			EFoundPort = 0x1,
			EFoundProtocol = 0x2,
			};
		TUint iFoundFlags;
	};

/**
This class implements a simple bluetooth application client.
Its responsibility is to make outgoing bluetooth connections.
Most of the complexity in this class deals with using the SDP agent API to 
retrieve connection parameters from the remote device.
*/
class CBtClient : public CActive, MSdpAgentNotifier
	{
	public:
		IMPORT_C static CBtClient* NewL(const TUUID& aServiceUUID, RSocketServ& aSocketServer, MConnectionObserver& aOwner, TBTServiceSecurity* aSecurityRequirements = NULL);
		IMPORT_C ~CBtClient();

		IMPORT_C void ConnectToRemoteDeviceL(MBluetoothSocketNotifier& aSocketOwner);
		IMPORT_C void ConnectToRemoteDeviceL(TBTDevAddr& aAddr, MBluetoothSocketNotifier& aSocketOwner);
	private:
		void ConstructL(TBTServiceSecurity* aSecurityRequirements);
		CBtClient(const TUUID& aServiceUUID, RSocketServ& aSocketServer, MConnectionObserver& aOwner);

		void ConnectFailed(TInt aError);
		void NewConnection();
		void AttemptConnection(TUUID aProtocol, TUint16 aPort);

		void CallOut(TInt aErr);
		//virtual functions from CActive
		virtual void RunL();
		virtual void DoCancel();

		//Virtual functions from MSdpAgentNotifier
		virtual void NextRecordRequestComplete(TInt aError, TSdpServRecordHandle aHandle, TInt aTotalRecordsCount);
		virtual void AttributeRequestResult(TSdpServRecordHandle aHandle, TSdpAttributeID aAttrID, CSdpAttrValue* aAttrValue);
		virtual void AttributeRequestComplete(TSdpServRecordHandle, TInt aError);

		//Data members
		/** A socket created in order to make a connection */
		CBluetoothSocket *iConnectionSocket;
		/** Handle to the socket server*/
		RSocketServ& iSocketServer;
		/** SDP UUID of the service to find */
		TUUID iServiceUUID;
		/** SDP agent for performing service search */
		CSdpAgent *iSdpAgent;
		/** 48 bit Bluetooth address of the device selected to connect to */
		TBTDevAddr iConnectingToDevice;
		/** Security requirements specified by the application */
		TBTServiceSecurity iSecurityRequirements;
		/** Handle to an application object - callbacks are sent here */
		MConnectionObserver& iOwner;
		/** Helper class for parsing SDP results */
		TBtClientSdpProtocolListParser iParser;
		/** Handle to an application object - the socket is created with this as owner*/
		MBluetoothSocketNotifier* iSocketOwner; //not owned
		/** Buffer for formatting a socket address*/
		TL2CAPSockAddr iAddrL2CAP;
		/** Buffer for formatting a socket address*/
		TRfcommSockAddr iAddrRFCOMM;
	};

#endif //CBTCLIENT_H
