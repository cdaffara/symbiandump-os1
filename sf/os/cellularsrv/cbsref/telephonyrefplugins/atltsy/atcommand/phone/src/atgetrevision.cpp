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
// CATGetRevision

#include "atgetrevision.h"
#include "mslogger.h"

_LIT8(KGetRevisionCommand,"AT+CGMR\r");
_LIT(KSpaceSeparator," ");
// CATGetRevision
// ---------------------------------------------------------------------------
// CATGetRevision::NewL
// other CATGetPhoneCellInfo were commented in a header
// ---------------------------------------------------------------------------
CATGetRevision* CATGetRevision::NewL(CGlobalPhonemanager& aGloblePhone, 
	                                 CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	CATGetRevision* self = new(ELeave) CATGetRevision(aGloblePhone,aCtsyDispatcherCallback);
	CleanupStack::PushL(self );
	self->ConstructL();
	CleanupStack::Pop(self );
	return self ;
	}
// ---------------------------------------------------------------------------
// CATGetRevision::CATGetRevision
// other CATGetPhoneCellInfo were commented in a header
// ---------------------------------------------------------------------------
CATGetRevision::CATGetRevision(CGlobalPhonemanager& aGloblePhone, 
	                           CCtsyDispatcherCallback& aCtsyDispatcherCallback)
		:CAtCommandBase(aGloblePhone,aCtsyDispatcherCallback)
	{
	}
// ---------------------------------------------------------------------------
// CATGetRevision::ConstructL
// other CATGetPhoneCellInfo were commented in a header
// ---------------------------------------------------------------------------
void CATGetRevision::ConstructL()
	{
	CAtCommandBase::ConstructL();
	iAtType = ELtsyAT_Phone_GetRevision;
	}
// ---------------------------------------------------------------------------
// CATGetRevision::~CATGetRevision
// other CATGetPhoneCellInfo were commented in a header
// ---------------------------------------------------------------------------
CATGetRevision::~CATGetRevision()
	{
	}
// ---------------------------------------------------------------------------
// CATGetRevision::StartRequest
// other CATGetPhoneCellInfo were commented in a header
// ---------------------------------------------------------------------------
void CATGetRevision::StartRequest()
	{
	ExecuteCommand();
	}
// ---------------------------------------------------------------------------
// CATGetRevision::ExecuteCommand
// other CATGetPhoneCellInfo were commented in a header
// ---------------------------------------------------------------------------
void CATGetRevision::ExecuteCommand( )
	{
	iTxBuffer.Copy(KGetRevisionCommand);
	Write();
	}
// ---------------------------------------------------------------------------
// CATGetRevision::GetPhoneRevision
// other CATGetPhoneCellInfo were commented in a header
// ---------------------------------------------------------------------------
RMobilePhone::TMobilePhoneIdentityV1 CATGetRevision::GetPhoneRevision()
	{
	return iPhoneId;
	}
// ---------------------------------------------------------------------------
// CATGetRevision::GetErrorValue
// other CATGetPhoneCellInfo were commented in a header
// ---------------------------------------------------------------------------
TInt CATGetRevision::GetErrorValue()
	{
	return iError;
	}
// ---------------------------------------------------------------------------
// CATGetRevision::ParseResponseL
// other CATGetPhoneCellInfo were commented in a header
// ---------------------------------------------------------------------------
void CATGetRevision::ParseResponseL(const TDesC8& /*aResponseBuf*/)
	{
	if (CurrentLine().Match(KLtsyOkString) != 0)
		{
		iError = KErrGeneral;
		return ;
		}
	RArray<TPtrC8> array;
	CleanupClosePushL(array);
	iParser->ParseRespondedBuffer(array,PrecedingLine());
	iMoreInfoFlag = EFalse;
	TInt Count = array.Count();
	if (Count < 1)
		{
		CleanupStack::PopAndDestroy();
		iError = KErrGeneral;
		return ;
		}
	for(TInt i = 0; i < Count; i++)
		{
		//lex.Assign(array[2]);
		
		if  (iMoreInfoFlag)
			{
			TBuf<RMobilePhone::KPhoneSerialNumberSize> Data;
			Data.Zero();
			TInt remainingBufferSize = RMobilePhone::KPhoneSerialNumberSize - 
			                           iPhoneId.iRevision.Length() - 1;
			if(array[i].Length() > remainingBufferSize)
				{
				Data.Copy(array[i].Mid(0,remainingBufferSize));
				}
			else
				{
				Data.Copy(array[i]);
				}
			iPhoneId.iRevision.Append(KSpaceSeparator);
			iPhoneId.iRevision.Append(Data);
			}
		else
			{
			if(array[i].Length() > RMobilePhone::KPhoneSerialNumberSize)
				{
				iPhoneId.iRevision.Copy(array[i].Mid(0,RMobilePhone::KPhoneSerialNumberSize));
				}
			else
				{
				iPhoneId.iRevision.Copy(array[i]);
				}
			iMoreInfoFlag = ETrue;
			}
		}
	iMoreInfoFlag = EFalse;
	CleanupStack::PopAndDestroy();
	iError = KErrNone;
	}
//
// End of file

