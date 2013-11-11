// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// CStartServer Client side header
// Implements an active object to asynchronously start server process,
// 
//

/**
 @file
 @internalAll
*/

#include "CA_StartServer.h"
#include "CS_AsyncConnect.h"

EXPORT_C CStartServer::CStartServer(RAsyncConnectBase& aAsyncConnectBase, const TVersion& aVersion, TInt aAsyncMessageSlots) :
CActive(EPriorityStandard),
iAsyncConnectBase(aAsyncConnectBase),
iVersion(aVersion),
iAsyncMessageSlots(aAsyncMessageSlots)
/**
* Constructor
*
* @internalTechnology
*
* @param aAsyncConnectBase the client side server object
* @param aVersion the desired version of the server
* @param aAsyncMessageSlots number of messages to allocate in client server message queue
*
*/
	{
	CActiveScheduler::Add(this);
	}

EXPORT_C CStartServer::~CStartServer()
	{
	Cancel();
	}

EXPORT_C void CStartServer::Cancel()
   {
	iStartProcess.Cancel();
   }

void CStartServer::DoCancel()
	{//never !!
   __ASSERT_DEBUG( EFalse, User::Panic( KNullDesC, KErrGeneral ) );
	}

void CStartServer::RunL()
	{
   if ( iStartProcess.ErrorCode() == KErrCancel )
      {//request has been canceled
		User::RequestComplete(iClientStatus, KErrCancel);
      }
	else if (!iProcessStarting && (iStatus.Int()==KErrNotFound || iStatus.Int()==KErrServerTerminated))
		{
      iProcessStarting = 1;
		iStartProcess.Start(iName, iStatus);
      SetActive();
		//wait for the server to start
		}
	else if (iProcessStarting && ((iStatus.Int() == KErrAlreadyExists && ++iCounter < iAttempts) || iStatus.Int() == KErrNone))
		{
		Connect(iName);
		//wait for the Connect to complete
		}
	else
		{//complete the connection with an error
		User::RequestComplete(iClientStatus, iStatus.Int());
		}
	}

void CStartServer::Connect(const TDesC& aServerName)
	{
	iAsyncConnectBase.CreateSession(aServerName, iVersion, iAsyncMessageSlots, iStatus);
   iName.Des().Copy( aServerName );
   iProcessStarting = 0;
	SetActive();
	}

EXPORT_C void CStartServer::Connect(const TDesC& aServerName, TRequestStatus& aStatus, TUint aAttempts)
/**
* The Start method
*
* Connect the Handle to the Server and asynchronously starts the server if neccessary
*
* @internalTechnology
*
* @param aServerName name of the server, no file extension
* @param aStatus a request of the client waiting for server to connect
* @param aAttempts how many times to try to connect
*
*/
	{
	iAttempts = aAttempts;
	iClientStatus = &aStatus;
	*iClientStatus = KRequestPending;
	iCounter = 0;
	Connect(aServerName);
	}

