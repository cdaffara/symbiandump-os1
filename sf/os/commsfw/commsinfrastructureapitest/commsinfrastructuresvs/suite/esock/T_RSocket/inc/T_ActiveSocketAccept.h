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


#ifndef __T_ACTIVE_SOCKET_ACCEPT__
#define __T_ACTIVE_SOCKET_ACCEPT__

//	User includes
#include "T_ActiveSocket.h"

class CActiveSocketAccept : public CActiveSocket
	{
public:
	static CActiveSocketAccept*	NewL(RSocket& aSocket, MActiveCallback& aCallback, TInt aPriority=EPriorityStandard);

	void	ExecuteL(RSocket* aBlankSocket, const TInt aAsyncErrorIndex);
	void	ExecuteL(RSocket* aBlankSocket, const TDesC* aExpectedDataIn, TInt aBufferSize, const TInt aAsyncErrorIndex);

protected:
	CActiveSocketAccept(RSocket& aSocket, MActiveCallback& aCallback, TInt aPriority);

private:
	RSocket*	iBlankSocket;
	};

#endif /* __T_ACTIVE_SOCKET_ACCEPT__ */
