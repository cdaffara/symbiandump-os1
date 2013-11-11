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


#ifndef __T_ACTIVE_SOCKET_RECV__
#define __T_ACTIVE_SOCKET_RECV__

//	User includes
#include "T_ActiveSocket.h"

class CActiveSocketRecv : public CActiveSocket
	{
public:	static CActiveSocketRecv*	NewL(RSocket& aSocket, MActiveCallback& aCallback, TInt aPriority=EPriorityStandard);

	void	ValidateCompletionL(CDataWrapperBase& aTestStep);
	void	ExecuteL(HBufC8* aExpectedDataIn, TUint aFlags, TInt aBufferSize, const TInt aAsyncErrorIndex);
	void	ExecuteL(HBufC8* aExpectedDataIn, TUint aFlags, TBool aCheckLength, TInt aBufferSize, const TInt aAsyncErrorIndex);

protected:
	CActiveSocketRecv(RSocket& aSocket, MActiveCallback& aCallback, TInt aPriority);

private:
	TUint			iFlags;
	TBool			iCheckLen;
	TSockXfrLength	iLenExpected;
	TSockXfrLength	iLenReturned;
	};

#endif /* __T_ACTIVE_SOCKET_RECV__ */
