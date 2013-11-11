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
// CATGetManufacturer

#include "atgetmanufacturer.h"
#include "mslogger.h"


_LIT8(KGetManufacturerCommand,"AT+CGMI\r");
_LIT(KSpaceSeparator," ");
// Class CATGetManufacturer
// ---------------------------------------------------------------------------
// CATGetManufacturer::NewL
// other items were commented in a header
// ---------------------------------------------------------------------------
CATGetManufacturer* CATGetManufacturer::NewL(CGlobalPhonemanager& aGloblePhone, 
	                                         CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	CATGetManufacturer* self = new(ELeave) CATGetManufacturer(aGloblePhone,
			                                                  aCtsyDispatcherCallback);
	CleanupStack::PushL(self );
	self->ConstructL();
	CleanupStack::Pop(self );
	return self ;
	}

// ---------------------------------------------------------------------------
// CATGetManufacturer::CATGetManufacturer
// other items were commented in a header
// ---------------------------------------------------------------------------
CATGetManufacturer::CATGetManufacturer(CGlobalPhonemanager& aGloblePhone, 
	                                   CCtsyDispatcherCallback& aCtsyDispatcherCallback)
		:CAtCommandBase(aGloblePhone,aCtsyDispatcherCallback)
	{
	}

// ---------------------------------------------------------------------------
// CATGetManufacturer::ConstructL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATGetManufacturer::ConstructL()
	{
	CAtCommandBase::ConstructL();
	iAtType = ELtsyAT_Phone_GetManufacturer;
	}

// ---------------------------------------------------------------------------
// CATGetManufacturer::~CATGetManufacturer
// other items were commented in a header
// ---------------------------------------------------------------------------
CATGetManufacturer::~CATGetManufacturer()
	{
	}

// ---------------------------------------------------------------------------
// CATGetManufacturer::StartRequestL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATGetManufacturer::StartRequest()
	{
	ExecuteCommand();
	}

// ---------------------------------------------------------------------------
// CATGetManufacturer::ExecuteCommand
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATGetManufacturer::ExecuteCommand()
	{
	iTxBuffer.Copy(KGetManufacturerCommand);
	Write();
	}

// ---------------------------------------------------------------------------
// CATGetManufacturer::GetPhoneManufacturer
// other items were commented in a header
// ---------------------------------------------------------------------------
RMobilePhone::TMobilePhoneIdentityV1 CATGetManufacturer::GetPhoneManufacturer()
	{
	return iPhoneId;
	}

// ---------------------------------------------------------------------------
// CATGetManufacturer::GetErrorValue
// other items were commented in a header
// ---------------------------------------------------------------------------
TInt CATGetManufacturer::GetErrorValue()
	{
	return iError;
	}

// ---------------------------------------------------------------------------
// CATGetManufacturer::ParseResponseL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATGetManufacturer::ParseResponseL(const TDesC8& /*aResponseBuf*/)
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
			                           iPhoneId.iManufacturer.Length() - 1;
			if(array[i].Length() > remainingBufferSize)
				{
				Data.Copy(array[i].Mid(0,remainingBufferSize));
				}
			else
				{
				Data.Copy(array[i]);
				}
			iPhoneId.iManufacturer.Append(KSpaceSeparator);
			iPhoneId.iManufacturer.Append(Data);
			}
		else
			{
			if(array[i].Length() > RMobilePhone::KPhoneSerialNumberSize)
				{
				iPhoneId.iManufacturer.Copy(array[i].Mid(0,RMobilePhone::KPhoneSerialNumberSize));
				}
			else
				{
				iPhoneId.iManufacturer.Copy(array[i]);
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

