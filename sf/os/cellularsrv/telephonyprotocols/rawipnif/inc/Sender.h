// Copyright (c) 2002-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Defines the active object that controls the BCA Write() requests. 
// 
//

/**
 @file
*/

#ifndef SENDER_H
#define SENDER_H

#include "BcaIoController.h"
#include "Constants.h"


class CSender : public CActive
	{
public:
	CSender(CBcaIoController& aObserver,  TInt aMaxPacketSise);
	static CSender* NewL(CBcaIoController& aObserver,  TInt aMaxPacketSise);
	void ConstructL();
	~CSender();	

	
	

    // Inherited from CActive.
    virtual void RunL();
    virtual void DoCancel();

    void Send(RMBufChain& aPdu);
    inline TInt SendBufferLength();

private: // Unowned data.
    CBcaIoController& iObserver;
    TInt iMaxPacketSize;
    RBuf8 iSendBuffer;
	};

TInt CSender::SendBufferLength()
	{
	return iSendBuffer.MaxLength();
	}

#endif // SENDER_H
