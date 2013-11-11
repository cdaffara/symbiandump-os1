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

#include "T_ActiveSocketSendTo.h"

/*@{*/
_LIT(KName,	"SendTo");
/*@}*/

CActiveSocketSendTo* CActiveSocketSendTo::NewL(RSocket& aSocket, MActiveCallback& aCallback, TInt aPriority)
	{
	CActiveSocketSendTo*	ret=new (ELeave) CActiveSocketSendTo(aSocket, aCallback, aPriority);
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop();
	return ret;
	}

CActiveSocketSendTo::CActiveSocketSendTo(RSocket& aSocket, MActiveCallback& aCallback, TInt aPriority)
:	CActiveSocket( KName(), aSocket, aCallback, aPriority )
,	iCheckLen(EFalse)
,	iSendToDataOut(NULL)
	{
	}

CActiveSocketSendTo::~CActiveSocketSendTo()
	{
	DestroyData();
	}

void CActiveSocketSendTo::DestroyData()
	{
	delete iSendToDataOut;
	iSendToDataOut=NULL;

	CActiveSocket::DestroyData();
	}

void CActiveSocketSendTo::ValidateCompletionL(CDataWrapperBase& aTestStep)
	{
	if ( iCheckLen )
		{
		TInt	lenExpected=iLenExpected();
		TInt	lenReturned=iLenReturned();
		if ( lenExpected!=lenReturned )
			{
			aTestStep.ERR_PRINTF3(_L("Expected length %d does not match returned %d"), lenExpected, lenReturned);
			aTestStep.SetBlockResult(EFail);
			}
		}
	CActiveSocket::ValidateCompletionL(aTestStep);
	}

void CActiveSocketSendTo::ExecuteL(HBufC8* aDataOut, const TInetAddr& aAddr, TUint aFlags, const TInt aAsyncErrorIndex)
	{
	DestroyData();

	iSendToDataOut=aDataOut;
	iDataOut.Set(iSendToDataOut->Des());

	iAddr=aAddr;
	iFlags=aFlags;

	iCheckLen=EFalse;

	iSocket.SendTo(*iSendToDataOut, iAddr, iFlags, iStatus);
	Activate( aAsyncErrorIndex );
	}

void CActiveSocketSendTo::ExecuteL(HBufC8* aDataOut, const TInetAddr& aAddr, TUint aFlags, TBool aCheckLength, const TInt aAsyncErrorIndex)
	{
	DestroyData();

	iSendToDataOut=aDataOut;
	iDataOut.Set(iSendToDataOut->Des());

	iAddr=aAddr;
	iFlags=aFlags;

	iCheckLen=aCheckLength;
	iLenExpected=iDataOut.Length();
	iLenReturned=0;

	iSocket.SendTo(*iSendToDataOut, iAddr, iFlags, iStatus, iLenReturned);
	Activate( aAsyncErrorIndex );
	}
