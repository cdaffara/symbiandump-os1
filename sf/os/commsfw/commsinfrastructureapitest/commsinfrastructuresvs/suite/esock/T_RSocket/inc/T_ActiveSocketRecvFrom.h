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


#ifndef __T_ACTIVE_SOCKET_RECV_FROM__
#define __T_ACTIVE_SOCKET_RECV_FROM__

//	User includes
#include "T_ActiveSocket.h"

// Epoc includes
#include <in_sock.h>

class CActiveSocketRecvFrom : public CActiveSocket
	{
public:
	static CActiveSocketRecvFrom*	NewL(RSocket& aSocket, MActiveCallback& aCallback, TInt aPriority=EPriorityStandard);

	void	ValidateCompletionL(CDataWrapperBase& aTestStep);
	void	ExecuteL(HBufC8* aExpectedDataIn, TUint aFlags, TInetAddr aExpectedAddr, TBool aCheckAddress, TBool aCheckPort, TInt aBufferSize, const TInt aAsyncErrorIndex);
	void	ExecuteL(HBufC8* aExpectedDataIn, TUint aFlags, TInetAddr aExpectedAddr, TBool aCheckAddress, TBool aCheckPort, TBool aCheckLength, TInt aBufferSize, const TInt aAsyncErrorIndex);

protected:
	CActiveSocketRecvFrom(RSocket& aSocket, MActiveCallback& aCallback, TInt aPriority);

private:
	TInetAddr		iAddr;
	TInetAddr		iExpectedAddr;
	TBool			iCheckAddress;
	TBool			iCheckPort;
	TUint			iFlags;
	TBool			iCheckLen;
	TSockXfrLength	iLenExpected;
	TSockXfrLength	iLenReturned;
	};

#endif /* __T_ACTIVE_SOCKET_RECV_FROM__ */
