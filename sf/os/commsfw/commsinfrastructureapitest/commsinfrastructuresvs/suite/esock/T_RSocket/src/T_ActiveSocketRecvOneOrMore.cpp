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

#include "T_ActiveSocketRecvOneOrMore.h"

/*@{*/
_LIT(KName,	"RecvOneOrMore");
/*@}*/

CActiveSocketRecvOneOrMore* CActiveSocketRecvOneOrMore::NewL(RSocket& aSocket, MActiveCallback& aCallback, TInt aPriority)
	{
	CActiveSocketRecvOneOrMore*	ret=new (ELeave) CActiveSocketRecvOneOrMore(aSocket, aCallback, aPriority);
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop();
	return ret;
	}

CActiveSocketRecvOneOrMore::CActiveSocketRecvOneOrMore(RSocket& aSocket, MActiveCallback& aCallback, TInt aPriority)
:	CActiveSocket( KName(), aSocket, aCallback, aPriority )
,	iCheckLen(EFalse)
	{
	}

void CActiveSocketRecvOneOrMore::ValidateCompletionL(CDataWrapperBase& aTestStep)
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

void CActiveSocketRecvOneOrMore::ExecuteL(HBufC8* aExpectedDataIn, TUint aFlags, TBool aCheckLength, TInt aBufferSize, const TInt aAsyncErrorIndex)
	{
	DestroyData();
	CreateDataL(aExpectedDataIn, aBufferSize);

	iFlags=aFlags;

	iCheckLen=aCheckLength;
	iLenExpected=aExpectedDataIn->Length();
	iLenReturned=0;

	iSocket.RecvOneOrMore(*iDataIn, iFlags, iStatus, iLenReturned);
	Activate( aAsyncErrorIndex );
	}
