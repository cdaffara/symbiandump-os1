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


#ifndef ACTIVECALLBACKWRAP_H_
#define ACTIVECALLBACKWRAP_H_


//#include <e32base.h>

#include "activecallback.h"


class CActiveCallbackWrap
	{
public:

	static CActiveCallbackWrap* NewLC(MActiveCallback& aCallback, TInt aFunctionId, TInt aExpectedAsyncError = KErrNone);
	static CActiveCallbackWrap* NewL(MActiveCallback& aCallback, TInt aFunctionId, TInt aExpectedAsyncError = KErrNone);
	virtual ~CActiveCallbackWrap();


	CActiveCallback& ActiveCallback() { return *iActiveCallback ; }
	TInt FunctionId() const { return iFunctionId; }
	TInt ExpectedAsyncError() const { return iExpectedAsyncError; }
	void SetDesData(HBufC8* aData);
	void SetExpectedDesData(HBufC8* aData);
	HBufC8* Data() { return iData; }
	HBufC8* ExpectedData() { return iExpectedData; }
	TPtr8& DataPtr() { return iDataPtr; }
	TPtr8& ExpectedDataPtr() { return iExpectedDataPtr; }

protected:
	CActiveCallbackWrap(TInt aFunctionId, TInt aExpectedAsyncError);
	void ConstructL(MActiveCallback& aCallback);
	
private:	
	TInt				iFunctionId;
	CActiveCallback*	iActiveCallback;
	TInt				iExpectedAsyncError;
	HBufC8*				iData;
	HBufC8*				iExpectedData;
	TPtr8				iDataPtr;
	TPtr8				iExpectedDataPtr;
	};

#endif /*ACTIVECALLBACKWRAP_H_*/
