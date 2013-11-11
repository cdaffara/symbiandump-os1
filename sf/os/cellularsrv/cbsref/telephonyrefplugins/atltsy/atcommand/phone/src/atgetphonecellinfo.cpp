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
// CATGetPhoneCellInfo

#include "atgetphonecellinfo.h"
#include "mslogger.h"

_LIT8(KGetPhoneCellInfoCommand,"AT+CCED=0,5\r");
_LIT8(KGetPhoneCellInfoCmdRepondStr,"+CCED:");
// Class CATGetPhoneCellInfo
// ---------------------------------------------------------------------------
// CATGetPhoneCellInfo::NewL
// other CATGetPhoneCellInfo were commented in a header
// ---------------------------------------------------------------------------
CATGetPhoneCellInfo* CATGetPhoneCellInfo::NewL(CGlobalPhonemanager& aGloblePhone, 
	                                           CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	CATGetPhoneCellInfo* self = new(ELeave) CATGetPhoneCellInfo(aGloblePhone,
			                                                    aCtsyDispatcherCallback);
	CleanupStack::PushL(self );
	self->ConstructL();
	CleanupStack::Pop(self );
	return self ;
	}
// ---------------------------------------------------------------------------
// CATGetPhoneCellInfo::CATGetPhoneCellInfo
// other CATGetPhoneCellInfo were commented in a header
// ---------------------------------------------------------------------------
CATGetPhoneCellInfo::CATGetPhoneCellInfo(CGlobalPhonemanager& aGloblePhone, 
                                         CCtsyDispatcherCallback& aCtsyDispatcherCallback)
		:CAtCommandBase(aGloblePhone,aCtsyDispatcherCallback)
	{
	}
// ---------------------------------------------------------------------------
// CATGetPhoneCellInfo::ConstructL
// other CATGetPhoneCellInfo were commented in a header
// ---------------------------------------------------------------------------
void CATGetPhoneCellInfo::ConstructL()
	{
	CAtCommandBase::ConstructL();
	}
// ---------------------------------------------------------------------------
// CATGetPhoneCellInfo::~CATGetPhoneCellInfo
// other CATGetPhoneCellInfo were commented in a header
// ---------------------------------------------------------------------------
CATGetPhoneCellInfo::~CATGetPhoneCellInfo()
	{
	}
// ---------------------------------------------------------------------------
// CATGetPhoneCellInfo::StartRequestL
// other CATGetPhoneCellInfo were commented in a header
// ---------------------------------------------------------------------------
void CATGetPhoneCellInfo::StartRequest()
	{
	ExecuteCommand();
	}
// ---------------------------------------------------------------------------
// CATGetPhoneCellInfo::ExecuteCommand
// other CATGetPhoneCellInfo were commented in a header
// ---------------------------------------------------------------------------
void CATGetPhoneCellInfo::ExecuteCommand()
	{
	iTxBuffer.Copy(KGetPhoneCellInfoCommand);
	Write();
	}

// ---------------------------------------------------------------------------
// CATGetPhoneCellInfo::ParseResponseL
// other CATGetPhoneCellInfo were commented in a header
// ---------------------------------------------------------------------------
void CATGetPhoneCellInfo::ParseResponseL(const TDesC8& /*aResponseBuf*/)
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
	if( array[0].MatchF(KGetPhoneCellInfoCmdRepondStr)==KErrNotFound)
		{
		CleanupStack::PopAndDestroy();
		iError = KErrNotFound;
		return ;
		}
	iCellInfo.iMode = RMobilePhone::ECellModeGsm;
	TInt val = 0;
	TLex8 lex(array[4]);
	lex.Val(val);
	iCellInfo.iCellId = val;
	TLex8 lex1(array[14]);
	lex1.Val(val);
	iCellInfo.iTimingAdvance = val;
	
	
	CleanupStack::PopAndDestroy(&array);
	iError = KErrNone;
	}
// ---------------------------------------------------------------------------
// CATGetPhoneCellInfo::EventSignal
// other CATGetPhoneCellInfo were commented in a header
// ---------------------------------------------------------------------------
void CATGetPhoneCellInfo::EventSignal(TAtEventSource aEventSource, TInt aStatus)
	{
	// handle IO error
	if(aStatus != KErrNone)
	    {
	    iPhoneGlobals.iEventSignalActive = EFalse;
        iCtsyDispatcherCallback.CallbackPhoneGetPhoneCellInfoComp(aStatus,iCellInfo);
        CAtCommandBase::Complete();
        if(iIpcCompleteObserver)
            {
            iIpcCompleteObserver->NotifyIpcRequestComplete(iIpcId);
            }
	    }
	else if(aEventSource == EReadCompletion)
	    {
        iPhoneGlobals.iEventSignalActive = EFalse;
        iCtsyDispatcherCallback.CallbackPhoneGetPhoneCellInfoComp(iError,iCellInfo);
        CAtCommandBase::Complete();
        if(iIpcCompleteObserver)
            {
            iIpcCompleteObserver->NotifyIpcRequestComplete(iIpcId);
            }
	    }
	}
//
// End of file
