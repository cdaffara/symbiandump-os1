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
//

/**
 @file
 @internalComponent
 @deprecated 
*/
#ifndef OBSERVERS_H_
#define OBSERVERS_H_

#include "suplmessagebase.h"

#include <es_sock.h>
#include <in_sock.h>


/** Used to uniquely identify a session with a SUPL server
 */
typedef TUint32 TSuplLocalSessionId;

/** Socket Writer Observer mixin definition.
This class is implemented by observers of the Socket Writer, which invokes
methods in this class to make the observers aware of events that occurred on their
network sessions.
*/
class MSuplSocketObserver
	{
public:
	
	enum TSuplConnectionError
	{
		EUndefinedConnectionError,
		EFailedToSend,
		EFailedToRead,
		ESocketError,
		EDisconnected,
		EFailedToHandshake,
		EFailedCertCheck,
		ETimeOut,
		EFailedToDecode,
		ETooMuchDataRead,
		ELastErrorCode
	};
	
	/** Notification of success in the stablishment of a connection (per-session so that the
	    connection manager doesn't notify sessions that already knew the connection was up)
	*/
	virtual void Connected(const TSuplLocalSessionId& sessionId, TInetAddr& aAddr) = 0;
	
	/** This method reports to the observer that an error has occurred in the socket that
	   it is not possible to recover from.
	   The observer should relay the error to all the users of this socket.
	*/
	virtual void ConnectionError(const TInt& aError, const TInt aSocketWriterIndex, const TSuplLocalSessionId& sessionId, TBool& aDelete) = 0;
	
	/** This method reports to the observer that a message has been received
		(ownership of the message is transferred)
	*/
	virtual void MessageReceived(CSuplMessageBase* aSuplMessage) = 0;
};

#endif /*OBSERVERS_H_*/
