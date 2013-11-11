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

#include "T_ActiveSocketShutdown.h"

/*@{*/
_LIT(KName,	"Shutdown");
/*@}*/

CActiveSocketShutdown* CActiveSocketShutdown::NewL(RSocket& aSocket, MActiveCallback& aCallback, TInt aPriority)
	{
	CActiveSocketShutdown*	ret=new (ELeave) CActiveSocketShutdown(aSocket, aCallback, aPriority);
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop();
	return ret;
	}

CActiveSocketShutdown::CActiveSocketShutdown(RSocket& aSocket, MActiveCallback& aCallback, TInt aPriority)
:	CActiveSocket( KName(), aSocket, aCallback, aPriority )
,	iDisconnectDataOut(NULL)
	{
	}

CActiveSocketShutdown::~CActiveSocketShutdown()
	{
	DestroyData();
	}

void CActiveSocketShutdown::DestroyData()
	{
	delete iDisconnectDataOut;
	iDisconnectDataOut=NULL;

	CActiveSocket::DestroyData();
	}

void CActiveSocketShutdown::ExecuteL(RSocket::TShutdown aHow, const TInt aAsyncErrorIndex)
	{
	DestroyData();
	iSocket.Shutdown(aHow, iStatus);
	Activate( aAsyncErrorIndex );
	}

void CActiveSocketShutdown::ExecuteL(RSocket::TShutdown aHow, const TDesC& aDisconnectDataOut, const TDesC* aExpectedDataIn, TInt aBufferSize, const TInt aAsyncErrorIndex)
	{
	DestroyData();
	CreateDataL(aExpectedDataIn, aBufferSize);

	iDisconnectDataOut=HBufC8::NewL(aDisconnectDataOut.Length());
	iDataOut.Set(iDisconnectDataOut->Des());
	iDisconnectDataOut->Des().Copy(aDisconnectDataOut);

	iSocket.Shutdown(aHow, *iDisconnectDataOut, *iDataIn, iStatus);
	Activate( aAsyncErrorIndex );
	}
