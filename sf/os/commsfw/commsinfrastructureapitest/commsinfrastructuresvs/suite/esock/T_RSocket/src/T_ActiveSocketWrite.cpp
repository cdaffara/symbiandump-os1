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

#include "T_ActiveSocketWrite.h"

/*@{*/
_LIT(KName,	"Write");
/*@}*/

CActiveSocketWrite* CActiveSocketWrite::NewL(RSocket& aSocket, MActiveCallback& aCallback, TInt aPriority)
	{
	CActiveSocketWrite*	ret=new (ELeave) CActiveSocketWrite(aSocket, aCallback, aPriority);
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop();
	return ret;
	}

CActiveSocketWrite::CActiveSocketWrite(RSocket& aSocket, MActiveCallback& aCallback, TInt aPriority)
:	CActiveSocket( KName(), aSocket, aCallback, aPriority )
,	iWriteDataOut(NULL)
	{
	}

CActiveSocketWrite::~CActiveSocketWrite()
	{
	DestroyData();
	}

void CActiveSocketWrite::DestroyData()
	{
	delete iWriteDataOut;
	iWriteDataOut=NULL;

	CActiveSocket::DestroyData();
	}

void CActiveSocketWrite::ExecuteL(HBufC8* aDataOut, const TInt aAsyncErrorIndex)
	{
	DestroyData();

	iWriteDataOut=aDataOut;
	iDataOut.Set(iWriteDataOut->Des());

	iSocket.Write(*iWriteDataOut, iStatus);
	Activate( aAsyncErrorIndex );
	}
