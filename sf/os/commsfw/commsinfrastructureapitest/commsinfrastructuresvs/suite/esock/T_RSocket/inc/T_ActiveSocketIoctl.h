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


#ifndef __T_ACTIVE_SOCKET_IOCTL__
#define __T_ACTIVE_SOCKET_IOCTL__

//	User includes
#include "T_ActiveSocket.h"

class CActiveSocketIoctl : public CActiveSocket
	{
public:
	static CActiveSocketIoctl*	NewL(RSocket& aSocket, MActiveCallback& aCallback, TInt aPriority=EPriorityStandard);

	void	ExecuteL(TUint aCommand, TUint aFlags, TUint aLevel, const TInt aAsyncErrorIndex);

protected:
	CActiveSocketIoctl(RSocket& aSocket, MActiveCallback& aCallback, TInt aPriority);
	
private:
	TPckgBuf<TUint>	iFlags;
	};

#endif /* __T_ACTIVE_SOCKET_IOCTL__ */
