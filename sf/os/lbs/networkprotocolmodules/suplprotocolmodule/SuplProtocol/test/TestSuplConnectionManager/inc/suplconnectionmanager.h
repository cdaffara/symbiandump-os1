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
// This file defines the class that handles the interface with the 
// network on behalf of the SUPL Protocol Module.
// 
//

/**
 @file
 @internalTechnology
 @deprecated
*/

#ifndef CTESTSUPLCONNECTIONMANAGER_H
#define CTESTSUPLCONNECTIONMANAGER_H

#include <e32base.h>
#include <lbs/lbsnetcommon.h>
#include <lbs/lbsnetprotocolbase.h>
#include <lbs/lbshostsettings.h>
#include "suplmessagebase.h"
#include "in_sock.h"

// Fake local IP address
const TUint32 KInetAddr = INET_ADDR(10,10,10,10);

/** Used to uniquely identify a session with a SUPL server
 */
typedef TUint32 TSuplLocalSessionId;

class MSuplConnectionManagerObserver;

/** Maximum number of simultaneous connections through access points.
*/
const TInt KSuplMaxNumberOfConnections = 1;

/** Maximum number of simultaneous SUPL sessions (one or more servers)
*/
const TInt KSuplMaxNumberOfSessions = 8;


NONSHARABLE_CLASS(CSuplSessionRecord) : public CBase
{

public: 

 enum TServiceType
 	{
 	EServiceUndefined,
 	EServiceMolr,
 	EServiceMtlr,
 	};


public:

	static CSuplSessionRecord* NewL(MSuplConnectionManagerObserver& aObserver,
					   const TSuplLocalSessionId& aSessionId,
					   const TUint& aSocketWriterIndex,
					   const TLbsHostSettingsId& aSlpId);
					   
	~CSuplSessionRecord();
	
	CSuplSessionRecord(const CSuplSessionRecord& aSessionRecord);
    // Methods for accessing private data
	MSuplConnectionManagerObserver& Observer() const;
	const TSuplLocalSessionId& SessionId() const;
	const TUint& SocketWriterIndex() const;
	const TLbsHostSettingsId SlpId() const;

private:

	CSuplSessionRecord(MSuplConnectionManagerObserver& aObserver,
					   const TSuplLocalSessionId& aSessionId,
					   const TUint& aSocketWriterIndex,
					   const TLbsHostSettingsId& aSlpId);
	void ConstructL();
	
private:

	MSuplConnectionManagerObserver& iObserver;
	TSuplLocalSessionId iLocalSessionId;
	TUint iSocketWriterIndex;
	TLbsHostSettingsId iSlpId;
	TServiceType iServiceType;
};




/** Connection Manager Observer mixin definition (for State Machines).
This class is implemented by observers of the Connection Manager, which invoke
methods in this class to make the observers aware of events that occurred on their
network sessions.
*/
class MSuplConnectionManagerObserver
	{
public:
	
	enum TSuplConnectionError
	{
		EUndefinedConnectionError,
		EFailedToSend,
		EDecodeMessageFailed,
		EConnectionLost,
		ESecurityInsufficient,
		//Last error code
		ELastErrorCode
	};

public:

	virtual void Connected(const TInetAddr& aAddr) = 0;
	virtual void ConnectionError(const TSuplConnectionError& aError) = 0;
	virtual void MessageReceived(CSuplMessageBase* aSuplMessage) = 0;
};


/** Connection Manager Observer mixin definition (for Test Observer).
This class is implemented by observers of the Connection Manager, which invoke
methods in this class to make the observers aware of events that occurred on their
network sessions.
*/
class MSuplConnectionManagerTestObserver
	{
public:

	virtual void ConnectRequestReceived(const TSuplLocalSessionId& aSessionId, const CSuplSessionRecord::TServiceType& aServiceType, const TLbsHostSettingsId& aSlpId) = 0;
	virtual void DisconnectRequestReceived(const TSuplLocalSessionId& aSessionId) = 0;
	virtual void SendMessageRequestReceived(const CSuplMessageBase* aMessage, const TSuplLocalSessionId& aSessionId) = 0;
};


/** Class that provides an interface for starting and progressing SUPL sessions.

In this implementation for test, the class simply forwards calls from its users (SUPL state
machines) to the test object and it also accepts calls from the test that are forwarded
to the state machines.
*/
NONSHARABLE_CLASS(CSuplConnectionManager) : public CBase
	{
public:

	IMPORT_C static CSuplConnectionManager* NewL();
	IMPORT_C ~CSuplConnectionManager(); // Destructor Called from production code
	IMPORT_C static void Destroy(); // Static destructor called from test code
	
	IMPORT_C void SetTestObserver(MSuplConnectionManagerTestObserver* aTestObserver);


	// --------    INTERFACE METHODS CALLED FROM THE STATE MACHINES    ---------------------------

	IMPORT_C void Connect(const TSuplLocalSessionId& aSessionId, const TLbsHostSettingsId& aSlpId,
	             const CSuplSessionRecord::TServiceType& aServiceType,
	             MSuplConnectionManagerObserver& aConnMgrObserver);
	IMPORT_C void Disconnect(const TSuplLocalSessionId& aSessionId);
	IMPORT_C void SendMessage(const CSuplMessageBase* aMessage,const TSuplLocalSessionId& aSessionId);

	// ---------- 			METHODS CALLED BY TEST CODE    ----------------------------------------

	IMPORT_C void InjectConnectedInd(const TSuplLocalSessionId& sessionId);
	IMPORT_C void InjectConnectionErrorInd(const TSuplLocalSessionId& aSessionId, 
								  const MSuplConnectionManagerObserver::TSuplConnectionError& aError);
								  
	IMPORT_C void InjectMessageReceivedInd(const TSuplLocalSessionId& sessionId, CSuplMessageBase* aSuplMessage);

private:

	CSuplConnectionManager();
	void ConstructL();

private:

	static CSuplConnectionManager* iInstance;
	
	MSuplConnectionManagerTestObserver* iTestObserver;
	
	CSuplSessionRecord* iSessionRecord;
	};

#endif // CTESTSUPLCONNECTIONMANAGER_H
