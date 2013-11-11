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
* Name     : mconnectionobserver.h
* Part of  : ex_btsocket
* Created  : 17/11/2004 by Shane Kearns
* Interface class for client upcalls
* Version  :
* 
*
*/



#ifndef MCONNECTIONOBSERVER_H
#define MCONNECTIONOBSERVER_H

/**
Interface class for owners of CBtService and CBtClient objects
HandleConnectFailed() will be called if an error occured making an outgoing connection
*/
class MConnectionObserver
	{
	public:
		/**
		This function is called when a new connection is successfully created (incoming or outgoing).
		Ownership of aConnectedSocket is transferred, so a memory leak will occur if it is not stored or deleted
		@param aConnectedSocket The socket which has been connected
		*/
		virtual void HandleNewConnection(CBluetoothSocket* aConnectedSocket)=0;
		/**
		This function is called on failure to create an outgoing connection

		*/
		virtual void HandleConnectFailed(TInt aError)=0;
	};

#endif // MCONNECTIONOBSERVER_H
