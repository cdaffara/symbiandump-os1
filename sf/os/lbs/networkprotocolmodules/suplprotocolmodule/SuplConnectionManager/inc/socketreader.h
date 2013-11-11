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

#ifndef SOCKETREADER_H_
#define SOCKETREADER_H_
#include <e32base.h>
#include <es_sock.h>
#include <in_sock.h>
#include <securesocket.h>

#include "observers.h"

// Forwards
class CSuplAsn1Decoder;

/** Size of buffer used for reading SUPL messages from a socket
*/
const TInt KSuplMaxMessageLength = 8192; // Enough to keep all the assistance data

class CSocketReader : public CActive
	{
public:
	static CSocketReader* NewL(RSocket& aSocket, MSuplSocketObserver& aObserver, TUint aCallbackId);
	~CSocketReader();
	
	// CActive
	void RunL();
	void DoCancel();
	void RunError();
private:
	// Constructors
	CSocketReader(RSocket& aSocket, MSuplSocketObserver& aObserver, TUint aCallbackId);
	void ConstructL();
	
	// Methods
	void SuplMessageLength();
	void IssueRead();
	void FireObservation();
private:
	// Socket to listen on
	RSocket& iSocket;

	// Observer for calling back when message received
	MSuplSocketObserver& iObserver;
	
	// Data buffer for message
	RBuf8 iBuffer;
	
	// Size of data read
	TSockXfrLength iBufferRead;
	
	// Is this the first fragment
	TBool iIsFirstFragment;
	
	// Total size of the message we are currently reading
	TUint iCurrentMessageSize;
	
	// Current read data
	TUint iReadSoFar;
	
	// Decoder
	CSuplAsn1Decoder* iDecoder;   
	
	// Ptr into buffer
	TPtr8 iPtr;
	
	// Id for callbacks
	TUint iCallbackId;
	};


class CSecureSocketReader : public CActive
	{
public:
	static CSecureSocketReader* NewL(CSecureSocket& aSocket, MSuplSocketObserver& aObserver, TUint aCallbackId);
	~CSecureSocketReader();
	
	// CActive
	void RunL();
	void DoCancel();
	void RunError();	
private:
	// Constructors
	CSecureSocketReader(CSecureSocket& aSocket, MSuplSocketObserver& aObserver, TUint aCallbackId);
	void ConstructL();
	
	// Methods
	void SuplMessageLength();
	void IssueRead();
	void FireObservation();	
private:
	// Socket to listen on
	CSecureSocket& iSocket;

	// Observer for calling back when message received
	MSuplSocketObserver& iObserver;
	
	// Data buffer for message
	RBuf8 iBuffer;
	
	// Size of data read
	TSockXfrLength iBufferRead;
	
	// Is this the first fragment
	TBool iIsFirstFragment;
	
	// Total size of the message we are currently reading
	TUint iCurrentMessageSize;
	
	// Current read data
	TUint iReadSoFar;
	
	// Decoder
	CSuplAsn1Decoder* iDecoder;
	
	// Ptr into buffer
	TPtr8 iPtr;
	
	// Id for callbacks
	TUint iCallbackId;
	};

#endif /*SOCKETREADER_H_*/
