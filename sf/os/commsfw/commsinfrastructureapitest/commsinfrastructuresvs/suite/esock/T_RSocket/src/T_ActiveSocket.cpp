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

#include "T_ActiveSocket.h"

CActiveSocket::CActiveSocket(const TDesC& aName, RSocket& aSocket, MActiveCallback& aCallback, TInt aPriority )
:CActiveCallback( aCallback, aPriority )
,	iSocket(aSocket)
,	iDataIn(NULL)
,	iName(aName)
,	iDataReturned(NULL)
,	iExpectedDataIn(NULL)
	{
	}

CActiveSocket::~CActiveSocket()
	{
	DestroyData();
	}
	
void CActiveSocket::DestroyData()
	{
	delete iExpectedDataIn;
	iExpectedDataIn=NULL;

	delete iDataIn;
	iDataIn=NULL;

	delete iDataReturned;
	iDataReturned=NULL;
	}

void CActiveSocket::CreateDataL(const TDesC* aExpectedDataIn, TInt aBufferSize)
	{
	TInt	length=aBufferSize;
	if ( aExpectedDataIn!=NULL )
		{
		iExpectedDataIn=HBufC8::NewL(aExpectedDataIn->Length());
		iExpectedDataIn->Des().Copy(*aExpectedDataIn);
		if ( length<aExpectedDataIn->Length() )
			{
			length=aExpectedDataIn->Length();
			}
		}
	iDataReturned=HBufC8::NewL(length);
	iDataIn=new (ELeave) TPtr8(iDataReturned->Des());
	iDataIn->Set(const_cast<TUint8 *>(iDataReturned->Ptr()), 0, length);
	}

void CActiveSocket::CreateDataL(HBufC8* aExpectedDataIn, TInt aBufferSize)
	{
	TInt	length=aBufferSize;
	if ( aExpectedDataIn!=NULL )
		{
		iExpectedDataIn=aExpectedDataIn;
		if ( length<aExpectedDataIn->Length() )
			{
			length=aExpectedDataIn->Length();
			}
		}
	iDataReturned=HBufC8::NewL(length);
	iDataIn=new (ELeave) TPtr8(iDataReturned->Des());
	iDataIn->Set(const_cast<TUint8 *>(iDataReturned->Ptr()), 0, length);
	}

void CActiveSocket::ValidateCompletionL(CDataWrapperBase& aTestStep)
	{
	aTestStep.INFO_PRINTF2(_L("Name:%S"), &iName);
	if ( iDataReturned!=NULL )
		{
		HBufC*	dataReceived = HBufC::NewLC(iDataIn->Length());
		dataReceived->Des().Copy(*iDataIn);
		aTestStep.INFO_PRINTF2(_L("Data Received: [%S]"), dataReceived);

		TInt	length=0;
		if ( iExpectedDataIn!=NULL )
			{
			length=iExpectedDataIn->Length();
			}
		if (  length != 0 )
			{
			HBufC*	expectedData = HBufC::NewLC(iExpectedDataIn->Length());
			expectedData->Des().Copy(*iExpectedDataIn);
			if ( *dataReceived!=*expectedData )
				{
				aTestStep.ERR_PRINTF3(_L("Received [%S], expected [%S]"), dataReceived, expectedData);
				aTestStep.SetBlockResult(EFail);
				}
			CleanupStack::PopAndDestroy(expectedData);
			}
		CleanupStack::PopAndDestroy(dataReceived);
		}
	}
