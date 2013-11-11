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
 
#ifndef COMREADWRITE_H
#define COMREADWRITE_H

#include <e32base.h>
#include <c32comm.h>

class RComm;
class RCommServ;
class IrTranpUtil;

class MComReadWriteObserver
	{
public:
	virtual void ReceiveComplete(TDesC8& aBuffer) = 0;
	virtual void SendComplete() = 0;
	virtual void SendError(TInt aError) = 0;
	virtual void ReceiveError(TInt aError) = 0;
	virtual void Error(TInt aError) = 0;
	};


NONSHARABLE_CLASS(CComReadWrite) : public CActive
{
public:
	enum TState {EError, ESend, EReceive, EOpen, EClose, EStop};

public:
	CComReadWrite();
	~CComReadWrite();

	static CComReadWrite* NewL(MComReadWriteObserver& iObserver);
	void ConstructL();
	
	void Open();
	void Close();
	void Send(TDesC8& data);
	void Receive();
	void Signal(TInt aError);

private:
	void RunL();
	void DoCancel();


private:
	
	CComReadWrite(TInt aPrority, MComReadWriteObserver& aObserver);
	RComm iCommPort;
	RCommServ iServer;
	TState iState;

	TBuf8<2048> iRxBuffer;
	TBuf8<2048> iSCEPFrame;
	TBuf8<2048> iTxBuffer;

	MComReadWriteObserver& iObserver;
};

#endif //COMREADWRITE_H
