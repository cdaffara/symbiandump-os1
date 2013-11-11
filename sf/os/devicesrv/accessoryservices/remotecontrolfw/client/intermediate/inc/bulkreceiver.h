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
*/

#ifndef BULKRECEIVER_H
#define BULKRECEIVER_H

#include <e32base.h>
#include <remconinterfaceselector.h>
#include <remcon/messagetype.h>
#include <e32msgqueue.h>
#include "utils.h"

class RRemConBulk;


/**
Active Object for receiving messages from an RRemConBulk 
*/
NONSHARABLE_CLASS(CBulkReceiver)
	: public CActive
	{
public:
	/**
	Factory method.
	@param aObserver Our observer.
	@return Ownership of a new CBulkReceiver.
	*/
	static CBulkReceiver* NewL(CRemConInterfaceSelector& aObserver);
	
	~CBulkReceiver();
	
	/**
	Indicates that the receiver needs to be initialised.
	*/
	TBool InitialisationRequired() const;

	/**
	Initialisation method.
	@param aRemConBulk The session to receive on.
	@param aMaxDataLength The size of the buffer to read operation-associated 
	data into.
	*/
	void InitialiseL(RRemConBulk& aRemConBulk, TUint aMaxDataLength);
	
	/**
	Called when the client wants to receive a message. On completion, 
	notifies the observer and re-issues itself until the client destroys it. 
	*/
	void Receive();

public:
	/**
	Called by the interface selector to connect the client session to the server.
	*/
	void WaitUntilConnectedL();

private: // from CActive
	void RunL();
	void DoCancel();

private:
	CBulkReceiver(CRemConInterfaceSelector& aObserver);
	void ConstructL();
	
	/**
	Places the active object on the current active scheduler.
	*/
	void BindToCurrentThreadL();
	void DoBindToCurrentThreadL();
	
	static TInt StaticConnect(TAny* aSelf);
	TInt Connect();
	
private: // owned
	// These are the members we pass to the inner client API to get messages 
	// out of RemCon.
	TUid			iInterfaceUid;
	TUint			iOperationId;
	RBuf8			iData;
	
	// These are the members related to co-ordinating the receiver which can
	// be in a different thread.
	TThreadId				iThreadId;		//<! The thread ID that the receiver is running in.
	RSpecificThreadCallBack	iCancelCall;
	RSpecificThreadCallBack	iConnectCall;
	

private: // unowned
	RRemConBulk*				iRemConBulk;
	CRemConInterfaceSelector&	iObserver;
	};


#endif // RECEIVER_H
