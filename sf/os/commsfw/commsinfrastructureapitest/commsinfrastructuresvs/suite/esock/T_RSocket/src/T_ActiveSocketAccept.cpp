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

#include "T_ActiveSocketAccept.h"

/*@{*/
_LIT(KName,	"Accept");
/*@}*/

CActiveSocketAccept* CActiveSocketAccept::NewL(RSocket& aSocket, MActiveCallback& aCallback, TInt aPriority)
	{
	CActiveSocketAccept*	ret=new (ELeave) CActiveSocketAccept(aSocket, aCallback, aPriority);
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop();
	return ret;
	}

CActiveSocketAccept::CActiveSocketAccept(RSocket& aSocket, MActiveCallback& aCallback, TInt aPriority)
:	CActiveSocket( KName(), aSocket, aCallback, aPriority )
,	iBlankSocket(NULL)
	{
	}
	
void CActiveSocketAccept::ExecuteL(RSocket* aBlankSocket, const TInt aAsyncErrorIndex)
	{
	DestroyData();
	iBlankSocket=aBlankSocket;
	iSocket.Accept(*iBlankSocket, iStatus);
	Activate(aAsyncErrorIndex);
	}

void CActiveSocketAccept::ExecuteL(RSocket* aBlankSocket, const TDesC* aExpectedDataIn, TInt aBufferSize, const TInt aAsyncErrorIndex)
	{
	DestroyData();
	CreateDataL(aExpectedDataIn, aBufferSize);

	iBlankSocket=aBlankSocket;

	iSocket.Accept(*iBlankSocket, *iDataIn, iStatus);
	Activate(aAsyncErrorIndex);
	}
