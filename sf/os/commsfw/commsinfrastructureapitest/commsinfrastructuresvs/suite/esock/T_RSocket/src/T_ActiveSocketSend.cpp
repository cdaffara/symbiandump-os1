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

#include "T_ActiveSocketSend.h"

/*@{*/
_LIT(KName,	"Send");
/*@}*/

CActiveSocketSend* CActiveSocketSend::NewL(RSocket& aSocket, MActiveCallback& aCallback, TInt aPriority)
	{
	CActiveSocketSend*	ret=new (ELeave) CActiveSocketSend(aSocket, aCallback, aPriority);
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop();
	return ret;
	}

CActiveSocketSend::CActiveSocketSend(RSocket& aSocket, MActiveCallback& aCallback, TInt aPriority)
:	CActiveSocket( KName(), aSocket, aCallback, aPriority )
,	iCheckLen(EFalse)
,	iSendDataOut(NULL)
	{
	}

CActiveSocketSend::~CActiveSocketSend()
	{
	DestroyData();
	}

void CActiveSocketSend::DestroyData()
	{
	delete iSendDataOut;
	iSendDataOut=NULL;

	CActiveSocket::DestroyData();
	}

void CActiveSocketSend::ValidateCompletionL(CDataWrapperBase& aTestStep)
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

void CActiveSocketSend::ExecuteL(HBufC8* aDataOut, TUint aFlags, const TInt aAsyncErrorIndex)
	{
	DestroyData();

	iSendDataOut=aDataOut;
	iDataOut.Set(iSendDataOut->Des());

	iFlags=aFlags;

	iCheckLen=EFalse;

	iSocket.Send(*iSendDataOut, iFlags, iStatus);
	Activate( aAsyncErrorIndex );
	}

void CActiveSocketSend::ExecuteL(HBufC8* aDataOut, TUint aFlags, TBool aCheckLength, const TInt aAsyncErrorIndex)
	{
	DestroyData();

	iSendDataOut=aDataOut;
	iDataOut.Set(iSendDataOut->Des());

	iFlags=aFlags;

	iCheckLen=aCheckLength;
	iLenExpected=iDataOut.Length();
	iLenReturned=0;

	iSocket.Send(*iSendDataOut, iFlags, iStatus, iLenReturned);
	Activate( aAsyncErrorIndex );
	}
