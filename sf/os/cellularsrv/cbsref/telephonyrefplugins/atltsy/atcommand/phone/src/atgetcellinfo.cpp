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
// CATGetCellInfo

//user include
#include "atgetcellinfo.h"
#include "mslogger.h"

_LIT8(KGetCellInfoCommand,"AT+CCED=0,7\r");
_LIT8(KGetCellInfoCmdRepondStr,"+CCED:");
// Class CATGetCellInfo
// ---------------------------------------------------------------------------
// CATGetCellInfo::NewL
// other items were commented in a header
// ---------------------------------------------------------------------------
CATGetCellInfo* CATGetCellInfo::NewL(CGlobalPhonemanager& aGloblePhone, 
	                                 CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	CATGetCellInfo* self = new(ELeave) CATGetCellInfo(aGloblePhone,
			                                          aCtsyDispatcherCallback);
	CleanupStack::PushL(self );
	self->ConstructL();
	CleanupStack::Pop(self );
	return self ;
	}
// ---------------------------------------------------------------------------
// CATGetCellInfo::CATGetCellInfo
// other items were commented in a header
// ---------------------------------------------------------------------------
CATGetCellInfo::CATGetCellInfo(CGlobalPhonemanager& aGloblePhone, 
	                           CCtsyDispatcherCallback& aCtsyDispatcherCallback)
		:CAtCommandBase(aGloblePhone,aCtsyDispatcherCallback)
	{
	}
// ---------------------------------------------------------------------------
// CATGetCellInfo::ConstructL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATGetCellInfo::ConstructL()
	{
	CAtCommandBase::ConstructL();
	}
// ---------------------------------------------------------------------------
// CATGetCellInfo::~CATGetCellInfo(
// other items were commented in a header
// ---------------------------------------------------------------------------
CATGetCellInfo::~CATGetCellInfo()
	{
	}
// ---------------------------------------------------------------------------
// CATGetCellInfo::StartRequestL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATGetCellInfo::StartRequest()
	{
	ExecuteCommand();
	}
// ---------------------------------------------------------------------------
// CATGetCellInfo::ExecuteCommand
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATGetCellInfo::ExecuteCommand()
	{
	iTxBuffer.Copy(KGetCellInfoCommand);
	Write();
	}
// ---------------------------------------------------------------------------
// CATGetCellInfo::ParseResponseL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATGetCellInfo::ParseResponseL(const TDesC8& /*aResponseBuf*/)
	{
	if (CurrentLine().Match(KLtsyOkString) != 0)
		{
		iError = KErrGeneral;
		return ;
		}
	RArray<TPtrC8> array;
	CleanupClosePushL(array);
	iParser->ParseRespondedBuffer(array,PrecedingLine());
	TInt Count = array.Count();
	if (Count < 1)
		{
		CleanupStack::PopAndDestroy();
		iError = KErrNotFound;
		return;
		}
	if( array[0].MatchF(KGetCellInfoCmdRepondStr)==KErrNotFound)
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
// CATGetCellInfo::EventSignal
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATGetCellInfo::EventSignal(TAtEventSource aEventSource, TInt aStatus)
	{
	if(KErrNone ==aStatus)
		{
		if(aEventSource == EReadCompletion)
			{
			aStatus = iError;
			}
		else if(aEventSource == EWriteCompletion)
		    {
		    return;
		    }
		}
	
	// complete this request
	CAtCommandBase::Complete();
	iPhoneGlobals.iEventSignalActive = EFalse;
	iCtsyDispatcherCallback.CallbackPhoneGetCellInfoComp(aStatus,iCellInfo);
	if(iIpcCompleteObserver)
	    {
	    iIpcCompleteObserver->NotifyIpcRequestComplete(iIpcId);
	    }
	}
//
// End of file
