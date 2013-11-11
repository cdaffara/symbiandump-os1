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


#ifndef __T_ACTIVE_SOCKET_SEND_TO__
#define __T_ACTIVE_SOCKET_SEND_TO__

//	User includes
#include "T_ActiveSocket.h"

//	EPOC includes
#include <in_sock.h>

class CActiveSocketSendTo : public CActiveSocket
	{
public:
	static CActiveSocketSendTo*	NewL(RSocket& aSocket, MActiveCallback& aCallback, TInt aPriority=EPriorityStandard);
	~CActiveSocketSendTo();

	void	ValidateCompletionL(CDataWrapperBase& aTestStep);
	void	ExecuteL(HBufC8* aDataOut, const TInetAddr& aAddr, TUint aFlags, const TInt aAsyncErrorIndex);
	void	ExecuteL(HBufC8* aDataOut, const TInetAddr& aAddr, TUint aFlags, TBool aCheckLength, const TInt aAsyncErrorIndex);

protected:
	CActiveSocketSendTo(RSocket& aSocket, MActiveCallback& aCallback, TInt aPriority);

	void	DestroyData();

private:
	TInetAddr		iAddr;
	TUint			iFlags;
	TBool			iCheckLen;
	TSockXfrLength	iLenExpected;
	TSockXfrLength	iLenReturned;
	HBufC8*			iSendToDataOut;
	TPtrC8			iDataOut;
	};

#endif /* __T_ACTIVE_SOCKET_SEND_TO__ */
