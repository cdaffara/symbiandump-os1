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

#include "T_ActiveSocketConnect.h"

/*@{*/
_LIT(KName,	"Connect");
/*@}*/

CActiveSocketConnect* CActiveSocketConnect::NewL(RSocket& aSocket, MActiveCallback& aCallback, TInt aPriority)
	{
	CActiveSocketConnect*	ret=new (ELeave) CActiveSocketConnect(aSocket, aCallback, aPriority);
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop();
	return ret;
	}

CActiveSocketConnect::CActiveSocketConnect(RSocket& aSocket, MActiveCallback& aCallback, TInt aPriority)
:	CActiveSocket( KName(), aSocket, aCallback, aPriority )
,	iConnectDataOut(NULL)
	{
	}

CActiveSocketConnect::~CActiveSocketConnect()
	{
	DestroyData();
	}

void CActiveSocketConnect::DestroyData()
	{
	delete iConnectDataOut;
	iConnectDataOut=NULL;

	CActiveSocket::DestroyData();
	}

void CActiveSocketConnect::ExecuteL(const TInetAddr& aAddr, const TInt aAsyncErrorIndex)
	{
	DestroyData();
	iAddr=aAddr;
	iSocket.Connect(iAddr, iStatus);
	Activate( aAsyncErrorIndex );
	}

void CActiveSocketConnect::ExecuteL(const TInetAddr& aAddr, const TDesC& aConnectDataOut, const TDesC* aExpectedDataIn, TInt aBufferSize, const TInt aAsyncErrorIndex)
	{
	DestroyData();
	CreateDataL(aExpectedDataIn, aBufferSize);

	iAddr=aAddr;

	iConnectDataOut=HBufC8::NewL(aConnectDataOut.Length());
	iDataOut.Set(iConnectDataOut->Des());
	iConnectDataOut->Des().Copy(aConnectDataOut);

	iSocket.Connect(iAddr, *iConnectDataOut, *iDataIn, iStatus);
	Activate( aAsyncErrorIndex );
	}
