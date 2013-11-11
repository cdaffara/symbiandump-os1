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
// CATGetImei

#include "atgetimei.h"
#include "mslogger.h"


_LIT8(KGetSerialNumCommand,"AT+CGSN\r");
_LIT(KSpaceSeparator," ");
// Class CATGetImei
// ---------------------------------------------------------------------------
// CATGetImei::NewL
// other items were commented in a header
// ---------------------------------------------------------------------------
CATGetImei* CATGetImei::NewL(CGlobalPhonemanager& aGloblePhone, 
	                         CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	CATGetImei* self = new(ELeave) CATGetImei(aGloblePhone,aCtsyDispatcherCallback);
	CleanupStack::PushL(self );
	self->ConstructL();
	CleanupStack::Pop(self );
	return self ;
	}
// ---------------------------------------------------------------------------
// CATGetImei::CATGetImei
// other items were commented in a header
// ---------------------------------------------------------------------------
CATGetImei::CATGetImei(CGlobalPhonemanager& aGloblePhone, 
	                   CCtsyDispatcherCallback& aCtsyDispatcherCallback)
		:CAtCommandBase(aGloblePhone,aCtsyDispatcherCallback)
	{
	}
// ---------------------------------------------------------------------------
// CATGetImei::ConstructL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATGetImei::ConstructL()
	{
	CAtCommandBase::ConstructL();
	iAtType = ELtsyAT_Phone_GetImei;
	}
// ---------------------------------------------------------------------------
// CATGetImei::~CATGetImei
// other items were commented in a header
// ---------------------------------------------------------------------------
CATGetImei::~CATGetImei()
	{
	}
// ---------------------------------------------------------------------------
// CATGetImei::StartRequestL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATGetImei::StartRequest()
	{
	ExecuteCommand();
	}
// ---------------------------------------------------------------------------
// CATGetImei::ExecuteCommand
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATGetImei::ExecuteCommand()
	{
	iTxBuffer.Copy(KGetSerialNumCommand);
	Write();
	}
// ---------------------------------------------------------------------------
// CATGetImei::CATGetImei
// other items were commented in a header
// ---------------------------------------------------------------------------
RMobilePhone::TMobilePhoneIdentityV1 CATGetImei::GetPhoneImei()
	{
	return iPhoneId;
	}
// ---------------------------------------------------------------------------
// CATGetImei::GetErrorValue
// other items were commented in a header
// ---------------------------------------------------------------------------
TInt CATGetImei::GetErrorValue()
	{
	return iError;
	}
// ---------------------------------------------------------------------------
// CATGetImei::ParseResponseL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATGetImei::ParseResponseL(const TDesC8& /*aResponseBuf*/)
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
	for(TInt i = 0; i < Count ; i++)
		{
		//lex.Assign(array[2]);
		
		if  (iMoreInfoFlag)
			{
			TBuf<RMobilePhone::KPhoneSerialNumberSize> Data;
			Data.Zero();
			TInt remainingBufferSize = RMobilePhone::KPhoneSerialNumberSize - 
			                           iPhoneId.iSerialNumber.Length() - 1;
			if(array[i].Length() > remainingBufferSize)
				{
				Data.Copy(array[i].Mid(0,remainingBufferSize));
				}
			else
				{
				Data.Copy(array[i]);
				}
			iPhoneId.iSerialNumber.Append(KSpaceSeparator);
			iPhoneId.iSerialNumber.Append(Data);
			}
		else
			{
			if(array[i].Length() > RMobilePhone::KPhoneSerialNumberSize)
				{
				iPhoneId.iSerialNumber.Copy(array[i].Mid(0,RMobilePhone::KPhoneSerialNumberSize));
				}
			else
				{
				iPhoneId.iSerialNumber.Copy(array[i]);
				}
			iMoreInfoFlag = ETrue;
			}
		}
	CleanupStack::PopAndDestroy();
	iError = KErrNone;
	}
//
// End of file

