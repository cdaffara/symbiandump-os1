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

#include "T_ActiveSocketIoctl.h"

/*@{*/
_LIT(KName,	"Ioctl");
/*@}*/

CActiveSocketIoctl* CActiveSocketIoctl::NewL(RSocket& aSocket, MActiveCallback& aCallback, TInt aPriority)
	{
	CActiveSocketIoctl*	ret=new (ELeave) CActiveSocketIoctl(aSocket, aCallback, aPriority);
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop();
	return ret;
	}

CActiveSocketIoctl::CActiveSocketIoctl(RSocket& aSocket, MActiveCallback& aCallback, TInt aPriority)
:	CActiveSocket( KName(), aSocket, aCallback, aPriority )
	{
	}

void CActiveSocketIoctl::ExecuteL(TUint aCommand, TUint aFlags, TUint aLevel, const TInt aAsyncErrorIndex)
	{
	iFlags=aFlags;
	iSocket.Ioctl(aCommand, iStatus, &iFlags, aLevel);
	Activate( aAsyncErrorIndex );
	}
