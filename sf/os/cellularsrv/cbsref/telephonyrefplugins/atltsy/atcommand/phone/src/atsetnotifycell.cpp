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
// CATSetNotifyCellInfo

#include "atsetnotifycell.h"
#include "mslogger.h"
#include "atgetphonecellinfo.h"
#include "atnotifycellinfo.h"


_LIT8(KATSetCellCommand, "AT+CCED=0,1\r");
// Class CATSetNotifyCellInfo
// ---------------------------------------------------------------------------
// CATSetNotifyCellInfo::NewL
// other items were commented in a header
// ---------------------------------------------------------------------------
CATSetNotifyCellInfo* CATSetNotifyCellInfo::NewL(CGlobalPhonemanager& aGloblePhone, 
	                                             CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	CATSetNotifyCellInfo* self = new(ELeave) CATSetNotifyCellInfo(aGloblePhone,
			                                                      aCtsyDispatcherCallback);
	CleanupStack::PushL(self );
	self->ConstructL();
	CleanupStack::Pop(self );
	return self ;
	}
// ---------------------------------------------------------------------------
// CATSetNotifyCellInfo::CATSetNotifyCellInfo
// other items were commented in a header
// ---------------------------------------------------------------------------
CATSetNotifyCellInfo::CATSetNotifyCellInfo(CGlobalPhonemanager& aGloblePhone, 
	                                      CCtsyDispatcherCallback& aCtsyDispatcherCallback)
		:CAtCommandBase(aGloblePhone,aCtsyDispatcherCallback)
	{
	}
// ---------------------------------------------------------------------------
// CATSetNotifyCellInfo::ConstructL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATSetNotifyCellInfo::ConstructL()
	{
	CAtCommandBase::ConstructL();
	}
// ---------------------------------------------------------------------------
// CATSetNotifyCellInfo::~CATSetNotifyCellInfo
// other items were commented in a header
// ---------------------------------------------------------------------------
CATSetNotifyCellInfo::~CATSetNotifyCellInfo()
	{
	}
// ---------------------------------------------------------------------------
// CATSetNotifyCellInfo::StartRequestL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATSetNotifyCellInfo::StartRequest()
	{
	ExecuteCommand();
	}
// ---------------------------------------------------------------------------
// CATSetNotifyCellInfo::ExecuteCommand
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATSetNotifyCellInfo::ExecuteCommand()
	{
	iTxBuffer.Copy(KATSetCellCommand);
	Write();
	}
// ---------------------------------------------------------------------------
// CATSetNotifyCellInfo::ParseResponseL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATSetNotifyCellInfo::ParseResponseL(const TDesC8& /*aResponseBuf*/)
	{
	if (CurrentLine().Match(KLtsyOkString) == 0)
		{
		iError = KErrNone;
		}
	else
		{
		iError = KErrGeneral;
		}
	}
//// ---------------------------------------------------------------------------
//// CATSetNotifyCellInfo::GetErrorValue
//// other items were commented in a header
//// ---------------------------------------------------------------------------
TInt CATSetNotifyCellInfo::GetErrorValue()
	{
	return iError;
	}
// ---------------------------------------------------------------------------
// CATSetNotifyCellInfo::EventSignal
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATSetNotifyCellInfo::EventSignal(TAtEventSource aEventSource, TInt aStatus)
	{
	if(KErrNone == aStatus)
		{
		if(aEventSource == EReadCompletion)
			{
			aStatus = iError;
			}
		else
		    {
		    return;
		    }
		}
	
	CAtCommandBase::Complete();
	iCtsyDispatcherCallback.CallbackPhoneRegisterCellInfoChangeNotificationComp(aStatus);
	if(iIpcCompleteObserver)
	    {
	    iIpcCompleteObserver->NotifyIpcRequestComplete(iIpcId);
	    }
	}
//
// End of file
