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


#ifndef __T_ACTIVE_SOCKET_WRITE__
#define __T_ACTIVE_SOCKET_WRITE__

//	User includes
#include "T_ActiveSocket.h"

class CActiveSocketWrite : public CActiveSocket
	{
public:
	static CActiveSocketWrite*	NewL(RSocket& aSocket, MActiveCallback& aCallback, TInt aPriority=EPriorityStandard);
	~CActiveSocketWrite();

	void	ExecuteL(HBufC8* aDataOut, const TInt aAsyncErrorIndex);

protected:
	CActiveSocketWrite(RSocket& aSocket, MActiveCallback& aCallback, TInt aPriority);

	void	DestroyData();

private:
	HBufC8*	iWriteDataOut;
	TPtrC8	iDataOut;
	};

#endif /* __T_ACTIVE_SOCKET_WRITE__ */
