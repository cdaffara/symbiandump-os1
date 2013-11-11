// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Accenture - modified for fshell's btincomingserial.dll
// 
// Description:
// Name     : CBtService.h
// Part of  : ex_btsocket
// Created  : 17/11/2004 by Shane Kearns
// Server "smart connector" class
// Version  :
// 

#ifndef CBTSERVICE_H
#define CBTSERVICE_H

#include <e32std.h>
#include <bt_sock.h>
#include <btsdp.h>

class CBluetoothIncomingSerialConnection;

/**
This class implements a simple bluetooth application service
Its responsibility is to advertise and accept incoming bluetooth connections
*/
NONSHARABLE_CLASS(CBtService) : public CBase, MBluetoothSocketNotifier
	{
	public:
		static CBtService* NewL(const TUUID& aServiceUUID, 
			RSdp& aSdpSession, 
			RSocketServ& aSocketServer, 
			CBluetoothIncomingSerialConnection& aOwner,
			TUint aProtocol,
			const TBTServiceSecurity* aSecurity=NULL);
		~CBtService();

		void AcceptConnection(CBluetoothSocket& aBlankSocket);
	private:
		CBtService(RSocketServ& aSocketServer, 
			CBluetoothIncomingSerialConnection& aOwner, 
			TUint aProtocol
			);
		void ConstructL(const TUUID& aServiceUUID, 
			RSdp& aSdpSession, 
			const TBTServiceSecurity* aSecurity=NULL);
		//Virtual functions from MBluetoothSocketNotifier
		virtual void HandleConnectCompleteL(TInt aErr);
		virtual void HandleAcceptCompleteL(TInt aErr);
		virtual void HandleShutdownCompleteL(TInt aErr);
		virtual void HandleSendCompleteL(TInt aErr);
		virtual void HandleReceiveCompleteL(TInt aErr);
		virtual void HandleIoctlCompleteL(TInt aErr);
		virtual void HandleActivateBasebandEventNotifierCompleteL(TInt aErr, TBTBasebandEventNotification& aEventNotification);

		//Data members
		RSocketServ& iSocketServer;
		CBluetoothSocket *iAcceptorSocket;
		CBluetoothSocket *iConnectionSocket; //not owned
		RSdpDatabase iServiceRecord;
		TSdpServRecordHandle iServiceRecordHandle;
		CBluetoothIncomingSerialConnection& iOwner;
		TUint iProtocol;
	};

#endif //CBTSERVICE_H
