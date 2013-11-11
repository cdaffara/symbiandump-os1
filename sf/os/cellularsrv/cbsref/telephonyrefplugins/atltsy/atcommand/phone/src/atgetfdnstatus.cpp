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
// CATGetFdnStatus

//user include
#include "atgetfdnstatus.h"
#include "mslogger.h"


_LIT8(KGetFdnStatusCommand,"AT+CPBS?\r");
_LIT8(KGetFdnStatusCmdRepondStr,"+CPBS:");
_LIT8(KFdnString,"FD");
// Class CATGetFdnStatus
// ---------------------------------------------------------------------------
// CATGetFdnStatus::NewL
// other items were commented in a header
// ---------------------------------------------------------------------------
CATGetFdnStatus* CATGetFdnStatus::NewL(CGlobalPhonemanager& aGloblePhone, 
	                               CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	CATGetFdnStatus* self = new(ELeave) CATGetFdnStatus(aGloblePhone,
			                                        aCtsyDispatcherCallback);
	CleanupStack::PushL(self );
	self->ConstructL();
	CleanupStack::Pop(self );
	return self ;
	}
// ---------------------------------------------------------------------------
// CATGetFdnStatus::CATGetFdnStatus
// other items were commented in a header
// ---------------------------------------------------------------------------
CATGetFdnStatus::CATGetFdnStatus(CGlobalPhonemanager& aGloblePhone, 
	                         CCtsyDispatcherCallback& aCtsyDispatcherCallback)
		:CAtCommandBase(aGloblePhone,aCtsyDispatcherCallback)
	{
	}
// ---------------------------------------------------------------------------
// CATGetFdnStatus::ConstructL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATGetFdnStatus::ConstructL()
	{
	CAtCommandBase::ConstructL();
	iFdnStatus = EFalse;
	iAtType = ELtsyAT_Phone_GetFdnStatus;
	}
// ---------------------------------------------------------------------------
// CATGetFdnStatus::~CATGetFdnStatus
// other items were commented in a header
// ---------------------------------------------------------------------------
CATGetFdnStatus::~CATGetFdnStatus()
	{
	}
// ---------------------------------------------------------------------------
// CATGetFdnStatus::StartRequestL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATGetFdnStatus::StartRequest()
	{
	ExecuteCommand();
	}
// ---------------------------------------------------------------------------
// CATGetFdnStatus::ExecuteCommand
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATGetFdnStatus::ExecuteCommand()
	{
	iTxBuffer.Copy(KGetFdnStatusCommand);
	Write();
	}
// ---------------------------------------------------------------------------
// CATGetFdnStatus::CATGetFdnStatus
// other items were commented in a header
// ---------------------------------------------------------------------------
TBool CATGetFdnStatus::GetFdnStatus()
	{
	return iFdnStatus;
	}

// ---------------------------------------------------------------------------
// CATGetFdnStatus::GetErrorValue
// other items were commented in a header
// ---------------------------------------------------------------------------
TInt CATGetFdnStatus::GetErrorValue()
	{
	return iError;
	}
// ---------------------------------------------------------------------------
// CATGetFdnStatus::ParseResponseL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATGetFdnStatus::ParseResponseL(const TDesC8& /*aResponseBuf*/)
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
	if( array[0].MatchF(KGetFdnStatusCmdRepondStr)==KErrNotFound)
		{
		CleanupStack::PopAndDestroy();
		iError = KErrNotFound;
		return ;
		}
	if( array[1].MatchF(KFdnString)!=KErrNotFound)
		{
		iFdnStatus = ETrue;
		}
	
	CleanupStack::PopAndDestroy(&array);
	iError = KErrNone;
	}

//
// End of file
