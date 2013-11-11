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
// CSuplConnectionManager.h
// This file defines the class that handles the interface with the 
// network on behalf of the SUPL Protocol Module.
// 
//

/**
 @file
 @internalTechnology
 @deprecated
*/

#ifndef CSUPLCONNECTIONMANAGER_H
#define CSUPLCONNECTIONMANAGER_H

#include <e32base.h>
#include <es_sock.h>

#include <lbs/lbsnetcommon.h>
#include <lbs/lbsnetprotocolbase.h>
#include <lbs/lbshostsettings.h>
#include "observers.h"

class MSuplConnectionManagerObserver;
class CSuplMessageBase;
class CSocketWriterBase;

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
								    const TLbsHostSettingsId& aSlpId,
								    const TServiceType aServiceType,
								    const TUint aSocketWriter);
	CSuplSessionRecord(const CSuplSessionRecord& aSessionRecord);
	~CSuplSessionRecord();

	// Methods for accessing private data
	MSuplConnectionManagerObserver& Observer() const;
	const TSuplLocalSessionId& SessionId() const;
	const TLbsHostSettingsId SlpId() const;
	TUint SocketWriter() const;
	TServiceType ServiceMode() const;
private:
	CSuplSessionRecord(MSuplConnectionManagerObserver& aObserver,
					   const TSuplLocalSessionId& aSessionId,
					   const TLbsHostSettingsId& aSlpId,
					   const TServiceType aServiceType,
					   const TUint aSocketWriter);
private:
	MSuplConnectionManagerObserver& iObserver;
	TSuplLocalSessionId iSessionId;
	TLbsHostSettingsId iSlpId;
	TServiceType iServiceType;
	
	TUint iSocketWriterIndex;
};




/** Connection Manager Observer mixin definition.
This class is implemented by observers of the Connection Manager, which invokes
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


/** Class that provides an interface for starting and progressing SUPL sessions.

This class maintains associations between users (observers) of the class and
a socket connection to a SUPL server. This class takes care of routing outgoing
messages from a user to the socket where its SUPL session is being carried and 
incoming messages from the socket to the user that owns the SUPL session.

This class is used together with MSuplConnectionManagerObserver, which is implemented
by users of CSuplConnectionManager.

Typical users/observers of this class are the SUPL state machines that start the MO-LR and
MT-LR sessions.

This class implements mixin clas MSuplSecureSocketObserver to receive messages from the 
sockets. 
*/
NONSHARABLE_CLASS(CSuplConnectionManager) : public CBase, 
											public MSuplSocketObserver
	{
public:
	IMPORT_C static CSuplConnectionManager* NewL();
	~CSuplConnectionManager();

	IMPORT_C void Connect(const TSuplLocalSessionId& aSessionId, const TLbsHostSettingsId& aSlpId,
	             		  const CSuplSessionRecord::TServiceType& aServiceType,
	             		  MSuplConnectionManagerObserver& aConnMgrObserver);
	IMPORT_C void Disconnect(const TSuplLocalSessionId& aSessionId);
	IMPORT_C void SendMessage(const CSuplMessageBase* aMessage,const TSuplLocalSessionId& aSessionId);
	
	// Methods derived from MSuplSocketObserver
	virtual void Connected(const TSuplLocalSessionId& sessionId, TInetAddr& aAddr);
	virtual void ConnectionError(const TInt& aError, const TInt aSocketWriterIndex, 
								 const TSuplLocalSessionId& sessionId,
								 TBool& aDelete);
	virtual void MessageReceived(CSuplMessageBase* aSuplMessage);
private:
	CSuplConnectionManager();
	void ConstructL();
	
	// Others
	void Connect(const TSuplLocalSessionId& aSessionId, 
				 const TLbsHostSettingsId& aSlpId,
				 const CSuplSessionRecord::TServiceType& aServiceType,
				 MSuplConnectionManagerObserver& aConnMgrObserver,
				 TBool aForceNonSecure);	
	CSuplSessionRecord* FindSession(TLbsHostSettingsId aId, TInt& aIndex);
	CSuplSessionRecord* FindSession(TSuplLocalSessionId aSessionId, TInt& aIndex);
	CSocketWriterBase* FindWriter(TLbsHostSettingsId aId, TInt& aIndex);
	CSocketWriterBase* FindWriter(TInt aId, TInt& aIndex);
	void GetHostSettingsL(TLbsHostSettingsId aId, TLbsHostSettingsSupl& aSettings);
private:
	/** Session with socket server
	*/
	RSocketServ iSocketServ;
	
	/** Session Records
	*/
	RPointerArray<CSuplSessionRecord> iSessionRecords;
	
	/** Socket writers
	 */
	RPointerArray<CSocketWriterBase> iSocketWriters;
	TInt iSocketWriterNextIndex;

	};

#endif // CSUPLCONNECTIONMANAGER_H
