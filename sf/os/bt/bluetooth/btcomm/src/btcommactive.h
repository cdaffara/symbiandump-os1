// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef BTCOMMACTIVE_H__
#define BTCOMMACTIVE_H__

_LIT(KConnectorThreadName, "ESock Connector Thread ");

NONSHARABLE_CLASS(CBTPortBuffer) : public CCirBufBase
/** 
	Circular buffer for received data.
	Add and Remove of single byte as well
	as multiple bytes.
*/
	{
public:
	static CBTPortBuffer *NewL(CBTPortProxy *aParent,TInt aBufSize);
	~CBTPortBuffer();
	TInt Add(const TUint8* aPtr,TInt aCount);
	TInt Remove(TUint8* aPtr,TInt aCount);
	TInt ScanForTerminator(TReadTerminator& aTerminator);
private:
	CBTPortBuffer(TInt aBufSize);
	void ConstructL(CBTPortProxy *aParent);
	TInt ScanForTerminatorSequence(TReadTerminator& aTerminator,TText8 aChar);
private:
	CBTPortProxy *iPortProxy;
	TInt iCircBufSize;
	};

//	** BT CSY ACTIVE OBJECTS **

NONSHARABLE_CLASS(CBTPortLocker) : public CActive
	{
public:
	static CBTPortLocker *NewL(CBTPortProxy *aParent);
	~CBTPortLocker();
	void RunL();
	void LockSession();
	void UnlockSession();
	void DoCancel();
	TBool IsSessionLocked();
private:
	CBTPortLocker();
	void InitL(CBTPortProxy *aParent);
private:
	CBTPortProxy *iPortProxy;
	TBool iLockOn; // indicates whether we already have a locked session with ESock
	};

NONSHARABLE_CLASS(CBTPortWriter) : public CActive
	{
public:
	static CBTPortWriter *NewL(CBTPortProxy *aParent);
	~CBTPortWriter();
	void RunL();
	void QueueWrite(const TDesC8& aData);
	void DoCancel();
private:
	CBTPortWriter();
	void InitL(CBTPortProxy *aParent);
private:
	CBTPortProxy *iPortProxy;
	};

NONSHARABLE_CLASS(CBTPortReader) : public CActive
	{
public:
	static CBTPortReader *NewL(CBTPortProxy *aParent);
	~CBTPortReader();
	void RunL();
	void QueueRead();
	void DoCancel();
	void StopReading();
	void StartReading();
	TBool IsReading();
private:
	CBTPortReader();
	void InitL(CBTPortProxy *aParent);
private:
	TBool			iKeepReading;
	HBufC8*			iLocalReadBuffer;
	TPtr8			iLocalReadBufferPtr;
	CBTPortProxy*	iPortProxy;
	TSockXfrLength	iPlen;
	};

NONSHARABLE_CLASS(CSockServConnector) : public CActive
	{
public:
	/**
	 Standard factory function.
	 @return a fully initialised CSockServConnector
	 */
	static CSockServConnector* NewL(RSocketServ& aSockServ);
	
	/**
	 Destructor
	 */
	~CSockServConnector();
	
	/**
	 Initiate connection to ESock.  To do this with no threat
	 of deadlock a new thread is spawned which connects up
	 the RSocketServ and shares it with this thread.  When the
	 callers request is completed the ESock session is ready to
	 use.
	 @param aSockServ ESock session to connect, ready to use on 
	 completion.
	 @param aStatus Callers iStatus, contains result of connect on 
	 completion.
	 */
	 void SockServConnect(TRequestStatus& aStatus);
	 
private:
	/**
	 Constructor
	 */
	CSockServConnector(RSocketServ& aSockServ);
	
	/**
	 Second phase construction.
	 */
	void ConstructL();
	
	/**
	 Implementation of RunL() function inherited from CActive.
	 When this is called the ESock Connect() has completed, this
	 informs user of the result and attaches the session to this
	 thread.
	 */
	void RunL();
	
	/** 
	 On leave from RunL notifies client of error and closes handle
	 on connector thread.
	 */
	TInt RunError(TInt aError);
	
	/**
	 Implementation of DoCancel() function inherited from CActive.
	 */
	void DoCancel();
	
	/** 
	 Completes client request and closes connector thread handle.
	 @param aError Error code used to complete client request
	 */
	 void NotifyAndClose(TInt aError);
	
private:
	RSocketServ& iSockServ;
	RThread	iConnectorThread;
	TRequestStatus* iClientStatus;
	};

#endif
