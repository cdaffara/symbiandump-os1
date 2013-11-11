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


#ifndef __T_ACTIVE_SOCKET_READ__
#define __T_ACTIVE_SOCKET_READ__

//	User includes
#include "T_ActiveSocket.h"

class CActiveSocketRead : public CActiveSocket
	{
public:
	static CActiveSocketRead*	NewL(RSocket& aSocket, MActiveCallback& aCallback, TInt aPriority=EPriorityStandard);

	void	ExecuteL(HBufC8* aExpectedDataIn, TInt aBufferSize, const TInt aAsyncErrorIndex);

protected:
	CActiveSocketRead(RSocket& aSocket, MActiveCallback& aCallback, TInt aPriority);
	
	};

#endif /* __T_ACTIVE_SOCKET_READ__ */
