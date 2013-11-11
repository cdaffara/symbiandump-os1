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

#include "T_ActiveSocketRecvFrom.h"

/*@{*/
_LIT(KName,	"RecvFrom");
/*@}*/

CActiveSocketRecvFrom* CActiveSocketRecvFrom::NewL(RSocket& aSocket, MActiveCallback& aCallback, TInt aPriority)
	{
	CActiveSocketRecvFrom*	ret=new (ELeave) CActiveSocketRecvFrom(aSocket, aCallback, aPriority);
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop();
	return ret;
	}

CActiveSocketRecvFrom::CActiveSocketRecvFrom(RSocket& aSocket, MActiveCallback& aCallback, TInt aPriority)
:	CActiveSocket( KName(), aSocket, aCallback, aPriority )
,	iCheckLen(EFalse)
	{
	}

void CActiveSocketRecvFrom::ValidateCompletionL(CDataWrapperBase& aTestStep)
	{
	TBuf<KMaxIPAddressLength>	actualAddrString;
	iAddr.Output(actualAddrString);
	aTestStep.INFO_PRINTF3(_L("Returned IP Address: [%S:%d]"), &actualAddrString, iAddr.Port());

	if ( iCheckAddress )
		{
		if( iAddr.Address()!=iExpectedAddr.Address() )
			{
			TBuf<KMaxIPAddressLength>	expectedAddrString;
			iExpectedAddr.Output(expectedAddrString);
			aTestStep.ERR_PRINTF3(_L("Returned IP Address: %S != expected IP Address: %S"), &actualAddrString, &expectedAddrString);
			aTestStep.SetBlockResult(EFail);
			}
		}

	if ( iCheckPort )
		{
		if( iAddr.Port() != iExpectedAddr.Port() )
			{
			aTestStep.ERR_PRINTF3(_L("Returned port: %d != expected port: %d"), iAddr.Port(), iExpectedAddr.Port());
			aTestStep.SetBlockResult(EFail);
			}
		}

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

void CActiveSocketRecvFrom::ExecuteL(HBufC8* aExpectedDataIn, TUint aFlags, TInetAddr aExpectedAddr, TBool aCheckAddress, TBool aCheckPort, TInt aBufferSize, const TInt aAsyncErrorIndex)
	{
	DestroyData();
	CreateDataL(aExpectedDataIn, aBufferSize);

	iFlags=aFlags;
	iExpectedAddr=aExpectedAddr;
	iCheckAddress=aCheckAddress;
	iCheckPort=aCheckPort;

	iCheckLen=EFalse;

	iSocket.RecvFrom(*iDataIn, iAddr, iFlags, iStatus);
	Activate( aAsyncErrorIndex );
	}

void CActiveSocketRecvFrom::ExecuteL(HBufC8* aExpectedDataIn, TUint aFlags, TInetAddr aExpectedAddr, TBool aCheckAddress, TBool aCheckPort, TBool aCheckLength, TInt aBufferSize, const TInt aAsyncErrorIndex)
	{
	DestroyData();
	CreateDataL(aExpectedDataIn, aBufferSize);

	iFlags=aFlags;
	iExpectedAddr=aExpectedAddr;
	iCheckAddress=aCheckAddress;
	iCheckPort=aCheckPort;

	iCheckLen=aCheckLength;
	iLenExpected=aExpectedDataIn->Length();
	iLenReturned=0;

	iSocket.RecvFrom(*iDataIn, iAddr, iFlags, iStatus, iLenReturned);
	Activate( aAsyncErrorIndex );
	}
