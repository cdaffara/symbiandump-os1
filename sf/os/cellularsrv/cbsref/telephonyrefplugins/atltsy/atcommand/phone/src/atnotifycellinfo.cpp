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
// CATGetModemStatus
// Description:
// CATNotifyCellInfo

//user include
#include "atnotifycellinfo.h"

_LIT8(KATNotifyCellInfo,"+CCED:*");

// ---------------------------------------------------------------------------
// CATNotifyCellInfo::NewL
// other items were commented in a header
// ---------------------------------------------------------------------------
CATNotifyCellInfo* CATNotifyCellInfo::NewL(CGlobalPhonemanager& aGloblePhone,
		                             CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	CATNotifyCellInfo* self = CATNotifyCellInfo::NewLC(aGloblePhone, aCtsyDispatcherCallback);
	CleanupStack::Pop(self);
	return self;
	}

// ---------------------------------------------------------------------------
// CATNotifyCellInfo::NewLC
// other items were commented in a header
// ---------------------------------------------------------------------------
CATNotifyCellInfo* CATNotifyCellInfo::NewLC(CGlobalPhonemanager& aGloblePhone,
		                              CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	CATNotifyCellInfo* self = new (ELeave) CATNotifyCellInfo(aGloblePhone, aCtsyDispatcherCallback);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

// ---------------------------------------------------------------------------
// CATNotifyCellInfo::~CATNotifyCellInfo
// other items were commented in a header
// ---------------------------------------------------------------------------
CATNotifyCellInfo::~CATNotifyCellInfo()
	{
	
	}

// ---------------------------------------------------------------------------
// CATNotifyCellInfo::ConstructL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATNotifyCellInfo::ConstructL()
	{
	CAtCommandBase::ConstructL();
	RemoveAllExpectString();
	AddExpectStringL(KATNotifyCellInfo);
	Enable();
	}

// ---------------------------------------------------------------------------
// CATNotifyCellInfo::CATNotifyCellInfo
// other items were commented in a header
// ---------------------------------------------------------------------------
CATNotifyCellInfo::CATNotifyCellInfo(CGlobalPhonemanager& aGloblePhone,
		                       CCtsyDispatcherCallback& aCtsyDispatcherCallback)
							   :CAtCommandBase(aGloblePhone, aCtsyDispatcherCallback)
	{
	}
// ---------------------------------------------------------------------------
// CATNotifyCellInfo::CATNotifyCellInfo
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATNotifyCellInfo::Enable()
	{
	AddUnsolicitedAtCommand();
	}
// ---------------------------------------------------------------------------
// CATNotifyCellInfo::CATNotifyCellInfo
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATNotifyCellInfo::Disable()
	{
	RemoveAtCommand();
	}
// ---------------------------------------------------------------------------
// CATNotifyCellInfo::ParseResponseL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATNotifyCellInfo::ParseResponseL(const TDesC8& /*aResponseBuf*/)
	{
	
	RArray<TPtrC8> array;
	CleanupClosePushL(array);
	iParser->ParseRespondedBuffer(array,CurrentLine());
	ClearCurrentLine();
	TInt Count = array.Count();
	if (Count < 1)
		{
		CleanupStack::PopAndDestroy();
		iError = KErrNotFound;
		return;
		}
	if( array[0].MatchF(KATNotifyCellInfo)==KErrNotFound)
		{
		CleanupStack::PopAndDestroy();
		iError = KErrNotFound;
		return ;
		}
	TInt number = 0;
	TInt val = 0;
	TLex8 lexBasic(array[5]);
	lexBasic.Val(val);
	iCellInfo.iNmr[number].iBSIC = val;
	TLex8 lexArfcn(array[6]);
	lexArfcn.Val(val);
	iCellInfo.iNmr[number].iARFCN = val;
	TLex8 lexRxlev(array[7]);
	lexRxlev.Val(val);
	iCellInfo.iNmr[number].iRxLEV = val;
	
	
    for(TInt i = 14;i < Count-2;i++ )
    	{
    	number++;
    	i = i + 4;
    	TInt val = 0;
    	TLex8 lexBasic(array[i]);
    	lexBasic.Val(val);
    	i++;
    	iCellInfo.iNmr[number].iBSIC = val;
    	TLex8 lexArfcn(array[i]);
    	lexArfcn.Val(val);
    	iCellInfo.iNmr[number].iARFCN = val;
    	i++;
    	TLex8 lexRxlev(array[i]);
    	lexRxlev.Val(val);
    	iCellInfo.iNmr[number].iRxLEV = val;
    	}
	TLex8 lexTa(array[Count-2]);
	lexTa.Val(val);
    iCellInfo.iTA = val;
	
	
	CleanupStack::PopAndDestroy(&array);
	iError = KErrNone;
	}
// ---------------------------------------------------------------------------
// CATNotifyCellInfo::ParseResponseL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATNotifyCellInfo::EventSignal(TAtEventSource /*aEventSource*/, TInt /*aStatus*/)
	{
	iCtsyDispatcherCallback.CallbackPhoneNotifyCellInfoChangedInd(iError,iCellInfo);
	}
//End of file
