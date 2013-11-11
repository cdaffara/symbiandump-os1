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


#include "ActiveCallbackWrap.h"


CActiveCallbackWrap::CActiveCallbackWrap(TInt aFunctionId, TInt aExpectedAsyncError) 
: iFunctionId(aFunctionId)
, iActiveCallback(NULL) 
, iExpectedAsyncError(aExpectedAsyncError)
, iData(NULL)
, iExpectedData(NULL)
, iDataPtr(NULL, 0)
, iExpectedDataPtr(NULL, 0)
	{
	}

void CActiveCallbackWrap::SetDesData(HBufC8* aData)
	{
	if (aData)
		{
		iData = aData;
		iDataPtr.Set(aData->Des());		
		}
	}

void CActiveCallbackWrap::SetExpectedDesData(HBufC8* aData)
	{
	if (aData)
		{
		iExpectedData = aData;
		iExpectedDataPtr.Set(aData->Des());		
		}
	}

CActiveCallbackWrap* CActiveCallbackWrap::NewLC(MActiveCallback& aCallback, TInt aFunctionId, TInt aExpectedAsyncError)
	{
	CActiveCallbackWrap* self = new (ELeave) CActiveCallbackWrap(aFunctionId, aExpectedAsyncError);
	CleanupStack::PushL(self);
	self->ConstructL(aCallback);
	
	return self;
	}

CActiveCallbackWrap* CActiveCallbackWrap::NewL(MActiveCallback& aCallback, TInt aFunctionId, TInt aExpectedAsyncError)
	{
	CActiveCallbackWrap* self = CActiveCallbackWrap::NewLC(aCallback, aFunctionId, aExpectedAsyncError);
	CleanupStack::Pop(self);
	
	return self;
	}

CActiveCallbackWrap::~CActiveCallbackWrap()
	{
	delete iActiveCallback;
	delete iData;
	delete iExpectedData;
	}


void CActiveCallbackWrap::ConstructL(MActiveCallback& aCallback)
	{
	iActiveCallback = CActiveCallback::NewL(aCallback, CActive::EPriorityStandard);	
	}
