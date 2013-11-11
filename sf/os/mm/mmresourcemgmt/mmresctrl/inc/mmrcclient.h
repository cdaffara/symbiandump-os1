//mmrcclient.h

// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalTechnology
*/

#ifndef RMMRCCLIENT_H
#define RMMRCCLIENT_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <a3f/a3fbase.h>
#include "maudiostreamadaptationobserver.h"

class MAudioContext;
class MLogicalChain;
class MMultimediaResourceControlObserver;
class CMMRCServer;
class CMMRCServerSession;
class CMMRCClientImplementation;
class CAudioContext;

// CLASS DECLARATION
//
// This class opens connection and provides functions to MMRC server.
//
// @lib mmrcclient.lib
//
class RMMRCClient
	{
public:
	IMPORT_C RMMRCClient();

	IMPORT_C virtual ~RMMRCClient();

    //
     // Open connection to server.
     //
     // @param aConnection Interface to server, must not be connected.
     // @param aCallback Callback interface.
     // @return an error code. KErrNone if successful, otherwise one of the system wide error codes.
     //
	IMPORT_C TInt Open(MMultimediaResourceControlObserver& aCallback);

    //
     // Zero is invalid Context id and indicates error.
     // @param aProcessId Process Id.
     // @return TUint32 Context Id.
     //
	IMPORT_C TUint64 LogOn(TProcessId aProcessId);

    //
     // Asynchronous return to ReceiveResourceResponse.
     //
     // @param aMessage Message to be sent.
     // @return an error code. KErrNone if successful, otherwise one of the system wide error codes.
     //
	IMPORT_C TInt SendResourceRequest(MLogicalChain* aCurrent, MLogicalChain* aDesired, CAudioContext* aContext);

    //
     // Synchronous.
     //
     // @return an error code. KErrNone if successful, otherwise one of the system wide error codes.
     //
	IMPORT_C TInt SendResourceConfirmation();

    //
     // Synchronous.
     //
     // @return an error code. KErrNone if successful, otherwise one of the system wide error codes.
     //
	IMPORT_C TInt SendResourceUpdateResult();

    //
     // Synchronous.
     //
     // @return an error code. KErrNone if successful, otherwise one of the system wide error codes.
     //
	IMPORT_C TInt SendResourceErrorNotification();

	//
     // Close connection to server.
	 //
	IMPORT_C void Close();
	//
     // Register a client for notification.
	 //
	IMPORT_C TInt RegisterAsClient(TUid aEventType, const TDesC8& aNotificationRegistrationData);
	
	//
     // Cancel the register of a client for notification.
	 //
	IMPORT_C TInt CancelRegisterAsClient(TUid aEventType);
	//
     // 
	 //
	IMPORT_C TInt WillResumePlay();

	IMPORT_C void ResetMessages();
	
private:
	CMMRCClientImplementation* iImplementation;
	};

#endif  // RMMRCCLIENT_H
