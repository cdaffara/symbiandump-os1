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


#ifndef __T_ACTIVE_SOCKET_SHUTDOWN__
#define __T_ACTIVE_SOCKET_SHUTDOWN__

//	User includes
#include "T_ActiveSocket.h"

class CActiveSocketShutdown : public CActiveSocket
	{
public:
	static CActiveSocketShutdown*	NewL(RSocket& aSocket, MActiveCallback& aCallback, TInt aPriority=EPriorityStandard);
	~CActiveSocketShutdown();

	void	ExecuteL(RSocket::TShutdown aHow, const TInt aAsyncErrorIndex);
	void	ExecuteL(RSocket::TShutdown aHow, const TDesC& aDisconnectDataOut, const TDesC* aExpectedDataIn, TInt aBufferSize, const TInt aAsyncErrorIndex);

protected:
	CActiveSocketShutdown(RSocket& aSocket, MActiveCallback& aCallback, TInt aPriority);

	void	DestroyData();

private:
	HBufC8*	iDisconnectDataOut;
	TPtrC8	iDataOut;
	};

#endif /* __T_ACTIVE_SOCKET_SHUTDOWN__ */
