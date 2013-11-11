// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// This contains CATUnsolicited
// 

//system include

//user include
#include "atunsolicited.h"

// ---------------------------------------------------------------------------
// CATUnsolicited::NewL
// other items were commented in a header
// ---------------------------------------------------------------------------
CATUnsolicited* CATUnsolicited::NewL(CGlobalPhonemanager& aGloblePhone,
		                             CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	CATUnsolicited* self = CATUnsolicited::NewLC(aGloblePhone, aCtsyDispatcherCallback);
	CleanupStack::Pop(self);
	return self;
	}

// ---------------------------------------------------------------------------
// CATUnsolicited::NewLC
// other items were commented in a header
// ---------------------------------------------------------------------------
CATUnsolicited* CATUnsolicited::NewLC(CGlobalPhonemanager& aGloblePhone,
		                              CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	CATUnsolicited* self = new (ELeave) CATUnsolicited(aGloblePhone, aCtsyDispatcherCallback);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

// ---------------------------------------------------------------------------
// CATUnsolicited::~CATUnsolicited
// other items were commented in a header
// ---------------------------------------------------------------------------
CATUnsolicited::~CATUnsolicited()
	{
	
	}

// ---------------------------------------------------------------------------
// CATUnsolicited::ConstructL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATUnsolicited::ConstructL()
	{
	CAtCommandBase::ConstructL();
	RemoveAllExpectString();
	AddExpectStringL(KLtsyGeneralIndMatchString);
	}

// ---------------------------------------------------------------------------
// CATUnsolicited::CATUnsolicited
// other items were commented in a header
// ---------------------------------------------------------------------------
CATUnsolicited::CATUnsolicited(CGlobalPhonemanager& aGloblePhone,
		                       CCtsyDispatcherCallback& aCtsyDispatcherCallback)
							   :CAtCommandBase(aGloblePhone, aCtsyDispatcherCallback)
	{
	iAtType = ELtsyAT_General_WIND;
	iResult = KErrNone;
	}

// ---------------------------------------------------------------------------
// CATUnsolicited::GetParserResult
// other items were commented in a header
// ---------------------------------------------------------------------------
TInt CATUnsolicited::GetParserResult() const
	{
	return iResult;
	}

// ---------------------------------------------------------------------------
//  CATUnsolicited::GetUnsolicitedParams
// other items were commented in a header
// ---------------------------------------------------------------------------
const TDesC8& CATUnsolicited::GetUnsolicitedParams() const
	{
	return iPckgBuf;
	}

// ---------------------------------------------------------------------------
// CATUnsolicited::ParseResponseL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATUnsolicited::ParseResponseL(const TDesC8& aResponseBuf)
	{
	iResult = KErrNone;
	
	RArray<TPtrC8> rArray;
	CleanupClosePushL(rArray);
	
	iParser->ParseRespondedBuffer(rArray, aResponseBuf);
	
	TUnsolicitedParams& tParams(iPckgBuf());
	tParams.InitParams();
	
	TInt nCount = rArray.Count();
	if (nCount <= 1)
		{
		iResult = KErrGeneral;
		CleanupStack::PopAndDestroy(1);
		return;
		}
	
	if (nCount >= 2)
		{
		TInt tVal(0);
		TLex8 tLex(rArray[1]);
		TInt nRes = tLex.Val(tVal);
		if (nRes == KErrNone)
			{
			tParams.iEvent = tVal;
			}
		else
			{
			iResult = nRes;
			CleanupStack::PopAndDestroy(1);
			return;
			}
		}
	
	if (nCount == 3 && tParams.iEvent != 10 && tParams.iEvent != 11)
		{
		TInt tVal(0);
		TLex8 tLex(rArray[2]);
		TInt nRes = tLex.Val(tVal);
		if (nRes == KErrNone)
			{
			tParams.iIdx = tVal;
			}
		else
			{
			iResult = nRes;
			CleanupStack::PopAndDestroy(1);	
			return;
			}
		}
	CleanupStack::PopAndDestroy(1);	
	}

//End of file
