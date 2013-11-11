/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/


#ifndef __T_ACTIVE_SOCKET_CONNECT__
#define __T_ACTIVE_SOCKET_CONNECT__

//	User includes
#include "T_ActiveSocket.h"

//	EPOC includes
#include <in_sock.h>

class CActiveSocketConnect : public CActiveSocket
	{
public:
	static CActiveSocketConnect*	NewL(RSocket& aSocket, MActiveCallback& aCallback, TInt aPriority=EPriorityStandard);
	~CActiveSocketConnect();

	void	ExecuteL(const TInetAddr& aAddr, const TInt aAsyncErrorIndex);
	void	ExecuteL(const TInetAddr& AAddr, const TDesC& aConnectDataOut, const TDesC* aExpectedDataIn, TInt aBufferSize, const TInt aAsyncErrorIndex);

protected:
	CActiveSocketConnect(RSocket& aSocket, MActiveCallback& aCallback, TInt aPriority);
	
	void	DestroyData();

private:
	TInetAddr	iAddr;
	HBufC8*		iConnectDataOut;
	TPtrC8		iDataOut;
	};

#endif /* __T_ACTIVE_SOCKET_CONNECT__ */
