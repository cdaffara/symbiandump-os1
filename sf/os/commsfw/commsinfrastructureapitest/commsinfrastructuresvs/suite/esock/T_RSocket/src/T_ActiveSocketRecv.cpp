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

#include "T_ActiveSocketRecv.h"

/*@{*/
_LIT(KName,	"Recv");
/*@}*/

CActiveSocketRecv* CActiveSocketRecv::NewL(RSocket& aSocket, MActiveCallback& aCallback, TInt aPriority)
	{
	CActiveSocketRecv*	ret=new (ELeave) CActiveSocketRecv(aSocket, aCallback, aPriority);
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop();
	return ret;
	}

CActiveSocketRecv::CActiveSocketRecv(RSocket& aSocket, MActiveCallback& aCallback, TInt aPriority)
:	CActiveSocket( KName(), aSocket, aCallback, aPriority )
,	iCheckLen(EFalse)
	{
	}

void CActiveSocketRecv::ValidateCompletionL(CDataWrapperBase& aTestStep)
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

void CActiveSocketRecv::ExecuteL(HBufC8* aExpectedDataIn, TUint aFlags, TInt aBufferSize, const TInt aAsyncErrorIndex)
	{
	DestroyData();
	CreateDataL(aExpectedDataIn, aBufferSize);

	iFlags=aFlags;

	iCheckLen=EFalse;

	iSocket.Recv(*iDataIn, iFlags, iStatus);
	Activate( aAsyncErrorIndex );
	}

void CActiveSocketRecv::ExecuteL(HBufC8* aExpectedDataIn, TUint aFlags, TBool aCheckLength, TInt aBufferSize, const TInt aAsyncErrorIndex)
	{
	DestroyData();
	CreateDataL(aExpectedDataIn, aBufferSize);

	iFlags=aFlags;

	iCheckLen=aCheckLength;
	iLenExpected=aExpectedDataIn->Length();
	iLenReturned=0;

	iSocket.Recv(*iDataIn, iFlags, iStatus, iLenReturned);
	Activate( aAsyncErrorIndex );
	}
