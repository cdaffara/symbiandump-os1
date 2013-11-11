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
//
// Description:
//

/**
 @file
 @internalComponent
*/

#ifndef RECEIVER_H
#define RECEIVER_H

#include <e32base.h>
#include <remconinterfaceselector.h>
#include <remcon/messagetype.h>
#include "remconserver.h"

class RRemCon;

/**
Active Object for receiving messages from an RRemConController or 
RRemConTarget.
*/
NONSHARABLE_CLASS(CReceiver) : public CActive
	{
public:
	/**
	Factory method.
	@param aRemCon The session to receive on.
	@param aObserver Our observer.
	@param aMaxDataLength The size of the buffer to read operation-associated 
	data into.	
	@param aType The type of session aRemCon is. We pass this back up with new 
	messages so the Interface Selector knows how to handle them.
	@return Ownership of a new CReceiver.
	*/
	static CReceiver* NewL(RRemCon& aRemCon, 
		CRemConInterfaceSelector& aObserver, 
		TUint aMaxDataLength,
		TRemConClientType aType);

	/** Destructor. */
	~CReceiver();

public:
	/** Called when the client wants to receive a message. On completion, 
	notifies the observer and re-issues itself until the client destroys it. 
	*/
	void Receive();

private: // from CActive
	void RunL();
	void DoCancel();

private:
	/** Constructor. */
	CReceiver(RRemCon& aRemCon, 
		CRemConInterfaceSelector& aObserver, 
		TRemConClientType aType);

	void ConstructL(TUint aMaxDataLength);

private: // owned
	// These are the members we pass to the inner client API to get messages 
	// out of RemCon.
	TRemConClientReceivePackage iReceivePackage;
	RBuf8 iData;

	const TRemConClientType iType;

private: // unowned
	RRemCon& iRemCon;
	CRemConInterfaceSelector& iObserver;
	};

#endif // RECEIVER_H
