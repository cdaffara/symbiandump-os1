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
// THIS API IS INTERNAL TO NETWORKING AND IS SUBJECT TO CHANGE AND NOT FOR EXTERNAL USE
// CStartServer Client side header
// Declares an active object to wait for asynchronously started server process,
// perhaps even repeat the session creation attempts
// 
//

/**
 @file 
 @internalTechnology
*/

#ifndef __CA_STARTSERVER_H__
#define __CA_STARTSERVER_H__

#include <comms-infras/startprocess.h>

class RAsyncConnectBase;
/**
 Starts a server process and asynchronously and synchronise with it. 
 
 The process must support synchronisation via Randezvous
 @see RStartServer
 @internalTechnology
**/
class CStartServer : public CActive
	{
public:
   	IMPORT_C CStartServer(RAsyncConnectBase& aAsyncConnectBase, const TVersion& aVersion, TInt aAsyncMessageSlots);
   	IMPORT_C ~CStartServer();
   	IMPORT_C void Connect(const TDesC& aServerName, TRequestStatus& aStatus, TUint aAttempts = 3);
   	IMPORT_C void Cancel();

protected:
   	virtual void DoCancel();
   	virtual void RunL();
   	void Connect(const TDesC& aServerName);

protected:
   	RAsyncConnectBase& iAsyncConnectBase;
   	const TVersion& iVersion;
   	TInt iAsyncMessageSlots;
   	/**
     iStartProcess - RStartProcess to asynchronously start server
     @internalTechnology
   	**/
   	RStartProcess iStartProcess;
   	TUint iAttempts;
   	TRequestStatus* iClientStatus;
   	/**
     iCounter - counts the attempts to start the daemon
     @internalTechnology
   	**/
   	TUint iCounter;
      TInt iProcessStarting;
   	TBufC<KMaxFileNameLength> iName;
};

#endif

