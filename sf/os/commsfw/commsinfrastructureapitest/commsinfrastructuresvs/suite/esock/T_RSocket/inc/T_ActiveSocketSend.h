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


#ifndef __T_ACTIVE_SOCKET_SEND__
#define __T_ACTIVE_SOCKET_SEND__

//	User includes
#include "T_ActiveSocket.h"

class CActiveSocketSend : public CActiveSocket
	{
public:
	static CActiveSocketSend*	NewL(RSocket& aSocket, MActiveCallback& aCallback, TInt aPriority=EPriorityStandard);
	~CActiveSocketSend();

	void	ValidateCompletionL(CDataWrapperBase& aTestStep);
	void	ExecuteL(HBufC8* aDataOut, TUint aFlags, const TInt aAsyncErrorIndex);
	void	ExecuteL(HBufC8* aDataOut, TUint aFlags, TBool aCheckLength, const TInt aAsyncErrorIndex);

protected:
	CActiveSocketSend(RSocket& aSocket, MActiveCallback& aCallback, TInt aPriority);

	void	DestroyData();
	
private:
	TUint			iFlags;
	TBool			iCheckLen;
	TSockXfrLength	iLenExpected;
	TSockXfrLength	iLenReturned;
	HBufC8*			iSendDataOut;
	TPtrC8			iDataOut;
	};

#endif /* __T_ACTIVE_SOCKET_SEND__ */
